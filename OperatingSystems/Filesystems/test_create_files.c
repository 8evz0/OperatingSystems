#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

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
	int count = 10000;
	unsigned long long t1 = gt();
	for (int i=0; i<count; i++){
		sprintf(buf,"%s/%08d",argv[1],i);
		FILE * f = fopen(buf,"a");
		fputs("a\n",f);
		fclose(f);
	}
	unsigned long long t2 = gt();
	unsigned long long diff = t2-t1;
	double diff2 = diff / 1000.0;
	double speed = count/diff2;
	printf("File create: %.4f\n",speed);	

	t1 = gt();
	for (int i=0; i<count; i++){
		sprintf(buf,"%s/%08d",argv[1],i);
		unlink(buf);
	}
	t2 = gt();
	diff = t2-t1;
	diff2 = diff / 1000.0;
	speed = count/diff2;
	printf("File remove: %.4f\n",speed);	

	
}
