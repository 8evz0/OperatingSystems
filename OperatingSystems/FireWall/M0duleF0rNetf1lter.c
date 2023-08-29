#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <linux/types.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <curl/curl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <resolv.h>
#include <ctype.h>

#define RED     "\x1B[31m"
#define ORANGE  "\x1B[38;5;208m"
#define YELLOW  "\x1B[38;5;226m"
#define GREEN   "\x1B[32m"
#define BLUE    "\x1B[34m"
#define RESET   "\x1B[0m"

// $ INFO green
// $ ERROR red
// $ IN PROGRESS yellow
// $ STATUS BLUE
// $ WARNING orange
// $ DECISION BLUE

#define UDP_HEADER_LEN 8
#define TCP_HEADER_LEN 20
#define MAX_LINE_LENGTH 256 // Максимальная длина строки в блэклистах (только for IP-&MAC-address)

#define BLOCK_PROTOCOL_ICMP 1
#define BLOCK_PROTOCOL_FTP  6
#define BLOCK_PROTOCOL_TELNET 23

// TCP syn-flood
#define MAX_SYN_PACKETS 10 // Максимальное количество SYN-пакетов в 1 секунду
static int syn_packet_count = 0;
static time_t last_syn_time = 0;

// TCP Dos/DDos-attack
#define RATE_LIMIT_THRESHOLD 100 // Максимальное количество пакетов за одну секунду
#define RATE_LIMIT_INTERVAL 1 // 1 second
struct RateLimitData {
    time_t last_reset;
    int packet_count;
};

// Добавьте глобальную переменную для хранения последнего известного номера последовательности
uint32_t last_seq_num = 0;

#define MAX_ALLOWED_WINDOW_SIZE 65535

// TCP port-scanning
#define MAX_IP_ADDRESSES 1000
#define SCAN_THRESHOLD 5 // Пороговое значение для определения сканирования (количество запросов)
#define MAX_PORTS 10
struct PortScanInfo {
    char ipAddressStr[INET_ADDRSTRLEN];
    time_t lastScanTime;
    int scannedPorts[MAX_PORTS]; // Массив для хранения номеров портов
    int numScannedPorts; // Количество отсканированных портов
};

struct PortScanInfo portScanInfoList[MAX_IP_ADDRESSES];
int numScanInfoEntries = 0;

// UDP flood
static time_t last_udp_packet_time = 0;
static int udp_packet_count = 0;

// UDP DNS Amplification Attack
#define DNS_PORT 53

// UDP NTP attack
#define NTP_PORT 123
#define MAX_NTP_PACKETS_PER_MINUTE 100  // Примерное количество NTP-пакетов, прежде чем начнется блокировка
#define TIME_WINDOW 60  // Временное окно в секундах (1 минута)
// Глобальные переменные
unsigned int ntp_packet_count = 0;
time_t time_window_start = 0;

// UDP PORT SCANNING
#define UDP_SCAN_THRESHOLD 10 // Количество пакетов для считывания сканирования
#define UDP_SCAN_TIMEFRAME 1  // Временное окно в секундах для сканирования
static struct {
    time_t timestamp; // Время начала окна для сканирования
    int packet_count; // Количество пакетов в окне
} udp_scan_state = {0, 0};

// UDP Fragmentation Attack
#define MAX_UDP_PACKET_SIZE 1500 // Пример максимального размера UDP-пакета (в байтах)

// Функция отслеживания и ограничения скорости
void reset_rate_limit_data(struct RateLimitData *rate_limit_data) {
    rate_limit_data->last_reset = time(NULL);
    rate_limit_data->packet_count = 0;
}
int is_rate_limited(struct RateLimitData *rate_limit_data) {
    time_t current_time = time(NULL);
    
    if (current_time - rate_limit_data->last_reset >= RATE_LIMIT_INTERVAL) {
        reset_rate_limit_data(rate_limit_data);
    }
    
    return rate_limit_data->packet_count >= RATE_LIMIT_THRESHOLD;
}


static int IPChecker(char *IP4Check)
{
	FILE *file = fopen("blacklist.txt", "r");
	if (file == NULL) 
	{
        	perror("Error open file");
        	return -1;
	}
	int sub_len = strlen(IP4Check);
	char buffer[sub_len+1];  // Буфер для чтения данных из файла

	while (fgets(buffer, sub_len+1, file) != NULL) 
	{
		if (strncmp(buffer, IP4Check, sub_len) == 0) 
		{
			fclose(file);
			return 1;  // Совпадение найдено
		}
	}
	fclose(file);
	return 0;  // Совпадение не найдено
}

// Dest ports
static int DestPortBlock(int *Port4Check)
{
	int port = *Port4Check;
	switch (port)
	{
		case 22:
			return 1;
			break;
			/*
		case 80:
			return 0;
			break;
		case 53:
			return 0;
			break;   
		case 443:
			return 0;
			break;
			*/
		default:
			break;
			
	}
	return 0; // Вернуть значение по вашему усмотрению
}

// Sources ports
static int PortBlock(int *Port4Check)
{
	int port = *Port4Check;
	switch (port)
	{
		case 22:
			return 1;
			break;
		case 80:
			return 0;
			break;
		case 53:
			return 0;
			break;   
		case 443:
			return 0;
			break;
		default:
			break;
	}
	return 0; // Вернуть значение по вашему усмотрению
}

// Callback функция для обработки пакетов
static int packet_callback(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data)
{
	FILE *fp;
	char fname[]="logs.txt";
	fp=fopen(fname,"a+");	
	fprintf(fp,"\n--------------------------------------------------------\n");
       
	struct nfqnl_msg_packet_hdr *ph;
	unsigned char *packet;
	int id,payload_len;
	int* SourcePort; // Указатель на int
	int* DestPort; 
	char* ipAddressStr;
	unsigned char *pkt_data;
	//struct nfqnl_msg_packet_timestamp *tstamp;
	
	// Получаем заголовок пакета
	ph = nfq_get_msg_packet_hdr(nfa);
	if (ph)
	{
		id=ntohl(ph->packet_id);
		// Получаем ID пакета
		fprintf(stdout,"\nPacket ID: %ld\n", id );
		fprintf(fp,"\nPacket ID: %ld\n", id );
	}
	else
	{
    		fprintf(stderr, "$ %sINFO%s: can't get packet ID\n",GREEN,RESET);
    		fprintf(fp, "$ INFO: can't get packet ID\n");
	}

	/*
	tstamp = nfq_get_packet_timestamp(nfa);
	if (tstamp)
	{
		printf("Timestamp: %u.%u\n", tstamp->sec, tstamp->usec);
	}
	*/

	// Получаем данные пакета
	payload_len = nfq_get_payload(nfa, &packet);
	if (payload_len >= 0)
	{
		fprintf(stdout,"Packet Data Length: %d\n", payload_len);
		fprintf(fp,"Packet Data Length: %d\n", payload_len);
		
		// Получаем mac адрес отправителя
		struct nfqnl_msg_packet_hw *hwph = nfq_get_packet_hw(nfa);
		if (hwph)
		{
			fprintf(stdout,"Source MAC-address: %02x:%02x:%02x:%02x:%02x:%02x\n",
			hwph->hw_addr[0], hwph->hw_addr[1], hwph->hw_addr[2],
			hwph->hw_addr[3], hwph->hw_addr[4], hwph->hw_addr[5]);
			fprintf(fp,"Source MAC-address: %02x:%02x:%02x:%02x:%02x:%02x\n",
			hwph->hw_addr[0], hwph->hw_addr[1], hwph->hw_addr[2],
			hwph->hw_addr[3], hwph->hw_addr[4], hwph->hw_addr[5]);
		}
		else
		{
			fprintf(stderr, "$ %sWARNING%s: can't get source MAC-address\n",ORANGE,RESET);
			fprintf(fp,"$ WARNING: can't get source MAC-address\n");
		}
		
		struct iphdr *ip_header = (struct iphdr *)packet;
		if (ip_header->protocol == IPPROTO_TCP)
		{
			fprintf(stdout,"Protocol: TCP\n");
			fprintf(fp,"Protocol: TCP\n");
		
			struct tcphdr *tcp_header = (struct tcphdr *)(packet + (ip_header->ihl << 2));
			int tcp_header_len = tcp_header->doff * 4; // Размер заголовка TCP в байтах
				
			fprintf(stdout,"tcp_heder_len = %d\n",tcp_header_len);
			fprintf(fp,"tcp_heder_len = %d\n",tcp_header_len);
			
			fprintf(stdout,"Source IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));
			fprintf(fp,"Source IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));
			
			fprintf(stdout,"Source Port: %u\n", ntohs(tcp_header->source));
			fprintf(fp,"Source Port: %u\n", ntohs(tcp_header->source));
		
			fprintf(stdout,"Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->daddr));
			fprintf(fp,"Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->daddr));
		     
			fprintf(stdout,"Destination Port: %u\n", ntohs(tcp_header->dest));
			fprintf(fp,"Destination Port: %u\n", ntohs(tcp_header->dest));
		    
			fprintf(stdout,"Sequence Number: %u\n", ntohl(tcp_header->seq));
			fprintf(fp,"Sequence Number: %u\n", ntohl(tcp_header->seq));
		    
			fprintf(stdout,"Acknowledgment Number: %u\n", ntohl(tcp_header->ack_seq));
			fprintf(fp,"Acknowledgment Number: %u\n", ntohl(tcp_header->ack_seq));
		    
			printf("Data Offset: %u\n", tcp_header->doff);
			fprintf(fp,"Data Offset: %u\n", tcp_header->doff);
		    
			fprintf(stdout,"Reserved: %u\n", tcp_header->res1);
			fprintf(fp,"Reserved: %u\n", tcp_header->res1);
		    
			fprintf(stdout,"Window: %u\n", ntohs(tcp_header->window));
			fprintf(fp,"Window: %u\n", ntohs(tcp_header->window));
		    
			fprintf(stdout,"Checksum: 0x%x\n", ntohs(tcp_header->check));
			fprintf(fp,"Checksum: 0x%x\n", ntohs(tcp_header->check));
		    
			fprintf(stdout,"Urgent Pointer: %u\n", ntohs(tcp_header->urg_ptr));
			fprintf(fp,"Urgent Pointer: %u\n", ntohs(tcp_header->urg_ptr));
		     
			// Обработка опций заголовка TCP
			if (tcp_header_len > TCP_HEADER_LEN)
			{
				int option_length = tcp_header_len - TCP_HEADER_LEN;
				unsigned char *options_ptr = (unsigned char *)tcp_header + TCP_HEADER_LEN;

				fprintf(stdout,"TCP Options:\n");
				fprintf(fp,"TCP Options:\n");
				int option_index = 0;
				while (option_index < option_length)
				{
					uint8_t option_kind = options_ptr[option_index];
					uint8_t option_length;

					if (option_kind == TCPOPT_EOL)
					{
						fprintf(stdout,"  End of Options\n");
						fprintf(fp,"  End of Options\n");
						option_length = 1;
			    		}
			    		else if (option_kind == TCPOPT_NOP)
			    		{
						fprintf(stdout,"  No-Operation (NOP) Option\n");
						fprintf(fp,"  No-Operation (NOP) Option\n");
						option_length = 1;
			    		}
			    		else
			    		{
						option_length = options_ptr[option_index + 1];
						fprintf(stdout,"  Option Kind: %u, Option Length: %u\n", option_kind, option_length);
						fprintf(fp,"  Option Kind: %u, Option Length: %u\n", option_kind, option_length);
						// Здесь можно обрабатывать данные в опциях, если необходимо.
			    		}

			    		option_index += option_length;
				}
		    	}
		    	
			fprintf(stdout,"Payload (Data in hex): ");
			int i;
			for (i = 0; i < payload_len; i++)
			{
			    fprintf(stdout,"%02x ", packet[i]);
			    fprintf(fp,"%02x ", packet[i]);
			}
			fprintf(stdout,"\n"); 
			
			fprintf(stdout,"Payload (Data in text): ");
                       
                        for (i = 0; i < payload_len; i++)
                        {
                            fprintf(stdout, "%c", isprint(packet[i]) ? packet[i] : '.');
                            fprintf(fp, "%c", isprint(packet[i]) ? packet[i] : '.');
                        }
                        fprintf(stdout,"\n");
			                        
			// блок проверки пакета по условиям
			fprintf(stdout,"\n$ %sINFO%s: checking packet by rules...",GREEN,RESET);
			fprintf(fp,"\n$ INFO: checking packet by rules...");
					
			// 1) проверка айпи на наличие его в блэклисте
			fprintf(stdout,"\n$ %sIN PROGRESS%s: finding IP in blacklist",YELLOW,RESET);
			fprintf(fp,"\n$ IN PROGRESS: finding IP in blacklist");
			ipAddressStr = inet_ntoa(*(struct in_addr *)&ip_header->saddr);
			if(IPChecker(ipAddressStr)==1)
			{
				fprintf(stdout,"\n$ %sDECISION%s: IP in blacklist !",BLUE,RESET);
				fprintf(fp,"\n$ DECISION: IP in blacklist !");
				goto DROP;
			}
			fprintf(stdout,"\n$ %sSTATUS%s: IP not in blacklist !",BLUE,RESET);
			fprintf(fp,"\n$ STATUS: IP not in blacklist !");
				
		    	// 2) Управление входящими соединениями по определенным портам( ssh-входящие подключения - блок, 80 и 53 - пропуск)
		    	fprintf(stdout,"\n$ %sIN PROGRESS%s: check source port",YELLOW,RESET);
		    	fprintf(fp,"\n$ IN PROGRESS: check source port");
		    	int port_value; // Переменная для хранения значения 
			port_value = ntohs(tcp_header->source);
			SourcePort = &port_value;
			if(PortBlock(SourcePort)==1)
			{
				fprintf(stdout,"\n$ %sDECISION%s: source with %d port has blocked !",BLUE,RESET,*SourcePort);
				fprintf(fp,"\n$ DECISION: source with %d port has blocked !",*SourcePort);
				goto DROP;
			}
			fprintf(stdout,"\n$ %sSTATUS%s: correctly source port %d !",BLUE,RESET,*SourcePort);
			fprintf(fp,"\n$ STATUS: correctly source port %d !",*SourcePort);
			
			// 3) SYN-flood
			fprintf(stdout,"\n$ %sIN PROGRESS%s: check for SYN-flood",YELLOW,RESET);
			fprintf(fp,"\n$ IN PROGRESS: check for SYN-flood");
			// Проверяем, является ли пакет SYN-пакетом
			if (tcp_header->syn && !tcp_header->ack) 
			{
				fprintf(stdout,"\n$ %sWARNING%s: detected syn-packate",ORANGE,RESET);
			    	fprintf(fp,"\n$ INFO: detected syn-packate");
				// Получаем текущее время
				time_t current_time = time(NULL);

				// Если прошло менее 1 секунды с момента последнего SYN-пакета, увеличиваем счетчик
				if (current_time - last_syn_time < 1) 
				{
					syn_packet_count++;
				} 
				else 
				{
					// Если прошло более 1 секунды, сбрасываем счетчик и обновляем время
					syn_packet_count = 1;
					last_syn_time = current_time;
				}

				// Если получены слишком много SYN-пакетов за короткий промежуток времени, блокируем пакет
				if (syn_packet_count > MAX_SYN_PACKETS) 
				{
					fprintf(stdout,"\n$ %sINFO%s: Detected SYN-flood attack, dropping packet\n",GREEN,RESET);
					fprintf(fp, "\n$ INFO: Detected SYN-flood attack, dropping packet\n");
					fprintf(fp, "\n$ INFO: Too many SYN packets received in a short time\n");
					fprintf(stdout,"\n$ %sINFO%s: Too many SYN packets received in a short time\n",GREEN,RESET);
					//goto ADD_TO_BLACKLIST;
					goto DROP;
				}
			}

			// 4) Защита от Dos/DDos-атак
			fprintf(stdout,"\n$ %sIN PROGRESS%s: check for Dos/DDos-attack",YELLOW,RESET);
			fprintf(fp,"\n$ IN PROGRESS: check for Dos/DDos-attack");
			struct RateLimitData *rate_limit_data = (struct RateLimitData *)data;
    			if (is_rate_limited(rate_limit_data)) 
    			{
    			        fprintf(stdout,"\n$ %sWARNING%s: rate limited !",ORANGE,RESET);
				fprintf(stdout,"\n$ %sINFO%s: rate limited, packet dropped !",GREEN,RESET);
				fprintf(fp,"\n$ %sWARNING%s: rate limited !");
				fprintf(fp,"\n$ INFO: rate limited, packet dropped !");
				//goto ADD_TO_BLACKLIST;
				//goto DROP;
			}
			rate_limit_data->packet_count++;
			
			// 5) защита от атаки tcp (reset)
			// Для защиты от атак TCP RST (сброса соединения) вы можете реализовать фильтрацию входящих RST пакетов в вашем коде обработки пакетов. В данной ситуации вы хотите игнорировать входящие RST пакеты, чтобы они не могли привести к разрыву существующих соединений.
			// Прежде всего, вам следует добавить проверку на TCP флаг RST в заголовке TCP пакета. Если флаг RST установлен, это означает, что пакет является RST пакетом и может быть обработан соответствующим образом.
			if (tcp_header->rst)
			{
				// Это пакет с флагом RST, возможно, атака TCP Reset (RST)
				fprintf(stdout,"\n$ %sWARNING%s: Detected TCP Reset (RST) packet, dropping...\n",ORANGE,RESET);
				fprintf(fp, "\n$ WARNING: Detected TCP Reset (RST) packet, dropping...\n");
				//goto DROP; // или выполните другие действия, в зависимости от вашей политики
			}
			
			// 6) TCP Hijacking (перехвата TCP соединения) можно использовать технику, называемую "TCP Sequence Number Check". В данной технике вы проверяете последовательные номера пакетов в заголовках TCP, чтобы обнаружить возможные изменения или перехваты.
			// Проверьте последовательный номер TCP пакета
			uint32_t current_seq_num = ntohl(tcp_header->seq);
			if (last_seq_num != 0 && current_seq_num < last_seq_num) 
			{
				// Пакеты были перехвачены или порядок нарушен
				fprintf(stdout,"\n$ %sWARNING%s: TCP Sequence Number Out of Order - Possible Hijacking Attempt!\n",ORANGE,RESET);
				fprintf(fp, "\n$ WARNING: TCP Sequence Number Out of Order - Possible Hijacking Attempt!\n");
				// Дополнительные действия по обработке этой ситуации
				// Например, отклонить пакет или выполнять другие проверки безопасности
			}
			last_seq_num = current_seq_num;
			
			// 7) TCP Window Scaling атака: Злоумышленник может эксплуатировать механизм масштабирования окна TCP для усиления атаки на сервер, увеличивая объем данных, отправляемых в одной "пачке", чтобы перегрузить сеть.
			// Для защиты от атак на механизм масштабирования окна TCP (TCP Window Scaling) можно внести соответствующие изменения в код вашего фильтра-пакетов. Этот вид атаки пытается искусственно увеличить размер TCP-пакетов для перегрузки сети и сервера.
			// Для защиты от такой атаки, вы можете включить проверку размера окна TCP в заголовке TCP и отбрасывать пакеты, в которых размер окна слишком велик.
			// Проверка размера окна TCP
			uint16_t window_size = ntohs(tcp_header->window);
			if (window_size > MAX_ALLOWED_WINDOW_SIZE) 
			{
				fprintf(stdout,"$ %sWARNING%s: TCP window size exceeds maximum allowed size\n",ORANGE,RESET);
				fprintf(fp, "$ WARNING: TCP window size exceeds maximum allowed size\n");
				goto DROP; // Отбрасываем пакет
			}
			
			// 8) TCP Port Scanning (сканирование портов): Это несколько иное, чем предыдущие виды атак. Злоумышленник сканирует открытые порты на хосте с целью определить, какие сервисы доступны и могут быть подвержены атакам.
			// 
			// Получаем IP-адрес отправителя
			ipAddressStr = inet_ntoa(*(struct in_addr *)&ip_header->saddr);

			// Проверяем, есть ли уже запись для данного IP-адреса в списке
			int existingIndex = -1;
			for (int i = 0; i < numScanInfoEntries; ++i) 
			{
				if (strcmp(portScanInfoList[i].ipAddressStr, ipAddressStr) == 0) 
				{
					existingIndex = i;
					break;
				}
			}

			if (existingIndex == -1) 
			{
				// Новый IP-адрес, добавляем запись в список
				if (numScanInfoEntries < MAX_IP_ADDRESSES) 
				{
					strncpy(portScanInfoList[numScanInfoEntries].ipAddressStr, ipAddressStr, INET_ADDRSTRLEN);
					portScanInfoList[numScanInfoEntries].lastScanTime = time(NULL);
					portScanInfoList[numScanInfoEntries].numScannedPorts = 0; // Изначально нет отсканированных портов
					numScanInfoEntries++;
				}
			} 
			else 
			{
				// Существующий IP-адрес, проверяем количество сканированных портов
				struct PortScanInfo *scanInfo = &portScanInfoList[existingIndex];
				if (scanInfo->numScannedPorts < SCAN_THRESHOLD) 
				{
				    // Добавляем номер порта в список
					int port = ntohs(tcp_header->dest);
					scanInfo->scannedPorts[scanInfo->numScannedPorts] = port;
					scanInfo->numScannedPorts++;
					if (scanInfo->numScannedPorts >= SCAN_THRESHOLD) 
					{
						// Порог сканирования достигнут, выполняем действия
						fprintf(stdout,"$ %sWARNING%s: Potential port scanning detected from %s\n",ORANGE,RESET,ipAddressStr);
						// Дополнительные действия по блокировке или логированию
					}
				}
				// Обновляем время последней попытки сканирования
				scanInfo->lastScanTime = time(NULL);
			}
			
			
			// 9) блокирование некоторого исходящего соединения + возможность Разблокировки исходящего ssh-подключения (может и другие тоже)
			// SOON
			
			// Если пакет успешно прошел все правила то принимаем его
			// здесь можно  подсчитать итоговые баллы и вынести вердикт
			
			
			// 10) Block incoming ssh-connection by tcp
			fprintf(stdout,"\n$ %sIN PROGRESS%s: check source port",YELLOW,RESET);
		    	fprintf(fp,"\n$ IN PROGRESS: check source port");
			port_value = ntohs(tcp_header->dest);
			DestPort = &port_value;
			if(DestPortBlock(DestPort)==1)
			{
				fprintf(stdout,"\n$ %sDECISION%s: destination with %d port has blocked !",BLUE,RESET,*DestPort);
				fprintf(fp,"\n$ DECISION: destination with %d port has blocked !",*SourcePort);
				goto DROP;
			}
			fprintf(stdout,"\n$ %sSTATUS%s: correctly destination port %d !",BLUE,RESET,*SourcePort);
			fprintf(fp,"\n$ STATUS: correctly destination port %d !",*SourcePort);
			
			// Accept packate by tcp protocol
			//goto ACCEPT;
		}
		else if (ip_header->protocol == IPPROTO_UDP)
		{
			fprintf(stdout,"Protocol: UDP\n");
			fprintf(fp,"Protocol: UDP\n");
			struct udphdr *udp_header = (struct udphdr *)(packet + (ip_header->ihl << 2));
		    
			fprintf(stdout,"Source IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));
			fprintf(fp,"Source IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));
			
			fprintf(stdout,"Source Port: %u\n", ntohs(udp_header->source));
			fprintf(fp,"Source Port: %u\n", ntohs(udp_header->source));
		    
			fprintf(stdout,"Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->daddr));
			fprintf(fp,"Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&ip_header->daddr));
		    
			fprintf(stdout,"Destination Port: %u\n", ntohs(udp_header->dest));
			fprintf(fp,"Destination Port: %u\n", ntohs(udp_header->dest));
		    
			fprintf(stdout,"Length: %u\n", ntohs(udp_header->len));
			fprintf(fp,"Length: %u\n", ntohs(udp_header->len));
		    
			fprintf(stdout,"Checksum: 0x%x\n", ntohs(udp_header->check));
			fprintf(fp,"Checksum: 0x%x\n", ntohs(udp_header->check));
		
			fprintf(stdout,"Payload (Data): ");
			int i;
			for (i = 0; i < payload_len; i++)
			{
				fprintf(stdout,"%02x ", packet[i]);
				fprintf(fp,"%02x ", packet[i]);
			}
			fprintf(stdout,"\n");
			
			// блок проверки пакета по условиям
			fprintf(stdout,"\n$ %sINFO%s: checking packet by rules...",GREEN,RESET);
			fprintf(fp,"\n$ INFO: checking packet by rules...");		
			
			// 1) проверка айпи на наличие его в блэклисте
			fprintf(stdout,"\n$ %sIN PROGRESS%s: finding IP in blacklist",YELLOW,RESET);
			fprintf(fp,"\n$ IN PROGRESS: finding IP in blacklist");
			ipAddressStr = inet_ntoa(*(struct in_addr *)&ip_header->saddr);
			if(IPChecker(ipAddressStr)==1)
			{
				fprintf(stdout,"\n$ %sDECISION%s: IP in blacklist !",BLUE,RESET);
				fprintf(fp,"\n$ DECISION: IP in blacklist !");
				goto DROP;
			}
			fprintf(stdout,"\n$ %sSTATUS%s: IP not in blacklist !",BLUE,RESET);
			fprintf(fp,"\n$ STATUS: IP not in blacklist !");	
			
		    	// 2) Управление входящими соединениями по определенным портам( ssh-входящие подключения - блок, 80 и 53 - пропуск)
		    	fprintf(stdout,"\n$ %sIN PROGRESS%s: check source port",YELLOW,RESET);
		    	fprintf(fp,"\n$ IN PROGRESS: check source port");
		    	int port_value; // Переменная для хранения значения 
			
			port_value = ntohs(udp_header->source);
			SourcePort = &port_value;
			if(PortBlock(SourcePort)==1)
			{
				fprintf(stdout,"\n$ %sDECISION%s: source with %d port has blocked !",BLUE,RESET,*SourcePort);
				fprintf(fp,"\n$ DECISION: source with %d port has blocked !",*SourcePort);
				goto DROP;
			}
			fprintf(stdout,"\n$ %sSTATUS%s: correctly source port %d !",BLUE,RESET,*SourcePort);
			fprintf(fp,"\n$ STATUS: correctly source port %d !",*SourcePort);
			
			
			// 3) Защита от Dos/DDos-атак
			fprintf(stdout,"\n$ %sIN PROGRESS%s: check for Dos/DDos-attack",YELLOW,RESET);
			fprintf(fp,"\n$ IN PROGRESS: check for Dos/DDos-attack");
			struct RateLimitData *rate_limit_data = (struct RateLimitData *)data;
    			if (is_rate_limited(rate_limit_data)) 
    			{
    			        fprintf(stdout,"\n$ %sWARNING%s: rate limited !",ORANGE,RESET);
				fprintf(stdout,"\n$ %sINFO%s: rate limited, packet dropped !",GREEN,RESET);
				fprintf(fp,"\n$ %sWARNING%s: rate limited !");
				fprintf(fp,"\n$ INFO: rate slimited, packet dropped !");
				//goto ADD_TO_BLACKLIST;
				//goto DROP;
			}
			rate_limit_data->packet_count++;
			
			// 3) uDP флуд (UDP Flood): Это тип атаки DDoS, при котором злоумышленники отправляют огромное количество UDP пакетов на целевой сервер или устройство, перегружая его ресурсы и вызывая отказ в обслуживании.
			//
			// Проверка на UDP флуд
			time_t current_time = time(NULL);
			if (current_time - last_udp_packet_time < 1) // Предполагаемый интервал в секундах (например, 1 секунда)
			{
				udp_packet_count++;
				if (udp_packet_count > 100) // Предполагаемый порог для флуда (например, 100 пакетов)
				{
					fprintf(stdout,"$ %sINFO%s: UDP flood detected, dropping packets\n",GREEN,RESET);
					fprintf(fp, "$ INFO: UDP flood detected, dropping packets\n");
				
					udp_packet_count = 0;
					goto DROP;
				}
			}
			else
			{
				udp_packet_count = 1;
			}

			last_udp_packet_time = current_time;
			
			// 4) DNS Amplification Attack (Атака с усилением через DNS): В этой атаке злоумышленники отправляют запросы к открытым DNS серверам, используя поддельный адрес источника. DNS серверы отвечают на запросы с большими объемами данных, которые направляются к жертве атаки, вызывая перегрузку её сетевых ресурсов.
			
			int udp_source_port = ntohs(udp_header->source);
			int udp_dest_port = ntohs(udp_header->dest);
			if (udp_source_port == DNS_PORT || udp_dest_port == DNS_PORT) 
			{
				fprintf(stdout,"DNS request detected on UDP port %d\n", udp_source_port);
				fprintf(fp, "DNS request detected on UDP port %d\n", udp_source_port);
			    
				// Дополнительные проверки на подделанные запросы и другие условия могут быть добавлены здесь
			    
				// Блокировать пакет, если он соответствует условиям атаки
				/*
				fprintf(stdout,"$ %sDECISION%s: DNS Amplification Attack detected and packet dropped!\n",BLUE,RESET);
				fprintf(fp, "$ %sDECISION%s: DNS Amplification Attack detected and packet dropped!\n",BLUE,RESET);
				goto DROP;
				*/
			}
			
			// 5)NTP Amplification Attack (Атака с усилением через NTP): Эта атака схожа с атакой через DNS, но использует NTP (Network Time Protocol) серверы для генерации больших объемов трафика на адрес жертвы.
			//time_t current_time = time(NULL);
			// Проверяем, находится ли текущий пакет в новом временном окне
			if (current_time - time_window_start >= TIME_WINDOW)
			{
				// Временное окно изменилось, сбрасываем счетчик и обновляем начальное время
				ntp_packet_count = 0;
				time_window_start = current_time;
			}
			if (ntohs(udp_header->source) == NTP_PORT || ntohs(udp_header->dest) == NTP_PORT)
			{
				fprintf(stdout,"$ %sWARNING%s: detected NTP traffic!\n",ORANGE,RESET);
				fprintf(fp,"$ WARNING: detected NTP traffic!\n");
				ntp_packet_count++;

				if (ntp_packet_count > MAX_NTP_PACKETS_PER_MINUTE)
				{
					// Отклоняем пакет
					fprintf(stdout,"$ %sDECISION%s: the package has been dropped due to excessive NTP traffic!\n",BLUE,RESET);
					fprintf(fp,"$ DECISION: the package has been dropped due to excessive NTP traffic!\n");
					goto DROP;
				}
			}
			
			// 6)UDP Port Scanning (Сканирование портов): Злоумышленники могут отправлять UDP пакеты на различные порты устройства для выявления открытых служб и потенциальных уязвимостей.
			// Проверка на сканирование портов
			//time_t current_time = time(NULL);
			if (udp_scan_state.timestamp == 0) 
			{
				// Начало нового временного окна
				udp_scan_state.timestamp = current_time;
				udp_scan_state.packet_count = 1;
			} 
			else if (current_time - udp_scan_state.timestamp < UDP_SCAN_TIMEFRAME) 
			{
				// Внутри того же временного окна
				udp_scan_state.packet_count++;

				if (udp_scan_state.packet_count >= UDP_SCAN_THRESHOLD) 
				{
					// Обнаружено сканирование портов
					fprintf(stdout,"\n$ %sINFO%s: UDP Port Scanning detected!\n",GREEN,RESET);
					fprintf(fp, "\n$ INFO: UDP Port Scanning detected!\n");
					goto DROP;
				}
			} 
			else 
			{
				// Временное окно завершилось, сброс состояния
				udp_scan_state.timestamp = current_time;
				udp_scan_state.packet_count = 1;
			}
			
			// 7) Fragmentation Attack (Атака с фрагментацией): В этой атаке злоумышленники отправляют большие UDP пакеты, которые разбиваются на фрагменты, чтобы обойти механизмы фильтрации. Это может вызвать перегрузку ресурсов приемника при сборке фрагментов обратно в исходные пакеты.
			// Проверяем размер UDP-пакета
			int udp_packet_size = ntohs(udp_header->len);
			if (udp_packet_size > MAX_UDP_PACKET_SIZE)
			{
				fprintf(stdout,"Large UDP packet detected, size: %d bytes\n", udp_packet_size);
				fprintf(fp,"Large UDP packet detected, size: %d bytes\n", udp_packet_size);
				// Теперь вы можете реализовать логику для обработки этой атаки, например, принимать решение о блокировке пакета.
				// Например:
				// goto DROP;
			}
			    
			// 8) Packet Amplification Attack (Атака с усилением пакетов): В данной атаке злоумышленники могут отправлять запросы на определенные службы, которые затем генерируют гораздо больше данных в ответ, чем было отправлено в запросе. Таким образом, можно усилить атаку и перегрузить жертву.
			    
			    
			    
			// 9) Application Layer Attacks (Атаки на уровне приложения): Некоторые приложения, использующие UDP, могут быть уязвимыми к специфичным атакам, таким как переполнение буфера или манипуляции с приложенными данными.
			
			// 10) Block incoming ssh-connection by udp
			fprintf(stdout,"\n$ %sIN PROGRESS%s: check source port",YELLOW,RESET);
		    	fprintf(fp,"\n$ IN PROGRESS: check source port");
			port_value = ntohs(udp_header->dest);
			DestPort = &port_value;
			if(DestPortBlock(DestPort)==1)
			{
				fprintf(stdout,"\n$ %sDECISION%s: destination with %d port has blocked !",BLUE,RESET,*DestPort);
				fprintf(fp,"\n$ DECISION: destionation with %d port has blocked !",*SourcePort);
				goto DROP;
			}
			fprintf(stdout,"\n$ %sSTATUS%s: correctly destionation port %d !",BLUE,RESET,*SourcePort);
			fprintf(fp,"\n$ STATUS: correctly destination port %d !",*SourcePort);
			
			// Accept packate by udp protocol
			//goto ACCEPT;
		    
		}
		else if (ip_header->protocol == BLOCK_PROTOCOL_ICMP || ip_header->protocol == BLOCK_PROTOCOL_FTP || ip_header->protocol == BLOCK_PROTOCOL_TELNET)
		{
			fprintf(stdout,"$ %sINFO%s: locked packet with protocol: %d\n",GREEN,RESET,ip_header->protocol);
			goto DROP;
		}
		else
		{
			fprintf(stdout,"Protocol: Unknown\n");
			fprintf(fp,"Protocol: Unknown\n");
			goto DROP;
		}
	}
	else 
	{
		fprintf(stdout,"\n$ %sINFO%s: payload <= 0",GREEN,RESET);
		fprintf(fp,"\n$ INFO: payload <= 0");
		goto DROP;
	}

	goto ACCEPT;

	ADD_TO_BLACKLIST:
		fprintf(stdout,"\n$ %sIN PROGRESS%s: adding %s to blacklist",YELLOW,RESET,ipAddressStr);
		FILE *file = fopen("blacklist.txt", "a+"); 
		if (file == NULL) 
		{
			perror("Error open file");
			return 1;
		}	
		if (fputs(ipAddressStr, file) == EOF) 
		{
			perror("Error write to file");
			return 1;
		}
		fprintf(stdout,"\n$ %sSTATUS%s: %s added to blacklist",BLUE,RESET,ipAddressStr);
   		goto DROP;
   		
	DROP:
		// Отклоняем пакет
		fprintf(stdout,"\n$ %sDECISION%s: the package has been dropped!",BLUE,RESET);
		fprintf(fp,"\n$ DECISION: the package has been dropped!");
		fprintf(fp,"\n--------------------------------------------------------\n");
		fclose(fp);
		return nfq_set_verdict(qh, id, NF_DROP, 0, NULL);
	    
	ACCEPT:
		//Принимаем пакет
		fprintf(stdout,"\n$ %sDECISION%s: package accepted!",BLUE,RESET);
		fprintf(fp,"\n$ DECISION: package accepted!");
		fprintf(fp,"\n--------------------------------------------------------\n");
		fclose(fp);
		return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
}



int main()
{
	struct nfq_handle *handle;
	struct nfq_q_handle *queue;
	char buf[4096] __attribute__((aligned));

///
// Open a NFQUEUE handler 
///
	fprintf(stdout,"$ %sIN PROGRESS%s: Open a NFQUEUE handler...\n",BLUE,RESET);
	handle = nfq_open();
	if (!handle)
	{
		fprintf(stderr, "$ %sERROR%s: opening nfq library\n",RED,RESET);
        	exit(1);
	}
	else
	{
	    	fprintf(stdout,"$ %sSUCCEEDED%s: Ubinding existing nf_queue handler for AF_INET succeeded !\n",GREEN,RESET);
	}

///
// Tell the kernel that userspace queuering is handle by NFQUEU for selected protocol.
///    
	fprintf(stdout,"$ %sIN PROGRESS%s: Tell the kernel that userspace queuering is handle by NFQUEU for selected protocol...\n",BLUE,RESET);
	if (nfq_unbind_pf(handle, AF_INET) < 0)
	{
	        fprintf(stderr, "$ %sERROR%s: unbinding existing nf_queue handler\n",RED,RESET);
	        exit(1);
	}
	else
	{
	    	fprintf(stdout,"$ %sSUCCEEDED%s: kernel knows that userspace quering is handle by NFQUEU for selected protocol (AF_INET) !\n",GREEN,RESET);
	}
///
// Bind a nfqueue handler to a given protocol family
///
	fprintf(stdout,"$ %sIN PROGRESS%s: Bind a nfqueue handler to a given protocol family...\n",BLUE,RESET);
	if (nfq_bind_pf(handle, AF_INET) < 0)
	{
	        fprintf(stderr, "$ %sERROR%s: binding nfnetlink_queue handler\n",RED,RESET);
	        exit(1);
	}
	else
	{
	    	fprintf(stdout,"$ %sSUCCEEDED%s: binding nfnetlink_queue as nd_queue handler for AF_INET !\n",GREEN,RESET);
	}
///	
// To bind the program to a specific queue
// The queue can then be tuned via nfq_set_mode() or nfq_set_queue_maxlen()
///
	fprintf(stdout,"$ %sIN PROGRESS%s: binding this socket to queue '0' !\n",BLUE,RESET);
	
	struct RateLimitData rate_limit_data;
	reset_rate_limit_data(&rate_limit_data);
	
	queue = nfq_create_queue(handle, 0, packet_callback, &rate_limit_data);
	if (!queue)
	{
	        fprintf(stderr, "$ %sERROR%s: during nfq_create_queue()\n",RED,RESET);
	        exit(1);
	}
	else
	{
	    	fprintf(stdout,"$ %sSUCCEEDED%s: nfq_create_queue() - queue created with no errors !\n",BLUE,RESET);
	}
///
// Setting copy_packet mode
///
	fprintf(stdout,"$ %sIN PROGRESS%s: setting copy_packet mode !\n",BLUE,RESET);
	if (nfq_set_mode(queue, NFQNL_COPY_PACKET, 0xffff) < 0)
	{
        	fprintf(stderr, "$ %sERROR%s: can't set packet_copy mode\n",RED,RESET);
        	exit(1);
	}
	else 
	{
	    	fprintf(stdout,"$ %sSUCCEEDED%s: copy_packet mode!\n",BLUE,RESET);
	}
///
// Handling incoming packets which can be done via a loop
///
	int fd = nfq_fd(handle);
	int rv;
	while ((rv = recv(fd, buf, sizeof(buf), 0)) && rv >= 0)
	{
		fprintf(stdout,"\n--------------------------------------------------------\n");
		// Получаем текущее время
		time_t current_time;
		struct tm *time_info;
		time(&current_time);
		time_info = localtime(&current_time);

		// Получаем текущие часы, минуты и секунды
		int hours = time_info->tm_hour;
		int minutes = time_info->tm_min;
		int seconds = time_info->tm_sec;
		
		FILE *fp;
		char fname[]="logs.txt";
		fp=fopen(fname,"a+");
		
		fprintf(stdout,"\nCurrent system time: %02d:%02d:%02d", hours, minutes, seconds);
		fprintf(fp,"\nCurrent system time: %02d:%02d:%02d", hours, minutes, seconds);		
		
		
		// Проверяем, равно ли время 23:59:59, то производим обновление баз данных (черные листы ip,mac ; вредосоные нагрузки)
		if (hours == 23 && minutes == 59 && seconds == 59) 
		{
				
			
			fprintf(stdout,"\n$ %sWARNING%s: rules back to default for updating !",ORANGE,RESET);
			fprintf(fp,"\n$ WARNING: rules back to default for updating !");
			
			system("sudo iptables -X;sudo iptables -Z; sudo iptables -F;sudo ./IPBlacklistUpgrader;sudo iptables -A INPUT -j NFQUEUE --queue-num 0; sudo iptables -A OUTPUT -j NFQUEUE --queue-num 0"); 
			
			fprintf(stdout,"$ %sINFO%s: updating done !",GREEN,RESET);
			fprintf(fp,"$ INFO: updating done !");
			
			fclose(fp);
			// ...
			// ...
			// ...   
		}
    
		fprintf(stdout,"\n$ %sIN PROGRESS%s: pkt received !",BLUE,RESET);
		fprintf(fp,"\n$ IN PROGRESS: pkt received !");
		nfq_handle_packet(handle, buf, rv);
		//system("python3 python_curl.py");

		fclose(fp);	printf("\n--------------------------------------------------------\n");	
	}
///
// Destrot queue and handle 
///
	fprintf(stdout,"Unbinding from queue 0...\n");
    	nfq_destroy_queue(queue);
    	
#ifdef INSANE
	/* normally, applications SHOULD NOT issue this command, since
	* it detaches other programs/sockets from AF_INET, too ! */
	fprintf(stdout,"Unbinding from AF_INET...\n");
	nfq_unbind_pf(h, AF_INET);
#endif

	fprintf(stdout,"Closing library handle...\n");
	nfq_close(handle);

	return 0;
}

