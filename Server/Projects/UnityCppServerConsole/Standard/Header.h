#pragma once
// Warning Ignore
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4505 )

// Windows Sockets
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <process.h>

// Characters
#include <string>
#include <tchar.h>

// Standard Template Libraries
#include <vector>
#include <list>
#include <queue>

// Standard
#include <iostream>

// Global
#include "..\Global\GlobalFunc.h"
#include "..\Global\GlobalConst.h"

// External Libraries
#pragma comment(lib,"ws2_32.lib")
