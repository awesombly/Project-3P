#pragma once
// Warning Ignore
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning( disable : 4244 )

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

// External Libraries
#pragma comment(lib,"ws2_32.lib")

const static u_short HeaderSize = 4;
const static u_short WorkerThreadCount = 3;
const static u_short OneLineOfText = 256;
const static u_short MsgMaxSize = 2048;
const static u_short RecvBufferMaxSize = 10000;