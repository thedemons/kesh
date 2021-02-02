# Start kesh and use with Cheat Engine

### How does kesh work?
- **kesh** load a process **kesh server** on the android device that communicate with windows using **tcp socket**.
- It uses the **kesh server** to manipulate ```memory, process, module, thread,..``` directly on the android device.
- The **kesh server** is a modified version of the [ceserver](https://github.com/cheat-engine/cheat-engine/tree/master/Cheat%20Engine/ceserver "ceserver") from **Cheat Engine**
<br>

### Start kesh server on the android device
- First you need to specify the **adb.exe** path
```cpp
    KeSetAdbPath((wchar_t*)L"D:\\Program Files\\Nox\\bin\\nox_adb.exe");
```
- Specify the **adb device** if needed
```cpp
    KeSetAdbDevice((wchar_t*)L"emulator-5554");
```
- Inject and start the **kesh server** on the android device
```cpp
    SOCKET sock = KeServerCreate(USHORT port = 0) ;// if port = 0 then it'll select a random unsed port
```
- The port can then be retrieved using ```KeServerGetPort(sock)```

<br>

### Connect to an existing kesh server
- If you already start the **kesh server** , you can connect it using this
```cpp
    KeServerConnect(21758) ;// connect to port 21758
```
- Or start the **kesh server** if can't connect to it
```cpp
    KeServerConnectOrCreate(21758)
```

<br>

### Use Cheat Engine with kesh server
- After you start the **kesh server**, you can connect **Cheat Engine** to it
- Click the **Select Process** button, then click **Network**, enter the port number you set when you start the **kesh server**, then click **Connect**.<br>
<p align="center"><img src="https://raw.githubusercontent.com/thedemons/kesh/main/documentation/ce_setup.jpg" width="350"></p>
