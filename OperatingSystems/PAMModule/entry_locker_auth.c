#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

// Регулярная инициализация
static struct pam_conv conv = {
    misc_conv,
    NULL
};

void* starter(void* arg) {
    //system("tmux send-keys -t s1:0.0 'sudo <FULL_PATH_TO_PROGRAMM> Enter' Enter");
    return NULL;
}

int threads()
{
    pthread_t Thread1;
    int result = pthread_create(&Thread1, NULL, starter, NULL);
    if (result != 0) {
        printf("Ошибка при создании потока: %d\n", result);
        return 1;
    }

	/* If you want create more threads use this construction --->
    pthread_t Thread2;
    int result1 = pthread_create(&Thread2, NULL, filter, NULL);
    if (result1  != 0) {
        printf("Ошибка при создании потока: %d\n", result1);
        return 1;
    }
    <--- */

    pthread_join(Thread1, NULL); // Ждем завершения потока
    
    //pthread_join(Thread2, NULL); // Ждем завершения потока
    return 0;
}

int main(int argc, char *argv[])
{
    system("clear");
    pam_handle_t *pamh=NULL;
    int retval;
    const char *arg;
    
    // Передача в суждение
    if(argc == 1) {
	arg = argv[1]; // Параметры командной строки для ввода имени пользователя
    } else {
	fprintf(stderr, "Usage: ./entry_locker_auth or ./entry_locker_auth ANY (to get help)\n");
	system("figlet O_o |  boxes -d parchment -p a4");
	exit(1);
    }
    
    // Выполните модуль PAM, файл конфигурации test_pam
    retval = pam_start("test_pam", arg, &conv, &pamh);
    
    system("figlet PAM |  boxes -d tux -p a1 | lolcat");
    fprintf(stderr, "Usage: ./entry or ./entry ANY (to get help)\n");
    

    
    // Запустить модуль PAM успешно запустил модуль Authule, настроенный в файле конфигурации
    if (retval == PAM_SUCCESS)
        retval = pam_authenticate(pamh, 0);   

    if (retval == PAM_SUCCESS) {
	fprintf(stdout, "Authenticated!!!\n");
	//loadingBar()
	sleep(2);
	system("clear");
	//system("unset TMUX");
	threads();
    } else {
	fprintf(stdout, "Not Authenticated!!!\n");
	sleep(2);
	system("clear");
    }
    
    // Завершите выпуск сервиса модуля PAM
    if (pam_end(pamh,retval) != PAM_SUCCESS) {     /* close Linux-PAM */
	pamh = NULL;
	fprintf(stderr, "check_user: failed to release authenticator\n");
	exit(1);
    }
    return ( retval == PAM_SUCCESS ? 0:1 );       /* indicate success */
}

