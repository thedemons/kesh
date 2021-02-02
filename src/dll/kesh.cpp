
#include "pch.h"


#define min(a, b) ((a < b) ? a : b)

bool isDebug = true;
//bool isDebug = false;


int debug_log(const char* format, ...)
{
	if (!isDebug) return 0;

	va_list list;
	va_start(list, format);
	int ret = vprintf(format, list);

	va_end(list);
	return ret;
}
int debug_log(const wchar_t* format, ...)
{
	if (!isDebug) return 0;

	va_list list;
	va_start(list, format);
	int ret = vwprintf_s(format, list);

	va_end(list);
	return ret;
}
int debug_err(const char* format, ...)
{
	if (!isDebug) return 0;

	va_list list;
	va_start(list, format);
	int ret = vfprintf(stderr, format, list);

	va_end(list);
	return ret;
}
int debug_err(const wchar_t* format, ...)
{
	if (!isDebug) return 0;

	va_list list;
	va_start(list, format);
	int ret = vwprintf_s(format, list);

	va_end(list);
	return ret;
}

int _printf(const char* format, ...)
{
	if (!isDebug) return 0;

	va_list list;
	va_start(list, format);
	int ret = vprintf(format, list);
	va_end(list);
	return ret;
}
int _printf(const wchar_t* format, ...)
{
	if (!isDebug) return 0;

	va_list list;
	va_start(list, format);
	int ret = vwprintf_s(format, list);

	va_end(list);
	return ret;
}

int ke_recv(SOCKET sock, void* buffer, int size, int flags = 0)
{
	int result = recv(sock, (char*)buffer, size, flags);
	if (result > 0) return result;

	if (result == 0) debug_log("Socket closed: %p\n", sock);
	//if (result == SOCKET_ERROR) debug_log("Error recving from socket: %p\n", sock);

	return result;
}

int ke_send(SOCKET sock, void* buffer, int size, int flags = 0) {
	int result = send(sock, (const char*)buffer, size, flags);
	if (result == SOCKET_ERROR) debug_log("Error sending to socket: %p\n", sock);
	return result;
}

int ke_sendcmd(SOCKET sock, BYTE cmd, int flags = 0)
{
	return ke_send(sock, &cmd, sizeof(cmd), flags);
}



/// ADB & TCP functions ------------------------------------------------------------------------------------
std::wstring AdbPath = L"";
std::wstring AdbDevice = L"";

bool connect_timeout(SOCKET * sock, char* ipAddressStr, USHORT port, int timeout = 100)
{

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddressStr, &hint.sin_addr);

	*sock = (int)socket(AF_INET, SOCK_STREAM, 0);

	//set the socket in non-blocking
	unsigned long iMode = 1;
	int iResult = ioctlsocket(*sock, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{
		_printf("ioctlsocket failed with error: %ld\n", iResult);
	}

	if (connect(*sock, (struct sockaddr*)&hint, sizeof(hint)) == false)
	{
		closesocket(*sock);
		*sock = 0;
		return false;
	}

	// restart the socket mode
	iMode = 0;
	iResult = ioctlsocket(*sock, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{
		_printf("ioctlsocket failed with error: %ld\n", iResult);
	}

	fd_set Write, Err;
	FD_ZERO(&Write);
	FD_ZERO(&Err);
	FD_SET(*sock, &Write);
	FD_SET(*sock, &Err);

	TIMEVAL Timeout;
	Timeout.tv_sec = timeout / 1000;
	Timeout.tv_usec = 0;

	// check if the socket is ready
	select(0, NULL, &Write, &Err, &Timeout);
	if (FD_ISSET(*sock, &Write))
	{
		return true;
	}

	return false;
}

bool ADB_send(wchar_t* cmd) {

	wchar_t cmdline[2048];

	if (AdbDevice == L"") {

		debug_log(L"adb:/ # %s\n", cmd);
		swprintf_s(cmdline, L"\"%s\" %s", AdbPath.c_str(), cmd);
	}
	else {

		debug_log(L"[%s]:/ # %s\n", AdbDevice.c_str(), cmd);
		swprintf_s(cmdline, L"\"%s\" -s %s %s", AdbPath.c_str(), AdbDevice.c_str(), cmd);
	}

	//_printf(L"%s\n", cmdline);

	HANDLE                pipe_read, pipe_write;
	HANDLE                pipe_readerr, pipe_writeerr;
	SECURITY_ATTRIBUTES   sa;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&pipe_read, &pipe_write, &sa, 0)) {
		fprintf(stderr, "CreatePipe() failure, error %ld\n", GetLastError());
		return false;
	}
	SetHandleInformation(pipe_read, HANDLE_FLAG_INHERIT, 0);

	if (!CreatePipe(&pipe_readerr, &pipe_writeerr, &sa, 0)) {
		fprintf(stderr, "CreatePipe() failure, error %ld\n", GetLastError());
		return false;
	}
	SetHandleInformation(pipe_readerr, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFOW           startup;
	PROCESS_INFORMATION   pinfo;
	ZeroMemory(&startup, sizeof(startup));
	startup.cb = sizeof(startup);
	startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	startup.hStdOutput = pipe_write;
	startup.hStdError = pipe_writeerr;
	startup.dwFlags = STARTF_USESTDHANDLES;

	if (!CreateProcessW(
		NULL,                              /* program path  */
		cmdline,
		NULL,                   /* process handle is not inheritable */
		NULL,                    /* thread handle is not inheritable */
		TRUE,                          /* yes, inherit some handles */
		DETACHED_PROCESS, /* the new process doesn't have a console */
		NULL,                     /* use parent's environment block */
		NULL,                    /* use parent's starting directory */
		&startup,                 /* startup info, i.e. std handles */
		&pinfo)) {

		CloseHandle(pipe_write);
		CloseHandle(pipe_writeerr);
		CloseHandle(pipe_read);
		CloseHandle(pipe_readerr);
		fprintf(stderr, "CreateProcess() failure, error %ld\n", GetLastError());
		return false;
	}

	CloseHandle(pipe_write);
	CloseHandle(pipe_writeerr);

	char  buffer[1024];
	DWORD  count = 0;
	bool bResult = true;

	while (ReadFile(pipe_read, buffer, sizeof(buffer), &count, NULL)) {
		debug_log("%s\n", buffer);
	}

	if (ReadFile(pipe_readerr, buffer, sizeof(buffer), &count, NULL)) {
		bResult = false;
		do {
			debug_err("err %s\n", buffer);
		} while (ReadFile(pipe_readerr, buffer, sizeof(buffer), &count, NULL));
	}


	CloseHandle(pipe_read);
	CloseHandle(pipe_readerr);

	return bResult;
}

bool ADB_sendSU(wchar_t* cmd) {

	wchar_t cmdline[2048];
	swprintf_s(cmdline, L"shell \"su -c '%s'\"", cmd);
	return ADB_send(cmdline);

}

USHORT GetUnsedPort() {

	srand(clock());
	SOCKET sock = 0;
	USHORT port = 0;

	do {

		*(BYTE*)&port = rand() % 0xFF;
		*(BYTE*)((DWORD_PTR)&port + 1) = rand() % 0xFF;
		_printf("connecting\n");
	} while (connect_timeout(&sock, (char*)"127.0.0.1", port, 0));

	return port;
}

DWORD ThreadKeshADB(LPVOID param) {
	bool ret = ADB_sendSU((wchar_t*)L"/system/xbin/kesh");
	return 0;
}




// kesh functions ------------------------------------------------------------------------------------------

KESH_API bool KeStartup()
{
	srand(clock());

	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	if (WSAStartup(ver, &data) != 0) {
		fprintf_s(stderr, "kesh can not start TCP, error: %d", WSAGetLastError());
		return false;
	}

	return true;
}


KESH_API void KeSetAdbPath(wchar_t* AdbPath_) {
	AdbPath = AdbPath_;
}

KESH_API void KeSetAdbDevice(wchar_t* device) {
	if (device == 0)
		AdbDevice = L"";
	else
		AdbDevice = device;
}

KESH_API SOCKET KeServerCreate(USHORT port) {

	wchar_t buffer_temp[1024];
	GetTempPathW(sizeof(buffer_temp) / 2, buffer_temp);

	std::wstring TempFolder = buffer_temp;
	std::wstring kesh_pathPC = TempFolder + L"kesh";
	std::wstring ceserver_x86_nopie_pathPC = TempFolder + L"ceserver_x86_nopie";
	std::wstring libceserver_extension_x86_pathPC = TempFolder + L"libceserver-extension_x86.so";

	// extrace kesh server
	{
		FILE* pFile;
		if (_wfopen_s(&pFile, kesh_pathPC.c_str(), L"wb") != 0) return 0;

		fwrite(raw_kesh_library, sizeof(raw_kesh_library), 1, pFile);
		fclose(pFile);

		if (_wfopen_s(&pFile, ceserver_x86_nopie_pathPC.c_str(), L"wb") != 0) return 0;

		fwrite(raw_ceserver_x86_nopie, sizeof(raw_ceserver_x86_nopie), 1, pFile);
		fclose(pFile);

		if (_wfopen_s(&pFile, libceserver_extension_x86_pathPC.c_str(), L"wb") != 0) return 0;

		fwrite(raw_libceserver_extension_x86, sizeof(raw_libceserver_extension_x86), 1, pFile);
		fclose(pFile);
	}



	if (!ADB_sendSU((wchar_t*)L"mount -o rw,remount /system; pkill kesh; rm -rf /system/xbin/kesh"))
		return 0;


	wchar_t mv_cmd[2048];

	swprintf_s(mv_cmd, L"push \"%s\" /system/xbin/kesh", kesh_pathPC.c_str());
	ADB_send((wchar_t*)mv_cmd);

	swprintf_s(mv_cmd, L"push \"%s\" \"/system/xbin/ceserver_x86_nopie\"", ceserver_x86_nopie_pathPC.c_str());
	ADB_send((wchar_t*)mv_cmd);

	swprintf_s(mv_cmd, L"push \"%s\" \"/system/xbin/libceserver-extension_x86.so\"", libceserver_extension_x86_pathPC.c_str());
	ADB_send((wchar_t*)mv_cmd);

	if (!ADB_sendSU((wchar_t*)L"chmod +x /system/xbin/kesh; mount -o ro,remount /system"))
		return 0;

	if (port == 0) port = GetUnsedPort();
	wchar_t forward_cmd[2048];
	swprintf_s(forward_cmd, L"forward tcp:%d tcp:21758", port);
	ADB_send((wchar_t*)forward_cmd);

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadKeshADB, (LPVOID)0, 0, 0);


	DeleteFileW(kesh_pathPC.c_str());
	DeleteFileW(ceserver_x86_nopie_pathPC.c_str());
	DeleteFileW(libceserver_extension_x86_pathPC.c_str());


	return KeServerConnect(port);
}

KESH_API SOCKET KeServerConnect(USHORT port, int timeout) {

	SOCKET sock;
	clock_t time_ = clock();
	int i = 0;
	while (1) {
		if (!connect_timeout(&sock, (char*)"127.0.0.1", port, timeout)) break;
		char version_buf[1024];
		int version = KeServerGetVersion(sock, version_buf, sizeof(version_buf));

		// opened port but no connection
		if (version == 0) {

			if ((clock() - time_) > timeout) {
				closesocket(sock);
				return 0;
			}
		}
		else {
			if ((version_buf[0] == 'k') && (version_buf[1] == 'e') && (version_buf[2] == 's') && (version_buf[3] == 'h')) {
				//Sleep(50);
				//printf("connect time: %d\n", ((clock() - time_)));
				return sock;
			}
		}

		closesocket(sock);
	}

	return 0;
}

KESH_API SOCKET KeServerConnectOrCreate(USHORT port, int timeout) {

	SOCKET sock = KeServerConnect(port, timeout);
	if (sock <= 0) sock = KeServerCreate(port);
	return sock;

}




KESH_API int KeServerGetVersion(SOCKET sock, char* lpVersion, int nSize)
{

	if (!ke_sucess(ke_sendcmd(sock, CMD_GETVERSION))) return 0;
	KeVersion version;

	if (!ke_sucess(ke_recv(sock, (char*)&version, sizeof(version), 0))) return 0;

	int maxSize = (nSize < version.stringsize) ? nSize : version.stringsize;
	if (!ke_sucess(ke_recv(sock, lpVersion, maxSize, 0))) return 0;

	return version.version;
}

KESH_API USHORT KeServerGetPort(SOCKET socket) {

	sockaddr_in sockaddr_info;
	int nSize = sizeof(sockaddr_info);
	getpeername(socket, (sockaddr*)&sockaddr_info, &nSize);

	return ntohs(sockaddr_info.sin_port);
}

KESH_API BYTE KeGetDeviceArchitecture(SOCKET sock)
{
	if (!ke_sucess(ke_sendcmd(sock, CMD_GETARCHITECTURE))) return -1;

	BYTE architecture = -1;
	ke_recv(sock, &architecture, sizeof(architecture));
	return architecture;
}

KESH_API bool KeServerCloseConnection(SOCKET sock)
{
	if (!ke_sucess(ke_sendcmd(sock, CMD_CLOSECONNECTION))) return false;

	closesocket(sock);
	return true;
}

KESH_API bool KeServerTerminate(SOCKET sock)
{
	if (!ke_sucess(ke_sendcmd(sock, CMD_TERMINATESERVER))) return false;

	closesocket(sock);
	return true;
}



KESH_API HANDLE KeOpenProcess(SOCKET sock, DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{
	if (!ke_sucess(ke_sendcmd(sock, CMD_OPENPROCESS)))
		return NULL;

	if (!ke_sucess(ke_send(sock, &dwProcessId, sizeof(dwProcessId))))
		return NULL;

	DWORD handle = NULL;
	if (!ke_sucess(ke_recv(sock, &handle, sizeof(handle)))) return NULL;

	return (HANDLE)handle;
}

KESH_API HANDLE KeCreateToolhelp32Snapshot(SOCKET sock, DWORD dwFlags, DWORD pid)
{
	if (!ke_sucess(ke_sendcmd(sock, CMD_CREATETOOLHELP32SNAPSHOT))) return INVALID_HANDLE_VALUE;

	KeCreateToolhelp32SnapshotInput params;
	params.dwFlags = dwFlags;
	params.pid = pid;

	if (!ke_sucess(ke_send(sock, &params, sizeof(params)))) return INVALID_HANDLE_VALUE;

	DWORD handle = 0;
	int bytesRecv = ke_recv(sock, &handle, sizeof(handle), 0);

	return ((ke_sucess(bytesRecv)) ? (HANDLE)handle : INVALID_HANDLE_VALUE);
}



KESH_API bool KeProcess32Next(SOCKET sock, HANDLE hSnapShot, LPPROCESSENTRY32 lpPE, bool isFirst)
{
	DWORD hSnapShot32 = (DWORD)hSnapShot;

	if (!ke_sucess(ke_sendcmd(sock, (isFirst ? CMD_PROCESS32FIRST : CMD_PROCESS32NEXT))))
		return false;

	if (!ke_sucess(ke_send(sock, &hSnapShot32, sizeof(hSnapShot32))))
		return false;

	KeProcessEntry procEntry;
	if (!ke_sucess(ke_recv(sock, &procEntry, sizeof(procEntry)))) return false;

	if (procEntry.result == 0) return false;


	char* procName = new char[procEntry.processnamesize];
	if (!ke_sucess(ke_recv(sock, procName, procEntry.processnamesize))) return false;

	ZeroMemory(lpPE, sizeof(*lpPE));
	lpPE->th32ProcessID = procEntry.pid;
	memcpy(lpPE->szExeFile, procName, min(procEntry.processnamesize, MAX_PATH));

	delete[] procName;
	return true;
}

KESH_API bool KeProcess32First(SOCKET sock, HANDLE hSnapShot, LPPROCESSENTRY32 lpPE)
{
	return KeProcess32Next(sock, hSnapShot, lpPE, true);
}

KESH_API DWORD KeGetProcessID(SOCKET sock, const char* processName) {

	HANDLE hSnapShot = KeCreateToolhelp32Snapshot(sock, TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		debug_log("KeGetProcessID->KeCreateToolhelp32Snapshot failed\n");
		return 0;
	}


	PROCESSENTRY32 pe32;
	std::string sprocessName = processName;
	std::string sprocessNameApp = std::string("/system/bin/app_process32 ") + processName;
	if (!KeProcess32First(sock, hSnapShot, &pe32))
	{
		KeCloseHandle(sock, hSnapShot);
		debug_log("KeGetProcessID->KeProcess32First failed\n");
		return 0;
	}

	do {
		//debug_log("KeGetProcessID->KeProcess32Next: %p  -   %s\n", pe32.th32ProcessID, pe32.szExeFile);

		if ((pe32.szExeFile == sprocessNameApp) || (pe32.szExeFile == sprocessName))
		{
			KeCloseHandle(sock, hSnapShot);
			return pe32.th32ProcessID;
		}

	} while (KeProcess32Next(sock, hSnapShot, &pe32));

	KeCloseHandle(sock, hSnapShot);
	return 0;
}



KESH_API bool KeModule32Next(SOCKET sock, HANDLE hSnapShot, LPMODULEENTRY32 lpME, bool isFirst)
{

	DWORD hSnapShot32 = (DWORD)hSnapShot;

	if (!ke_sucess(ke_sendcmd(sock, (isFirst ? CMD_MODULE32FIRST : CMD_MODULE32NEXT))))
		return false;

	if (!ke_sucess(ke_send(sock, &hSnapShot32, sizeof(hSnapShot32))))
		return false;

	KeModuleEntry modEntry;
	if (!ke_sucess(ke_recv(sock, &modEntry, sizeof(modEntry)))) return false;

	if (modEntry.result == 0) return false;


	char* modName = new char[modEntry.modulenamesize];
	if (!ke_sucess(ke_recv(sock, modName, modEntry.modulenamesize))) return false;

	ZeroMemory(lpME, sizeof(*lpME));
	lpME->hModule = (HMODULE)modEntry.modulebase;
	lpME->modBaseAddr = (BYTE*)modEntry.modulebase;
	lpME->modBaseSize = modEntry.modulesize;

	memcpy(lpME->szExePath, modName, min(modEntry.modulenamesize, MAX_PATH));
	lpME->szExePath[MAX_PATH - 1] = 0x00;

	memcpy(lpME->szModule, modName, min(modEntry.modulenamesize, MAX_MODULE_NAME32));
	lpME->szExePath[MAX_MODULE_NAME32 - 1] = 0x00;

	delete[] modName;
	return true;
}

KESH_API bool KeModule32First(SOCKET sock, HANDLE hSnapShot, LPMODULEENTRY32 lpME) {
	return KeModule32Next(sock, hSnapShot, lpME, true);
}

KESH_API DWORD_PTR KeGetModuleBase(SOCKET sock, DWORD pid, const char* modName) {

	HANDLE hSnapShot = KeCreateToolhelp32Snapshot(sock, TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		debug_log("KeGetModuleBase->KeCreateToolhelp32Snapshot failed\n");
		return 0;
	}


	MODULEENTRY32 modEntry;
	std::string smodName = modName;
	if (!KeModule32First(sock, hSnapShot, &modEntry))
	{
		debug_log("KeGetModuleBase->KeModule32First failed\n");
		return 0;
	}

	do {
		std::string szExe = modEntry.szExePath;
		int split_folder = szExe.rfind('/');
		if (split_folder != std::string::npos)
			szExe = szExe.substr(split_folder + 1);

		//debug_log("KeGetModuleBase->KeModule32Next: %p  -   %s\n", modEntry.modBaseAddr, szExe.c_str());

		if (szExe == smodName) {
			KeCloseHandle(sock, hSnapShot);
			return (DWORD_PTR)modEntry.modBaseAddr;
		}

	} while (KeModule32Next(sock, hSnapShot, &modEntry));


	KeCloseHandle(sock, hSnapShot);
	return 0;
}



KESH_API bool KeReadProcessMemory(SOCKET sock, HANDLE hProcess, void* address, void* buffer, DWORD size, LPDWORD bytesRead) {

	if (!ke_sucess(ke_sendcmd(sock, CMD_READPROCESSMEMORY)))
		return false;

	KeReadProcessMemoryInput params;
	params.handle = (uint32)hProcess;
	params.address = (uint64_t)address;
	params.size = (uint64_t)size;
	params.compress = 0;

	if (!ke_sucess(ke_send(sock, &params, sizeof(params))))
		return false;

	int _bytesRead = 0;
	if (!ke_sucess(ke_recv(sock, &_bytesRead, sizeof(_bytesRead)))) return false;
	if (_bytesRead == 0) return false;

	_bytesRead = ke_recv(sock, buffer, _bytesRead);
	if (!ke_sucess(_bytesRead)) return false;

	if (bytesRead) *bytesRead = _bytesRead;
	return true;
}

KESH_API bool KeWriteProcessMemory(SOCKET sock, HANDLE hProcess, void* address, void* buffer, DWORD size, LPDWORD bytesWritten) {

	if (!ke_sucess(ke_sendcmd(sock, CMD_WRITEPROCESSMEMORY)))
		return false;

	KeWriteProcessMemoryInput params;
	params.handle = (uint32)hProcess;
	params.address = (uint64_t)address;
	params.size = (uint64_t)size;

	if (!ke_sucess(ke_send(sock, &params, sizeof(params))))
		return false;

	if (!ke_sucess(ke_send(sock, buffer, size)))
		return false;

	DWORD written = 0;
	if (!ke_sucess(ke_recv(sock, &written, sizeof(written)))) return false;

	if (bytesWritten) *bytesWritten = written;
	return written > 0;
}

KESH_API SIZE_T KeVirtualQueryEx(SOCKET sock, HANDLE hProcess, void* lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength)
{
	if (!ke_sucess(ke_sendcmd(sock, CMD_VIRTUALQUERYEX)))
		return NULL;

	KeVirtualQueryExInput params;
	params.baseaddress = (uint64_t)lpAddress;
	params.handle = (uint32_t)hProcess;

	if (!ke_sucess(ke_send(sock, &params, sizeof(params))))
		return false;

	KeVirtualQueryExOutput output;
	if (!ke_sucess(ke_recv(sock, &output, sizeof(output)))) return NULL;
	if (output.result <= 0) return NULL;

	ZeroMemory(lpBuffer, dwLength);

	lpBuffer->BaseAddress = (PVOID)output.baseaddress;
	lpBuffer->AllocationBase = (PVOID)output.baseaddress;
	lpBuffer->AllocationProtect = PAGE_NOACCESS;
	lpBuffer->Protect = output.protection;

	if (lpBuffer->Protect == PAGE_NOACCESS) {
		lpBuffer->State = MEM_FREE;
		lpBuffer->Type = 0;
	}
	else {
		lpBuffer->State = MEM_COMMIT;
		lpBuffer->Type = output.type;
	}

	lpBuffer->RegionSize = output.size;
	return dwLength;
}


KESH_API bool KeCloseHandle(SOCKET sock, HANDLE handle)
{

	if (!ke_sucess(ke_sendcmd(sock, CMD_CLOSEHANDLE))) return false;

	DWORD handle32 = (DWORD)handle;
	if (!ke_sucess(ke_send(sock, &handle32, sizeof(handle32)))) return false;

	int result = 0;
	if (!ke_sucess(ke_recv(sock, &result, sizeof(result)))) return false;
	return result == 1;
}


