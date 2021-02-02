# Examples
### Setup kesh
```autoit
; open kesh library
If @AutoItX64 Then
    KeDllOpen("kesh64.dll")
Else
    KeDllOpen("kesh.dll")
EndIf

; set path to adb.exe
Local $AdbPath = "D:\LDPlayer\LDPlayer4.0\adb.exe"
KeSetAdbPath($AdbPath)
```

### Start the kesh server
```autoit
; connect to the kesh server on port 21758, if the server isn't running then start it
Local $socket = KeServerConnectOrCreate(21758)

If $socket <= 0 Then
    MsgBox(16, "Error", "Failed to open kesh server")
    Exit
EndIf
```

### Get process id and open its handle
```autoit
; get pid of the process "com.google.android.gms"
Local $pid = KeGetProcessID("com.google.android.gms")
; get process handle
Local $hProcess = KeOpenProcess($pid)
```

### Read and write the process memory
```autoit
; get the base address of the module "app_process32"
Local $baseAddress = KeGetModuleBase($pid, "app_process32")
; read the value at the base address
Local $readBase = KeReadInt($hProcess, $baseAddress)
; write a value to the base address
KeWriteInt($hProcess, $baseAddress, 999999)
```

### Use kesh with multiple devices
```autoit
Local $dev1_name = "emulator-5554"
Local $dev2_name = "127.0.0.1:5555"

; Start kesh on device 1
KeSetAdbDevice($dev1_name)
; Use KeServerCreate($port = 0) to auto select an unsed port,
; you can also use KeServerConnectOrCreate() with a pre-defined port
Local $socket_dev1 = KeServerCreate() 

; Start kesh on device 2
KeSetAdbDevice($dev2_name)
Local $socket_dev2 = KeServerCreate()

; Switch to device 1
KeSetAdbDevice($dev1_name)
KeServerSetSocket($socket_dev1)
; Get a list of process on device 1
_ArrayDisplay(KeGetProcessList())

; Switch to device 2
KeSetAdbDevice($dev2_name)
KeServerSetSocket($socket_dev2)
; Get a list of process on device 1
_ArrayDisplay(KeGetProcessList())
```
