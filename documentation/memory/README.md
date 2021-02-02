# Memory manipulation
### Read a value from memory
- The usage of ```KeReadProcessMemory``` is similar to [Win API ReadProcessMemory](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-readprocessmemory)
```cpp
    int readValue = 0;
    DWORD_PTR addr = 0x384DCF50;
    if (!KeReadProcessMemory(sock, hProcess, (void*)addr, &readValue, sizeof(readValue)))
    {
        printf("KeReadProcessMemory failed\n");
    }
    else
    {
        printf("Value at 0x%p: %d\n", addr, readBase);
    }
```
<br>

### Write a value to memory
- The usage of ```KeWriteProcessMemory``` is similar to [Win API WriteProcessMemory](https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-writeprocessmemory)
```cpp
    int value = 6969;
    DWORD_PTR addr = 0x384DCF50;
    if (!KeWriteProcessMemory(sock, hProcess, (void*)addr, &value, sizeof(value)))
    {
        printf("KeWriteProcessMemory failed\n");
    }
```
