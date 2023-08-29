#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

long long gt() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
int main (int argc, char * argv[]){
	char buf[1024];
	if (argc !=2){
		printf("NO");
		return 1;
	}
	sprintf(buf,"%s/file.bin",argv[1]);
	FILE * f = fopen(buf,"w+");

	char *buf2 = (char*)malloc(1024*1024);
	memset(buf2,1024*1024,'a');
	unsigned int size = 900*1024*1024;
	int count = size/(1024*1024);
	unsigned long long t1 = gt();
	for (int i=0; i<count; i++){
		fwrite(buf2,1024*1024,1,f);
		fflush(f);
	}
	unsigned long long t2 = gt();
	printf("%lld %lld\n",t2,t1);
	double speed = size/(1024.0*1024.0*(t2-t1));
	printf("Write: %.4f\n",speed);	
	
	fclose(f);
}
