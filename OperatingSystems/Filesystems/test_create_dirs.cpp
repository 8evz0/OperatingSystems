#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

long long gt() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
int gcounter = 0;
int total_dirs=0;
void CreateDirs (int maxdeep,int curdeep,char * prevname,int subdirs){
	char buf[1024];
	int status;
	if (curdeep == maxdeep){
		for (int i=0; i<subdirs; i++){	
			sprintf(buf,"%s/%08d",prevname,gcounter);
			gcounter+=1;
			FILE * f = fopen(buf,"a");
			fputs("a",f);
			fclose(f);
		}
	}
	else{
		for (int i=0; i<subdirs; i++){
			sprintf(buf,"%s/%08d/",prevname,i);
			status = mkdir(buf, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			total_dirs+=1;
			CreateDirs(maxdeep,curdeep+1,buf,subdirs);
		}
	}
}
int main (int argc, char * argv[]){
	char buf[1024];
	if (argc !=2){
		printf("NO");
		return 1;
	}
	int count = 10000;
	unsigned long long t1 = gt();
	CreateDirs(10,0,argv[1],3);
	unsigned long long t2 = gt();
	unsigned long long diff = t2-t1;
	double diff2 = diff / 1000.0;
	double speed = (total_dirs+gcounter)/diff2;
	printf("File create: %.4f\n",speed);	
}
