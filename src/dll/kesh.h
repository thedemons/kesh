
#ifndef KESH_MEMORY_H
#define KESH_MEMORY_H

#pragma-once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#include <iomanip>
#pragma comment(lib, "ws2_32.lib")
#endif // !WIN32_LEAN_AND_MEAN

#include <Windows.h>


#define KESH_API extern "C" __declspec(dllexport)

KESH_API bool KeStartup();

KESH_API void KeSetAdbPath(wchar_t* AdbPath_);
KESH_API void KeSetAdbDevice(wchar_t* device = 0);

KESH_API SOCKET KeServerCreate(USHORT port = 0);
KESH_API SOCKET KeServerConnect(USHORT port, int timeout = 1000);
KESH_API SOCKET KeServerConnectOrCreate(USHORT port, int timeout = 1000);

KESH_API int KeServerGetVersion(SOCKET sock, char* lpVersion, int nSize);
KESH_API USHORT KeServerGetPort(SOCKET socket);
KESH_API BYTE KeGetDeviceArchitecture(SOCKET sock);
KESH_API bool KeServerCloseConnection(SOCKET sock);
KESH_API bool KeServerTerminate(SOCKET sock);



KESH_API HANDLE KeOpenProcess(SOCKET sock, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
KESH_API HANDLE KeCreateToolhelp32Snapshot(SOCKET sock, DWORD dwFlags, DWORD pid);

KESH_API bool KeProcess32Next(SOCKET sock, HANDLE hSnapShot, LPPROCESSENTRY32 lpPE, bool isFirst = false);
KESH_API bool KeProcess32First(SOCKET sock, HANDLE hSnapShot, LPPROCESSENTRY32 lpPE);
KESH_API DWORD KeGetProcessID(SOCKET sock, const char* processName);

KESH_API bool KeModule32Next(SOCKET sock, HANDLE hSnapShot, LPMODULEENTRY32 lpME, bool isFirst = false);
KESH_API bool KeModule32First(SOCKET sock, HANDLE hSnapShot, LPMODULEENTRY32 lpME);
KESH_API DWORD_PTR KeGetModuleBase(SOCKET sock, DWORD pid, const char* modName);


KESH_API bool KeReadProcessMemory(SOCKET sock, HANDLE hProcess, void* address, void* buffer, DWORD size, LPDWORD bytesRead = 0);
KESH_API bool KeWriteProcessMemory(SOCKET sock, HANDLE hProcess, void* address, void* buffer, DWORD size, LPDWORD bytesWritten = 0);
KESH_API SIZE_T KeVirtualQueryEx(SOCKET sock, HANDLE hProcess, void* lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);

KESH_API bool KeCloseHandle(SOCKET sock, HANDLE handle);






#define ke_sucess(a) (a>0)

#define CMD_GETVERSION 0
#define CMD_CLOSECONNECTION 1
#define CMD_TERMINATESERVER 2
#define CMD_OPENPROCESS 3
#define CMD_CREATETOOLHELP32SNAPSHOT 4
#define CMD_PROCESS32FIRST 5
#define CMD_PROCESS32NEXT 6
#define CMD_CLOSEHANDLE 7
#define CMD_VIRTUALQUERYEX 8
#define CMD_READPROCESSMEMORY 9
#define CMD_WRITEPROCESSMEMORY 10
#define CMD_STARTDEBUG 11
#define CMD_STOPDEBUG 12
#define CMD_WAITFORDEBUGEVENT 13
#define CMD_CONTINUEFROMDEBUGEVENT 14
#define CMD_SETBREAKPOINT 15
#define CMD_REMOVEBREAKPOINT 16
#define CMD_SUSPENDTHREAD 17
#define CMD_RESUMETHREAD 18
#define CMD_GETTHREADCONTEXT 19
#define CMD_SETTHREADCONTEXT 20
#define CMD_GETARCHITECTURE 21
#define CMD_MODULE32FIRST 22
#define CMD_MODULE32NEXT 23
#define CMD_GETSYMBOLLISTFROMFILE 24
#define CMD_LOADEXTENSION         25
#define CMD_ALLOC                   26
#define CMD_FREE                    27
#define CMD_CREATETHREAD            28
#define CMD_LOADMODULE              29
#define CMD_SPEEDHACK_SETSPEED      30
#define CMD_VIRTUALQUERYEXFULL      31
#define CMD_GETREGIONINFO           32
#define CMD_AOBSCAN					200
#define CMD_COMMANDLIST2            255


// GetArchitecture()
#define CE_ARCHITECTURE_i386 0
#define CE_ARCHITECTURE_x86_64 1
#define CE_ARCHITECTURE_ARM 2
#define CE_ARCHITECTURE_AARCH64 3

#pragma pack(push, 1)

typedef struct {
	int version;
	BYTE stringsize;
} KeVersion, * PKeVersion;

typedef struct {
	int result;
	__int64 modulebase;
	DWORD modulesize;
	DWORD modulenamesize;
} KeModuleEntry, * PKeModuleEntry;

typedef struct {
	int result;
	DWORD pid;
	DWORD processnamesize;
} KeProcessEntry, * PKeProcessEntry;

typedef struct {
	DWORD dwFlags;
	DWORD pid;
} KeCreateToolhelp32SnapshotInput, * PKeCreateToolhelp32SnapshotInput;

typedef struct {
	uint32_t handle;
	uint64_t address;
	uint32_t size;
	uint8_t  compress;
} KeReadProcessMemoryInput, * PKeReadProcessMemoryInput;

typedef struct {
	uint32_t handle;
	uint64_t address;
	uint32_t size;
} KeWriteProcessMemoryInput, * PKeWriteProcessMemoryInput;

typedef struct {
	uint32_t handle;
	uint64_t baseaddress;
} KeVirtualQueryExInput, * PKeVirtualQueryExInput;

typedef struct {
	uint8_t result;
	uint32_t protection;
	uint32_t type;
	uint64_t baseaddress;
	uint64_t size;
} KeVirtualQueryExOutput, * PKeVirtualQueryExOutput;

typedef struct {
	int handle;
	uint8_t flags;
} KeVirtualQueryExFullInput, * PKeVirtualQueryExFullInput;

#pragma pack(pop)

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef          char   int8;
typedef   signed char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef   signed short  sint16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef   signed int    sint32;
typedef unsigned int    uint32;



//unsigned char raw_kesh_library2[74764] = { 0,1,2 };

#endif // !KESH_MEMORY_H