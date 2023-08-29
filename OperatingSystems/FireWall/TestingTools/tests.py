import socket
import time
import threading
import struct
import ssl
import binascii
import subprocess

import signal
def test_port(ip4tcp):
    open_ports = []
    
    for port in range(1, 65536):  # Сканирование всех портов от 1 до 65535
        # Создаем сокет
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.settimeout(1)  # Устанавливаем таймаут для соединения
        result = client_socket.connect_ex((ip4tcp, port))
        if result == 0:
            open_ports.append(port)
            print(f"Порт {port} на {ip4tcp} открыт")
        client_socket.close()

    
    return open_ports

def send_packets(ip, port):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        message = b"This is a test message"
        
        while True:
            client_socket.sendto(message, (ip, port))
            time.sleep(0.01)  # Отправляем пакет каждые 0.01 секунды
    
    except Exception as e:
        print(f"Произошла ошибка при отправке пакетов: {e}")
    finally:
        client_socket.close()

def send_rst_packets(ip, port):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_TCP)
        client_socket.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
        
        # Создаем TCP-заголовок для RST-пакета
        source_port = 12345  # Исходный порт (любой)
        seq_number = 0
        ack_number = 0
        data_offset = 5 << 4  # Длина заголовка в 32-битных словах (5)
        flags = 0x04  # RST флаг
        window_size = socket.htons(8192)  # Размер окна
        checksum = 0
        urg_pointer = 0
        tcp_header = struct.pack('!HHLLBBHHH', source_port, port, seq_number, ack_number, data_offset, flags,
                                 window_size, checksum, urg_pointer)
        
        while True:
            client_socket.sendto(tcp_header, (ip, port))
            time.sleep(0.01)  # Отправляем пакет каждые 0.01 секунды
    
    except Exception as e:
        print(f"Произошла ошибка при отправке RST-пакетов: {e}")
    finally:
        client_socket.close()

def send_custom_packets(ip, port, seq_number):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_TCP)
        client_socket.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
        
        # Создаем TCP-заголовок с заданным sequence number
        source_port = 12345  # Исходный порт (любой)
        ack_number = 0
        data_offset = 5 << 4  # Длина заголовка в 32-битных словах (5)
        flags = 0x02  # SYN флаг
        window_size = socket.htons(8192)  # Размер окна
        checksum = 0
        urg_pointer = 0
        tcp_header = struct.pack('!HHLLBBHHH', source_port, port, seq_number, ack_number, data_offset, flags,
                                 window_size, checksum, urg_pointer)
        
        while True:
            client_socket.sendto(tcp_header, (ip, port))
            time.sleep(0.01)  # Отправляем пакет каждые 0.01 секунды
    
    except Exception as e:
        print(f"Произошла ошибка при отправке пользовательских пакетов: {e}")
    finally:
        client_socket.close()

def send_custom_window_packets(ip, port, window_size):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        message = b"This is a test message"
        
        while True:
            client_socket.sendto(message, (ip, port))
            client_socket.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, window_size)
            time.sleep(0.01)  # Отправляем пакет каждые 0.01 секунды
    
    except Exception as e:
        print(f"Произошла ошибка при отправке пакетов с пользовательским размером окна: {e}")
    finally:
        client_socket.close()
        
def send_custom_syn_packets(ip, port):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_TCP)
        client_socket.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
        
        # Создаем TCP-заголовок для SYN-пакета
        source_port = 12345  # Исходный порт (любой)
        seq_number = 123456789  # Произвольное начальное sequence number
        ack_number = 0
        data_offset = 5 << 4  # Длина заголовка в 32-битных словах (5)
        flags = 0x02  # SYN флаг
        window_size = socket.htons(8192)  # Размер окна
        checksum = 0
        urg_pointer = 0
        tcp_header = struct.pack('!HHLLBBHHH', source_port, port, seq_number, ack_number, data_offset, flags,
                                 window_size, checksum, urg_pointer)
        
        while True:
            client_socket.sendto(tcp_header, (ip, port))
            time.sleep(0.01)  # Отправляем пакет каждые 0.01 секунды
    
    except Exception as e:
        print(f"Произошла ошибка при отправке SYN-пакетов: {e}")
    finally:
        client_socket.close()

def scan_all_ports(target_host):
    open_ports = []
    
    for port in range(1, 65536):  # Сканирование всех портов от 1 до 65535
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.settimeout(1)
        result = sock.connect_ex((target_host, port))
        if result == 0:
            open_ports.append(port)
            print(f"Порт {port} на {target_host} открыт")
        sock.close()

    
    return open_ports
    
def send_udp_packets(ip):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        message = b"This is a UDP test message"
        
        for port in range(1, 65536):
            client_socket.sendto(message, (ip, port))
            time.sleep(0.001)  # Отправляем пакет каждые 0.001 секунды
    
    except Exception as e:
        print(f"Произошла ошибка при отправке UDP пакетов: {e}")
    finally:
        client_socket.close()
        
def udp_flood(ip, port, packets_per_second):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        message = b"This is a UDP test message"
        
        sleep_time = 1.0 / packets_per_second
        
        for _ in range(packets_per_second):
            client_socket.sendto(message, (ip, port))
            time.sleep(sleep_time)
    
    except Exception as e:
        print(f"Произошла ошибка при отправке UDP пакетов: {e}")
    finally:
        client_socket.close()

def send_large_udp_packets(ip, port):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        large_message = b"A" * 10000  # Создаем пакет с большим размером
        
        while True:
            client_socket.sendto(large_message, (ip, port))
            time.sleep(0.01)  # Отправляем пакет каждые 0.01 секунды
    
    except Exception as e:
        print(f"Произошла ошибка при отправке больших UDP пакетов: {e}")
    finally:
        client_socket.close()

def send_split_large_udp_packets(ip, port):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        large_message = b"A" * 10000  # Создаем пакет с большим размером
        
        chunk_size = 1000
        num_chunks = len(large_message) // chunk_size
        
        for i in range(num_chunks):
            chunk = large_message[i * chunk_size: (i + 1) * chunk_size]
            client_socket.sendto(chunk, (ip, port))
            time.sleep(0.01)  # Отправляем пакет каждые 0.01 секунды
    
    except Exception as e:
        print(f"Произошла ошибка при отправке разделенных больших UDP пакетов: {e}")
    finally:
        client_socket.close()
        
def send_ping_udp(ip, port, timeout=1):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        client_socket.settimeout(timeout)
        
        message = b"PING"
        
        start_time = time.time()
        client_socket.sendto(message, (ip, port))
        
        data, addr = client_socket.recvfrom(1024)
        end_time = time.time()
        
        print(f"Received ping response from {addr[0]}:{addr[1]} in {end_time - start_time:.6f} seconds")
        
    except Exception as e:
        print(f"Error sending or receiving UDP ping: {e}")
    finally:
        client_socket.close()

def send_ping_tcp(ip, port, timeout=1):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.settimeout(timeout)
        
        start_time = time.time()
        client_socket.connect((ip, port))
        end_time = time.time()
        
        print(f"Connected to {ip}:{port} in {end_time - start_time:.6f} seconds")
        
    except Exception as e:
        print(f"Error connecting via TCP ping: {e}")
    finally:
        client_socket.close()
        
def send_raw_packet(data, interface="eth0"):
    try:
        raw_socket = socket.socket(socket.AF_PACKET, socket.SOCK_RAW)
        raw_socket.bind((interface, 0))
        
        raw_socket.send(data)
        
        print("Raw packet sent successfully")
        
    except Exception as e:
        print(f"Error sending raw packet: {e}")
    finally:
        raw_socket.close()
        
def send_encrypted_data(target_host, port, data):
    try:
        # Создаем сокет
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((target_host, port))
        
        # Оборачиваем сокет в SSL-соединение
        context = ssl.create_default_context()
        secure_socket = context.wrap_socket(client_socket, server_hostname=target_host)
        
        # Отправляем зашифрованные данные
        secure_socket.send(data.encode())
        
        # Чтение ответа сервера (необязательно)
        response = secure_socket.recv(4096)
        print("Received encrypted response:", response.decode())
        
        # Закрываем соединение
        secure_socket.close()
        
    except Exception as e:
        print(f"Error sending encrypted data: {e}")
    finally:
        client_socket.close()

def send_arp_request(ip, interface="eth0"):
    try:
        # Создаем сокет для отправки ARP-запроса
        arp_socket = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(0x0806))
        
        # Получаем MAC-адрес интерфейса
        mac_address = binascii.unhexlify(open(f"/sys/class/net/{interface}/address", "r").read().strip().replace(':', ''))
        
        # Создаем ARP-запрос
        arp_request = struct.pack("!6s6s2s2s1s1s", b'\xff\xff\xff\xff\xff\xff', mac_address, b'\x08\x06', b'\x00\x01', mac_address, socket.inet_aton(ip))
        
        # Отправляем ARP-запрос
        arp_socket.send(arp_request)
        
        print("ARP request sent successfully")
        
    except Exception as e:
        print(f"Error sending ARP request: {e}")
    finally:
        arp_socket.close()

def test_firewall_http(ip, port):
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.settimeout(5)  # Устанавливаем таймаут в 5 секунд
        
        client_socket.connect((ip, port))
        
        # Отправляем HTTP GET-запрос
        http_request = b"GET / HTTP/1.1\r\nHost: " + ip.encode() + b"\r\n\r\n"
        client_socket.send(http_request)
        
        # Получаем ответ
        response = client_socket.recv(4096)
        
        print("HTTP response:", response.decode())
        
    except Exception as e:
        print(f"Error testing firewall with HTTP: {e}")
    finally:
        client_socket.close()

def test_firewall_all_ports(ip, start_port, end_port):
    try:
        for port in range(start_port, end_port + 1):
            # HTTP запрос
            print("HTTP request")
            test_firewall_http(ip, port)
            
            # ARP
            print("ARP request")
            send_arp_request(ip, interface)
            
            # Тестирование ping запросами
            print("Sending UDP ping...")
            send_ping_udp(ip, port)
            
            print("Sending TCP ping...")
            send_ping_tcp(ip, port)
            
            # Отправка сырых Ethernet-фреймов
            print("Sending payload...")
            ethernet_header = b'\x00\x22\x22\x33\x44\x22\x33\x44\x55\x55\x33\x44\x55\x11\x22\x33\x44\x22\x33\x44\x55\x55\x22\x22\x33\x44\x22\x33\x44\x55\x55\x22\x33\x44\x55\x33\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x22\x33\x44\x55\x33\x44\x55\x44\x22\x22\x33\x44\x55\x33\x44\x22\x33\x44\x55\x22\x33\x44\x22\x33\x44\x55\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x55\x22\x22\x33\x44\x55\x33\x44\x55\x55\x22\x33\x44\x55\x22\x33\x44\x55\x00\x11\x22\x33\x22\x33\x44\x55\x44\x66\x08\x22\x33\x44\x22\x33\x44\x55\x22\x33\x44\x55\x55\x22\x33\x44\x55\x22\x33\x44\x22\x33\x44\x22\x33\x44\x55\x55\x55\x00'
            payload = b'This is a test payload'
            raw_data = ethernet_header + payload
            
            send_raw_packet(raw_data, "eth0")
            
            # Отправка зашифрованного трафика
            print("Encrypted data SSL")
            encrypted_data = b'\x00\x22\x22\x33\x44\x22\x33\x44\x55\x55\x33\x44\x55\x11\x22\x33\x44\x22\x33\x44\x55\x55\x22\x22\x33\x44\x22\x33\x44\x55\x55\x22\x33\x44\x55\x33\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x22\x33\x44\x55\x33\x44\x55\x44\x22\x22\x33\x44\x55\x33\x44\x22\x33\x44\x55\x22\x33\x44\x22\x33\x44\x55\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x22\x33\x44\x55\x55\x22\x22\x33\x44\x55\x33\x44\x55\x55\x22\x33\x44\x55\x22\x33\x44\x55\x00\x11\x22\x33\x22\x33\x44\x55\x44\x66\x08\x22\x33\x44\x22\x33\x44\x55\x22\x33\x44\x55\x55\x22\x33\x44\x55\x22\x33\x44\x22\x33\x44\x22\x33\x44\x55\x55\x55\x00'
            send_encrypted_data(ip, port, encrypted_data)
            
            
            
            print("Testing by TCP")
            
            # Запуск функции отправки пакетов в отдельном потоке
            packet_thread = threading.Thread(target=send_packets, args=(ip, port))
            packet_thread.start()
            
            # Запуск функции отправки RST-пакетов в отдельном потоке
            rst_thread = threading.Thread(target=send_rst_packets, args=(ip, port))
            rst_thread.start()
            
            # Запуск функции отправки пользовательских пакетов с заданным sequence number
            custom_seq_number = 123456
            custom_thread = threading.Thread(target=send_custom_packets, args=(ip, port, custom_seq_number))
            custom_thread.start()
            
            # Тестирование открытости порта
            test_port(ip)
            
            # Запуск функции отправки пользовательских пакетов с заданным размером окна
            custom_window_size = 100000 # Размер окна (в байтах)
            custom_window_thread = threading.Thread(target=send_custom_window_packets,
                                                    args=(ip, port, custom_window_size))
            custom_window_thread.start()
            
            # Запуск функции отправки SYN-пакетов
            syn_thread = threading.Thread(target=send_custom_syn_packets, args=(ip, port))
            syn_thread.start()
            
           
            
            print("Testing by TCP finished")
            
            print("Testing by UDP")
            
            # Сканирование всех портов на устройстве
            open_ports = scan_all_ports(ip)
            
                

            # Запуск функции отправки UDP пакетов на весь диапазон портов
            udp_flood = threading.Thread(target=send_udp_packets, args=(ip,))
            udp_flood.start()
            
            # Запуск функции отправки больших UDP пакетов
            large_udp_thread = threading.Thread(target=send_large_udp_packets, args=(ip, port))
            large_udp_thread.start()
            
            # Запуск функции отправки разделенных больших UDP пакетов
            split_large_udp_thread = threading.Thread(target=send_split_large_udp_packets, args=(ip, port))
            split_large_udp_thread.start()
            
            # Ожидание завершения потоков 
            packet_thread.join()
            rst_thread.join()
            custom_thread.join()
            custom_window_thread.join()
            syn_thread.join()
            udp_thread.join()
            udp_flood.join()
            large_udp_thread.join()
            split_large_udp_thread.join()
            
            
            print("Testing by UDP finished")
                
    except Exception as e:
        print(f"Error testing firewall: {e}")
        
# Define a custom timeout handler to ignore timeouts
def timeout_handler(signum, frame):
    pass

def discover_devices(subnet):
    active_ips = []

    # Set the custom timeout handler for the ping subprocess
    signal.signal(signal.SIGALRM, timeout_handler)

    for i in range(1, 256):
        for j in range(1, 256):
            for k in range(1, 256):
                ip = f"{subnet}.{i}.{j}.{k}"

                # Set a timeout of 2 seconds for each ping
                signal.alarm(2)
                try:
                    response = subprocess.run(["ping", "-c", "1", "-W", "1", ip], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    if response.returncode == 0:
                        active_ips.append(ip)
                        print(f"Device found at IP: {ip}")
                    #else:
                        #print(f"Device hasnt found at IP: {ip}")
                except subprocess.TimeoutExpired:
                    pass
                finally:
                    signal.alarm(0)  # Disable the alarm

    return active_ips

def main():
    subnet = "192"  # Specify the subnet to scan
    active_ips = discover_devices(subnet)
   
if __name__ == "__main__":
    target_host = "192.169.3.23"  # Замените на целевой IP-адрес
    start_port = 1  # Начальный порт для сканирования
    end_port = 65535  # Конечный порт для сканирования
    interface = "enp3s0"  # Замените на ваш интерфейс
    
    # Сканирование сети
    main()
    
    test_firewall_all_ports(target_host, start_port, end_port)
    
    

