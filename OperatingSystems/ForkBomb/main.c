#ifdef __gnu_linux__

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/time.h>

#define PROC_DIR "/proc"
struct timeval start,end;

#endif
#ifdef _WIN64

#include <windows.h>
#include <psapi.h>
#include <time.h>
#include <process.h>

#pragma warning (disable : 4996)
#include <stdio.h> 

double PCFreq;
__int64 CounterStart;

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
#ifdef __gnu_linux__

int is_pid_folder(const struct dirent *entry)
{
const char *p;

for(p=entry->d_name; *p;p++)
{
if(!isdigit(*p))
{
return 0;
}
}
return 1;
}

#endif
#ifdef _WIN64

int StartUpInfoFileWriter()
{
FILE* f = fopen("result.txt", "a+");
if (f == NULL)
{
printf("Can't open file!");
}
else
{
int Count = 0; 

DWORD aProcesses[8096], cbNeeded, cProcesses; 

if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
{
printf("EnumProcesses() Error \t%d", GetLastError());
}
else
{
printf("EnumProcesses() is OK!\n");
}

cProcesses = cbNeeded / sizeof(DWORD);

fprintf(f, "Process;Time\n");
fprintf(f, "%d;0\n", cProcesses);
fclose(f);
}
return 0;
}

int FileWriter()
{
FILE* f = fopen("result.txt", "a+");
if (f == NULL) 
{
printf("Can't open file!");
}
else 
{
int Count = 0; 
DWORD aProcesses[8096], cbNeeded, cProcesses;

if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
{
printf("EnumProcesses() Error \t%d", GetLastError());
}
else
{
printf("EnumProcesses() is OK!\n");
}

cProcesses = cbNeeded / sizeof(DWORD);

fprintf(f, "%d;%f\n", cProcesses, GetCounter());
fclose(f);
}
return 0;
}

#endif
#ifdef __gnu_linux__

int StartInfoWriter()
{
FILE *fp;
char fname[]="result.txt";
fp=fopen(fname,"a+");

int CountOfProc=0;
DIR *procdir;
struct dirent *entry;

procdir=opendir("/proc");
if(!procdir)
{
perror("opendir failed");
return 1;
}

while ((entry=readdir(procdir)))
{
if(!is_pid_folder(entry))
{
continue;
}
CountOfProc=CountOfProc+1;

}
fprintf(fp,"%d;%d.%d\n",CountOfProc,0,0);
fclose(fp);
return 0;
}

float time_diff(struct timeval *start,struct timeval *end)
{
return (end->tv_sec-start->tv_sec)+1e-6*(end->tv_usec-start->tv_usec);
}

int FileWriter()
{
FILE *fp;
char fname[]="result.txt";
fp=fopen(fname,"a+");

int CountOfProc=0;
DIR *procdir;
struct dirent *entry;

procdir=opendir("/proc");
if(!procdir)
{
perror("opendir failed");
return 1;
}

while ((entry=readdir(procdir)))
{
if(!is_pid_folder(entry))
{
continue;
}
CountOfProc=CountOfProc+1;

}

fprintf(fp,"%d;%0.8f\n",CountOfProc,time_diff(&start,&end));
fclose(fp);
return 0;
}
#endif
#ifdef _WIN64

int main()
{
StartCounter();
StartUpInfoFileWriter();


TCHAR szCommanndLine[] =TEXT("C:\\Users\\study\\Desktop\\test\\openprocess.exe");

while (1) 
{
CreateProcess(NULL, szCommanndLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
Sleep(3);
FileWriter();
}
}

#endif
#ifdef __gnu_linux__

int main()
{
StartInfoWriter();

gettimeofday(&start, NULL);
while (1)
{
fork();
gettimeofday(&end, NULL);
FileWriter();
}
return 0;
}
#endif





