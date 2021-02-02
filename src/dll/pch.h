

#ifndef PCH_H
#define PCH_H

#pragma once

#define IS_DLL


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#include <iomanip>
#pragma comment(lib, "ws2_32.lib")
#endif // !WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <TlHelp32.h>
#include <psapi.h>
#include <iostream>
#include <atlbase.h>
#include "kesh.h"




#endif //PCH_H
