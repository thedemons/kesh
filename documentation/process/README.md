# Process manipulation
### Get a list of processes
- First, we have to open a snapshot handle:
```cpp
    HANDLE hSnapShot = KeCreateToolhelp32Snapshot(sock, TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
        printf("KeCreateToolhelp32Snapshot failed\n");
		return 0;
	}
```
- Then loop through the list of processes using ```KeProcess32Next```:
```cpp
    PROCESSENTRY32 pe32;
    if (!KeProcess32First(sock, hSnapShot, &pe32))
    {
        printf("KeProcess32First failed\n");
        return 0;
    }

    do {
        printf("%d  -  %s\n", pe32.th32ProcessID, pe32.szExeFile);
    } while (KeProcess32Next(sock, hSnapShot, &pe32));
	
	KeCloseHandle(sock, hSnapShot);
```
- The results should look like this
```
    1417  -  /system/bin/app_process32 system_server
    1511  -  /system/bin/sdcard /system/bin/sdcard
    1653  -  /system/bin/app_process32 com.android.phone
    1658  -  /system/bin/app_process32 com.android.systemui
    1799  -  /system/bin/app_process32 android.ext.services
    1821  -  /system/bin/app_process32 com.google.android.gms.persistent
    1838  -  /system/bin/app_process32 com.android.coreservice
    1844  -  /system/bin/app_process32 com.android.printspooler
    ...
```
- The process name includes the path such as ```/system/bin/app_process32 com.google.android.gms.ui```

<br>

### Get the process ID
- You can use ```KeGetProcessID()``` to make everything easier:
```cpp
    DWORD pid = KeGetProcessID(sock, processName)
```
- ```processName``` must included the full path of the process, or it can be ```"com.google.android.gms.ui"``` in this case:
<p align="center"><img src="https://raw.githubusercontent.com/thedemons/kesh/main/documentation/process/ce_processlist.jpg" width="400"></p>

<br>

### Get a list of modules in the process
- Open a snapshot handle
```cpp
    HANDLE hSnapShot = KeCreateToolhelp32Snapshot(sock, TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (hSnapShot == INVALID_HANDLE_VALUE)
    {
        printf("KeCreateToolhelp32Snapshot failed\n");
        return 0;
    }
```
- Loop through the modules list using ```KeModule32Next```:
```cpp
    MODULEENTRY32 modEntry;
    if (!KeModule32First(sock, hSnapShot, &modEntry))
    {
        printf("KeModule32First failed\n");
        return 0;
    }

    do {
        printf("%p  -  %s\n", modEntry.modBaseAddr, modEntry.szExePath);
    } while (KeModule32Next(sock, hSnapShot, &modEntry));
	
	KeCloseHandle(sock, hSnapShot);
```
- The results should look like this
```
    C7620000  -  /system/lib/libRS.so
    C762D000  -  /system/lib/libbinder.so
    C76AD000  -  /system/lib/liblzma.so
    C76DD000  -  /system/lib/libm.so
    C7714000  -  /system/lib/libnbaio.so
    C7720000  -  /system/lib/libsoundtrigger.so
    C7737000  -  /system/lib/libGLESv1_CM.so
    C7741000  -  /system/lib/libsonivox.so
    ...
```

## Get the base address of a module
- You can use ```KeGetModuleBase()``` to make everything easier:
```cpp
    DWORD_PTR baseAddress = KeGetModuleBase(sock, pid, moduleName)
```
- ```moduleName``` doesn't have to include the full path

<br>

### Open handle to the process
- This handle can later be used to manipulate the process
```cpp
    HANDLE hProcess = KeOpenProcess(pid)
```
