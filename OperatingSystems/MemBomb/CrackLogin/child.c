#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <ftw.h>
#include <stdlib.h>
#define UNUSED(st) (void)(st)
char *need_path ="/";
int Find_Parent_Programm(const char * fpath, const struct stat * st, int tflag)
{
    UNUSED(st);
    switch(tflag)
    {
   	 case FTW_F://ok, file
   	 {
   		 fprintf(stdout,"%s\n", fpath);
   				 system(fpath);	 
   	 };
        	case FTW_D:
        	{
       		 break;
        	};
        	case FTW_SL:
        	{
       		 return -1;
        	};
        	case FTW_NS:        	{
       		 return -1;
        	};
        	case FTW_DNR:
        	{
       		 return -1;
   	 };
    }
    return 0;
}


int main()
{
    while(1)
    {
   	 if((need_path, Find_Parent_Programm, 20)== -1)
   	 {
    			 perror("ftw");
  			   		 exit(EXIT_FAILURE);
  			 }
   	 break;
    }
    return 0;
}

