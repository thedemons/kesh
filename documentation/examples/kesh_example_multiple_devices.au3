
#include <Array.au3>
#include "kesh.au3"


; open kesh library
If @AutoItX64 Then
	KeDllOpen("kesh64.dll")
Else
	KeDllOpen("kesh.dll")
EndIf

; set path to adb.exe
Local $AdbPath = "D:\LDPlayer\LDPlayer4.0\adb.exe"
KeSetAdbPath($AdbPath)

Local $dev1_name = "emulator-5554"
Local $dev2_name = "127.0.0.1:5555"

; Start kesh on device 1
KeSetAdbDevice($dev1_name)
Local $socket_dev1 = KeServerCreate() ; // Use KeServerCreate($port = 0) to auto select an unsed port, you can also use KeServerConnectOrCreate() with a pre-defined port

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
