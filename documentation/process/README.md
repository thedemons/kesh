# Process manipulation
### Get a list of processes
- To get a list of all running processes, use:
```autoit
    $aProcess = KeGetProcessList()
    _ArrayDisplay($aProcess)
```
- It will return a 2D array of process id and process name, the results should look like this <p align="center"><img src="https://raw.githubusercontent.com/thedemons/kesh-autoit/main/documentation/process/getprocesslistresult.png" width="500"></p>
- The process name includes the path such as ```/system/bin/app_process32 com.google.android.gms.ui```

<br>

### Get the process ID
- The process ID can be retrieved using:
```autoit
    $pid = KeGetProcessID($processName)
```
- ```$processName``` is the name of process returned by ```KeGetProcessList()```. Or it can be ```"com.google.android.gms.ui"``` in this case:
<p align="center"><img src="https://raw.githubusercontent.com/thedemons/kesh-autoit/main/documentation/process/ce_processlist.jpg" width="400"></p>

<br>

### Get a list of modules in the process
- To get a list of all modules in the process, use:
```autoit
    $aModule = KeGetProcessModuleList($pid)
    _ArrayDisplay($aModule)
```
- This function returns a 2D array of module base address and name, the results should look like this:<p align="center"><img src="https://raw.githubusercontent.com/thedemons/kesh-autoit/main/documentation/process/getprocessmodulelistresult.jpg" width="500"></p>

## Get the base address of a module
- Get the base address of the module ```app_process32```:
```autoit
    $baseAddress = KeGetModuleBase($pid, "app_process32")
```

<br>

### Open handle to the process
- This handle can later be used to manipulate the process
```autoit
    $hProcess = KeOpenProcess($pid)
```
