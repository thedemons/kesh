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

; connect to the kesh server on port 21758, if the server isn't running then start it
Local $socket = KeServerConnectOrCreate(21758)

If $socket <= 0 Then
	MsgBox(16, "Error", "Failed to open kesh server")
	Exit
EndIf

; get list of all process
Local $aProcessList = KeGetProcessList()
_ArrayDisplay($aProcessList)

; get pid of "com.google.android.gms"
Local $processName = "com.google.android.gms" ; "/system/bin/app_process32 com.google.android.gms"
Local $pid = KeGetProcessID($processName)
MsgBox(0,"PID of " & $processName, $pid)

; get list of all module in the process
Local $aModuleList = KeGetProcessModuleList($pid)
_ArrayDisplay($aModuleList)

; get process handle
Local $hProcess = KeOpenProcess($pid)
MsgBox(0,"Process handle", $hProcess)

; get module base address
Local $baseAddress = KeGetModuleBase($pid, "app_process32")
MsgBox(0,"Module base address", $baseAddress)

; read base address value
Local $readBase = KeReadInt($hProcess, $baseAddress)
MsgBox(0,"Value at base address", $readBase)

; write base address value
KeWriteInt($hProcess, $baseAddress, 999999)

; read the value again
Local $readBaseNew = KeReadInt($hProcess, $baseAddress)
MsgBox(0,"New value at base address", $readBaseNew)

; restore the value
KeWriteInt($hProcess, $baseAddress, $readBase)
