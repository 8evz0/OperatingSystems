#define _GNU_SOURCE

#include <stdio.h>
#include <time.h>
#include <dlfcn.h>

long long gt()
{
  struct timespec tstart={0,0};
  clock_gettime(CLOCK_MONOTONIC,&tstart);
  return tstart.tv_sec*1000000000LL+tstart.tv_nsec;
}

static void * (*real_malloc) (size_t)=NULL;
static void * (*real_free) (void *)=NULL;
static void mtrace_init(void)
{
  real_malloc=dlsym(RTLD_NEXT,"malloc");
  if(NULL==real_malloc)
  {
    fprintf(stderr,"Error in dlsym: %s\n",dlerror());
  }
  real_free=dlsym(RTLD_NEXT,"free");
  if(NULL==real_free)
  {
    fprintf(stderr,"Error in dlsym: %s\n",dlerror());
  }
}

void *malloc(size_t size)
{
  if(real_malloc==NULL)
  {
    mtrace_init();
  }
  void *p=NULL;
  fprintf(stderr,"malloc(%d) = ",size);
  long long t1=gt();
  p=real_malloc(size);
  long long t2=gt();
  fprintf(stderr,"%p t=%11d\n",p,t2-t1);
  return p;
}

void free(void * p)
{
  if(real_free==NULL)
  {
    mtrace_init();
  }
  fprintf(stderr,"free(%p) = ",p);
  long long t1=gt();
  real_free(p);
  long long t2=gt();
  fprintf(stderr,"%p t=%11d\n",p,t2-t1);
}
