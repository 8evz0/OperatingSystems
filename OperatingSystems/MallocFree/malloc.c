#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long long gt()
{
  struct timespec tstart={0,0};
  clock_gettime(CLOCK_MONOTONIC,&tstart);
  return tstart.tv_sec*1000000000LL+tstart.tv_nsec;
}
int main(int argc,char*argv[])
{
  if(argc!=2)
  {
    printf("No arg!\n");
    return 1;
  }
  int size = atoi(argv[1]);
  for(int i=0;i<1000;i++)
  {
    long long t1=gt();
    char * buf =(char*)malloc(size);
    long long t2=gt();
    printf("%lld\n",t2-t1);
  }
  return 0;
}
