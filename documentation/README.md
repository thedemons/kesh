# Start kesh and use with Cheat Engine

### How does kesh work?
- **kesh** load a process **kesh server** on the android device that communicate with windows using **tcp socket**.
- It uses the **kesh server** to manipulate ```memory, process, module, thread,..``` directly on the android device.
- The **kesh server** is a modified version of the [ceserver](https://github.com/cheat-engine/cheat-engine/tree/master/Cheat%20Engine/ceserver "ceserver") from **Cheat Engine**
<br>

### Start kesh server on the android device
- First you need to load the kesh.dll
```autoit
    If @AutoItX64 Then
        KeDllOpen("kesh64.dll")
    Else
        KeDllOpen("kesh.dll")
    EndIf
```
- Then specify the **adb.exe** path
```autoit
    KeSetAdbPath("D:\LDPlayer\LDPlayer4.0\adb.exe")
```
- Specify the **adb device** if needed
```autoit
    KeSetAdbDevice("emulator-5555")
```
- Inject and start the **kesh server** on the android device
```autoit
    $socket = KeServerCreate($port = 0) ;// if $port = 0 then it'll select a random unsed port
```
- The port can then be retrieved using ```KeServerGetPort($socket)```

<br>

### Connect to an existing kesh server
- If you already start the **kesh server** , you can connect it using this
```autoit
    KeServerConnect(21758) ;// connect to port 21758
```
- Or start the **kesh server** if can't connect to it
```autoit
    KeServerConnectOrCreate(21758)
```

<br>

### Use Cheat Engine with kesh server
- After you start the **kesh server**, you can connect **Cheat Engine** to it
- Click the **Select Process** button, then click **Network**, enter the port number you set when you start the **kesh server**, then click **Connect**.<br>
<p align="center"><img src="https://raw.githubusercontent.com/thedemons/kesh-autoit/main/documentation/ce_setup.jpg" width="350"></p>
