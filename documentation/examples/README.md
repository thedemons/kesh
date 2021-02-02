# Examples
### Setup kesh
```cpp
    KeStartup()
    KeSetAdbPath($AdbPath)
```

### Specify the device (if needed)
```cpp
    KeSetAdbDevice((wchar_t*)L"emulator-5554");
```

### Start the kesh server
```cpp
    SOCKET sock = KeServerConnectOrCreate(12344);
    if (sock == 0) {
        fprintf_s(stderr, "Can not create kesh server\n");
        return 0;
    }
```

### Get process id and open its handle
```cpp
    int app_pid = KeGetProcessID(sock, "com.google.android.gms");
    if (app_pid == 0) {
        fprintf_s(stderr, "Can not get process id\n");
        return 0;
    }
    
    printf("App PID: %d\n", app_pid);

    HANDLE hProcess = KeOpenProcess(sock, 0, false, app_pid);
    if (hProcess == 0) {
        fprintf_s(stderr, "Can not get process handle\n");
        return 0;
    }
    
    printf("Process handle: 0x%p\n", hProcess);
```

### Get module base address
```cpp
    DWORD_PTR baseAddr = KeGetModuleBase(sock, app_pid, "app_process32");
    printf("Base address of app_process32: 0x%p\n", baseAddr);
```

### Read and write the process memory
```cpp
    int readBase = 0;
    KeReadProcessMemory(sock, hProcess, (void*)baseAddr, &readBase, sizeof(readBase));
    printf("Base value: %d\n", readBase);

    int newValue = 999;
    printf("Writing %d to the base address\n", newValue);
    KeWriteProcessMemory(sock, hProcess, (void*)baseAddr, &newValue, sizeof(newValue));

    int readBaseNew = 0;
    KeReadProcessMemory(sock, hProcess, (void*)baseAddr, &readBaseNew, sizeof(readBaseNew));
    printf("New base value: %d\n", readBaseNew);

    printf("Restoring base address value\n");
    KeWriteProcessMemory(sock, hProcess, (void*)baseAddr, &readBase, sizeof(readBase));
```
