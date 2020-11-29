#pragma once
// Warning Ignore
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4505 )

// Windows
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <process.h>

// Character
#include <string>
#include <tchar.h>
#include <fstream>

// Data Structure
#include <vector>
#include <list>
#include <queue>
#include <map>

// Standard
#include <iostream>
#include <cstdio>

// Thread
#include <thread>
#include <mutex>
#include <future>
#include <type_traits>

// Function
#include <condition_variable>
#include <functional>

// Time
#include <chrono>
#include <iomanip>
#include <ctime>

// Global
#include "..\Global\GlobalFunc.h"
#include "..\Global\GlobalVariable.h"

// External Library
#pragma comment(lib,"ws2_32.lib")
