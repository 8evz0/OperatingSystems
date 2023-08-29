#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <cpuid.h>

#ifdef _WIN64
#include <stdio.h>
#include <Windows.h>
#include <wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")
int main() {
    HKEY hKey;
    char vmwareKey[] = "SOFTWARE\\VMware, Inc.\\VMware Tools";
    char vboxKey[] = "SOFTWARE\\Oracle\\VirtualBox Guest Additions";
    char hyperVKey[] = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Virtualization\\";
    char qemuKey[] = "HARDWARE\\Description\\System\\BIOS\\";
    

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, vmwareKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        printf("VMware virtual machine detected.\n");
        RegCloseKey(hKey);
    } 
    else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, vboxKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        printf("VirtualBox virtual machine detected.\n");
        RegCloseKey(hKey);
    }
    else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, hyperVKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        printf("Hyper-V virtual machine detected.\n");
        RegCloseKey(hKey);
    }
    else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, qemuKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        printf("QEMU virtual machine detected.\n");
        RegCloseKey(hKey);
    }
    else {
        printf("No virtual machine detected.\n");
    }


    HRESULT hres;

    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        printf("Error during COM library initialization. Error code: 0x%x\n", hres);
        return 1;
    }

    hres = CoInitializeSecurity(
        NULL,
        -1,
        NULL,
        NULL,
        RPC_C_AUTHN_LEVEL_DEFAULT,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);
    if (FAILED(hres))
    {
        printf("Error during initialization of COM library security. Error code: 0x%x\n", hres);
        CoUninitialize();
        return 1;
    }

    IWbemLocator* pLoc = NULL;
    hres = CoCreateInstance(
        CLSID_WbemLocator,
        0,
        CLSCTX_INPROC_SERVER,
        IID_IWbemLocator,
        (LPVOID*)&pLoc);
    if (FAILED(hres))
    {
        printf("Error while creating an IWbemLocator instance. Error code: 0x%x\n", hres);
        CoUninitialize();
        return 1;
    }

    IWbemServices* pSvc = NULL;
    hres = pLoc->ConnectServer(
        _bstr_t(L"ROOT\\CIMV2"),
        NULL,
        NULL,
        0,
        NULL,
        0,
        0,
        &pSvc);
    if (FAILED(hres))
    {
        printf("Error connecting to WMI. Error code: 0x%x\n", hres);
        pLoc->Release();
        CoUninitialize();
        return 1;
    }


    hres = CoSetProxyBlanket(
        pSvc,
        RPC_C_AUTHN_WINNT,
        RPC_C_AUTHZ_NONE,
        NULL,
        RPC_C_AUTHN_LEVEL_CALL,
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE);
    if (FAILED(hres))
    {
        printf("Error setting authentication level. Error code: 0x%x\n", hres);
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }


    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_ComputerSystem WHERE Model LIKE '%Virtual Machine%'"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        NULL,
        &pEnumerator);
    if (FAILED(hres))
    {
        printf("Error while executing WQL query. Error code: 0x%x\n", hres);
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }


    IWbemClassObject* pClassObj = NULL;
    ULONG uReturn = 0;
    while (pEnumerator)
    {
        hres = pEnumerator->Next(WBEM_INFINITE, 1, &pClassObj, &uReturn);
        if (0 == uReturn)
            break;

        VARIANT vtProp;
        hres = pClassObj->Get(L"Name", 0, &vtProp, 0, 0);
        if (SUCCEEDED(hres))
        {
            printf("Detected virtual machine: %ws\n", vtProp.bstrVal);
            VariantClear(&vtProp);
        }
        pClassObj->Release();
    }

    pSvc->Release();
    pLoc->Release();
    CoUninitialize();

    return 0;
}
#endif



bool detect_hypervisor_cpuid() {
    unsigned int eax = 0x1, ebx = 0, ecx = 0, edx = 0;
    __cpuid(eax, eax, ebx, ecx, edx);
    return (bool)((ecx >> 31) & 1);
}

const char* detectVirtualMachineByMACAddress(const char* macAddress) {
    const char* virtualBoxPrefixes[] = {"080020", "080026", "080024", "08002D", "08002E", "080027"};
    const char* parallelsPrefix = "001C42";
    const char* virtualPCPrefixes[] = {"0003FF", "000D3A", "0050F2", "7C1E52", "00125A", "00155D", "0017FA", "281878", "7CED8D", "001DD8", "6045BD", "DCB4C4"};
    const char* vmwarePrefix = "001C42";
    
    const char* firstThreeBytes = strndup(macAddress, 6);
    
    if (strcmp(firstThreeBytes, parallelsPrefix) == 0) {
        return "Parallels Workstation";
    }
    
    for (int i = 0; i < sizeof(virtualBoxPrefixes) / sizeof(virtualBoxPrefixes[0]); ++i) {
        if (strcmp(firstThreeBytes, virtualBoxPrefixes[i]) == 0) {
            return "VirtualBox";
        }
    }
    
    for (int i = 0; i < sizeof(virtualPCPrefixes) / sizeof(virtualPCPrefixes[0]); ++i) {
        if (strcmp(firstThreeBytes, virtualPCPrefixes[i]) == 0) {
            return "Virtual PC";
        }
    }
    
    if (strcmp(firstThreeBytes, vmwarePrefix) == 0) {
        return "VMware Workstation";
    }
    
    return "Unknown virtual machine";
}

int detectVirtualMachineByTools(){
	int result = 0;
	FILE *fp;
	char output[128];

	fp = popen("hostnamectl", "r");
	if (fp == NULL) {
		printf("Error while executing the hostnamectl\n command");
		return result;
	}

	while (fgets(output, sizeof(output), fp) != NULL) {
		if (strstr(output, "Virtualization:") != NULL) {
			if (strstr(output, "none") == NULL) {
				result = 1; 
			}
			break;
		}
	}

	pclose(fp);

	return result;
}

int VMProccesAndObjectFind(){
		
}

int VMps(const char* processName) {

    const char* vmProcessNames[] = {
        "vmware",
        "vbox",
        "qemu",
    };

    int numVMProcesses = sizeof(vmProcessNames) / sizeof(vmProcessNames[0]);

    for (int i = 0; i < numVMProcesses; i++) {
        if (strstr(processName, vmProcessNames[i]) != NULL) {
            return 1; 
        }
    }
    return 0; 
}

bool AnalyseProcessAndObjects()
{
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            if (strstr(buffer, "hypervisor") != NULL) {
                fclose(fp);
                return true;
            }
        }
        fclose(fp);
    }

    if (access("/sys/hypervisor/uuid", F_OK) != -1) {
        return true;
    }

    fp = fopen("/proc/cpuinfo", "r");
    if (fp) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            if (strstr(buffer, "QEMU Virtual CPU") != NULL) {
                fclose(fp);
                return true;
            }
        }
        fclose(fp);
    }

    if (access("/sys/devices/virtual/dmi/id/product_name", F_OK) != -1)   {
        return true;
    }

    if (access("/proc/xen/capabilities", F_OK) != -1) {
        return true;
    }

    fp = fopen("/proc/scsi/scsi", "r");
    if (fp) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            if (strstr(buffer, "VMware") != NULL) {
                fclose(fp);
                return true;
            }
        }
        fclose(fp);
    }

    fp = fopen("/proc/scsi/scsi", "r");
    if (fp) {
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            if (strstr(buffer, "VirtualBox") != NULL) {
                fclose(fp);
                return true;
            }
        }
        fclose(fp);
    }


    if (access("/dev/vboxguest", F_OK) != -1) {
        return true;
    }


    if (access("/dev/vmware", F_OK) != -1) {
        return true;
    }


    if (access("/dev/xen", F_OK) != -1) {
        return true;
    }

    return false;
    
}

int FindDirAndFiles() {

    if (access("/usr/bin/VBoxManage", F_OK) != -1 ||
        access("/Applications/VirtualBox.app", F_OK) != -1) {
        return 1;
    }


    if (access("/usr/bin/vmrun", F_OK) != -1 ||
        access("/Applications/VMware Fusion.app", F_OK) != -1) {
        return 1;
    }

    if (access("/usr/bin/qemu-system-x86_64", F_OK) != -1) {
        return 1;
    }

    return 0; 
}


    
int BaseVMAddress()
{
    unsigned short ldt_base;
    unsigned long gdt_base;


    asm("sldt %0" : "=m" (ldt_base));

    // Получаем базовый адрес GDT
    asm("sgdt %0" : "=m" (gdt_base));


    if (ldt_base >= 0x0000)
    {
        return 1;
    }

    if (gdt_base >= 0x00000000)
    {
        return 1;
    }

    return 0;
}

int main() 
{

    printf("$ Detecting VM operation by MAC address\n");

    FILE* pipe = popen("ifconfig | awk '/ether/{print $2}'", "r");
    if (pipe == NULL) {
        printf("Error while executing the ifconfig\n command");
        return 1;
    }
    
    char macAddress[18];
    if (fgets(macAddress, sizeof(macAddress), pipe) == NULL) {
        printf("Failed to get MAC address\n");
        pclose(pipe);
        return 1;
    }
   
    pclose(pipe);
    
    size_t len = strlen(macAddress);
    if (macAddress[len - 1] == '\n') {
        macAddress[len - 1] = '\0';
    }
    char* ptr = macAddress;
    while ((ptr = strchr(ptr, ':')) != NULL) {
        memmove(ptr, ptr + 1, strlen(ptr));
    }
    const char* virtualMachine = detectVirtualMachineByMACAddress(macAddress);
    printf("Detected virtual machine: %s\n", virtualMachine);

    
    printf("$ Detecting VM operation using built-in Linux tools\n");
    if (detectVirtualMachineByTools()) {
        printf("VirtualMachine detected!\n");
    } else {
        printf("No virtual machine detected!\n");
    }
    
    printf("$ Detecting VM operation with auxiliary processes and objects\n");
    DIR* dir;
    struct dirent* entry;

    dir = opendir("/proc");
    if (dir == NULL) {
        printf("Error opening directory /proc\n");
        return 1;
    }

    printf("Detected virtual machine processes:\n");

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            int isNumeric = 1;
            for (int i = 0; i < strlen(entry->d_name); i++) {
                if (entry->d_name[i] < '0' || entry->d_name[i] > '9') {
                    isNumeric = 0;
                    break;
                }
            }

            if (isNumeric) {
                char cmdlinePath[286];
                snprintf(cmdlinePath, sizeof(cmdlinePath), "/proc/%s/cmdline", entry->d_name);
                FILE* cmdlineFile = fopen(cmdlinePath, "r");
                if (cmdlineFile) {
                    char processName[256];
                    fgets(processName, sizeof(processName), cmdlineFile);
                    if (VMps(processName)) {
                        printf("%s\n", processName);
                    }

                    fclose(cmdlineFile);
                }
            }
        }
    }
    closedir(dir);
    
    printf("$ Analyze auxiliary processes and objects\n");
    if (AnalyseProcessAndObjects()) {
        printf("Virtual machine operation detected.\n");
    } else {
        printf("Physical machine operation detected.\n");
    }
    
    bool hypervisor_detected = false;

    hypervisor_detected = detect_hypervisor_cpuid();
    if (hypervisor_detected) {
        printf("$ Using the __cpuid function to check the hypervisor: Detected hypervisor operation\n");
    } 
    else 
    {
      printf("$ Using the __cpuid function to test the hypervisor: No hypervisor detected\n");
    }

    FILE *fp = fopen("/proc/xen/capabilities", "r");
    if (fp != NULL) {
        hypervisor_detected = true;
        fclose(fp);
        printf("$ $ Checking for /proc/xen/capabilities: Hypervisor operation detected\n");
    } else {
        hypervisor_detected = false;
        printf("$ Checking for /proc/xen/capabilities: Hypervisor not detected\n");
    }

    fp = popen("dmesg | grep -i hypervisor", "r");
    if (fp != NULL) {
        char output[100];
        if (fgets(output, sizeof(output), fp) != NULL) {
            hypervisor_detected = true;
            printf("$ Checking for the hypervisor string in the output of the dmesg command: Hypervisor operation detected: %s", output);
        } else {
            hypervisor_detected = false;
            printf("$ Checking for the hypervisor string in the dmesg command output: No hypervisor detected\n");
        }
        pclose(fp);
    } else {
        hypervisor_detected = false;
        printf("$ Checking for the hypervisor string in the output of the dmesg command: Hypervisor not detected\n");
    }
    
    
    printf("$ Find files and directories\n");
    if (FindDirAndFiles()) {
        printf("$ Virtual machine operation detected.\n");
    }
    else {
        printf("$ Virtual machine not detected.\n");
    }

    printf("$ BaseVMAddress analysis\n");
    if (BaseVMAddress())
    {
        printf("$ Virtual machine\n");
    }
    else
    {
        printf("$ Real host\n");
    }
    return 0;
}
  

