#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>

#define loadingBar() \
{\
	int j,i=0;\
	for(;i<=100;i++)\
	{\
		fprintf(stdout,"\r\033[33mLoading \033[32m[");\
		for(j=0;j<i;j+=5)\
		{\
			fprintf(stdout,"#");\
		}\
		for(;j<100;j+=5)\
		{\
			fprintf(stdout," ");\
		}\
		fprintf(stdout,"]\033[33m %3d%% ",i);\
		fflush(stdout);\
		usleep(10000);\
	}\
	fprintf(stdout,"\n");\
}\

void* menu(void* arg) {
    system("tmux send-keys -t s1:0.0 '$(find / -name \"menu_script.sh\") Enter' Enter");
    return NULL;
}

void* filter(void* arg) {
    system("tmux send-keys -t s1:0.2 'sudo $(find / -type f -name \"M0duleF0rNetf1lter\" -executable -print -quit) Enter' Enter");
    //system("tmux send-keys -t s1:0.2 'clear' Enter");
    return NULL;
}

void* MACChanger(void* arg) {
	/*
    system("tmux send-keys -t s1:0.1 'sudo bash $(find / -name MACandIPChanger.sh) Enter' Enter");
    */
    return NULL;
}


int threads()
{
	system("tmux new-session -d -s s1");
	system("tmux split-window -h");
	system("tmux select-pane -t s1:0.0");
	
	pthread_t Thread1;
	int result = pthread_create(&Thread1, NULL, menu, NULL);
	if (result != 0) {
		printf("Ошибка при создании потока: %d\n", result);
		return 1;
	}
	
	system("tmux split-window -v");
	system("tmux select-pane -t s1:0.1");
	
	pthread_t Thread2;
	int result1 = pthread_create(&Thread2, NULL, filter, NULL);
	if (result1  != 0) {
        	printf("Ошибка при создании потока: %d\n", result1);
        	return 1;
    	}

    	system("tmux select-pane -t s1:0.2");
	pthread_t Thread3;
	int result2 = pthread_create(&Thread3, NULL, MACChanger, NULL);
	if (result2  != 0) {
        	printf("Ошибка при создании потока: %d\n", result2);
        	return 1;
    	}
    	
    	system("tmux attach-session -t s1:0.0");

	pthread_join(Thread1, NULL); // Ждем завершения потока
	pthread_join(Thread2, NULL); // Ждем завершения потока
	pthread_join(Thread3, NULL); // Ждем завершения потока
	return 0;
}

int main(int argc, char *argv[])
{
	//sleep(5);
	system("clear & (figlet SHIELD |  boxes -d nuke -p a1 | lolcat)");
    	//system("sudo ./IPBlacklistUpgrader");
	loadingBar()
	system("clear");
	system("sudo iptables -X;sudo iptables -Z; sudo iptables -F; sudo iptables -A INPUT -j NFQUEUE --queue-num 0; sudo iptables -A OUTPUT -j NFQUEUE --queue-num 0");
	//system("unset TMUX");
	if (threads()==0){
		return 0;       /* indicate success */
	}
	else{
		return 1;
	}		
}

