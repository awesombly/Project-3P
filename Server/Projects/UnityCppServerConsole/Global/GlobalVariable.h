#pragma once
#include <windows.h>
#include <mutex>

// Global

// Global Const
// namespace�� ���� �̸��� ������ �ȳ�.
const static u_short HeaderSize = 4;
const static u_short WorkerThreadCount = 8;
const static u_short OneLineOfText = 256;
const static u_short DataMaxSize = 2048;
const static u_short RecvBufferMaxSize = 10000;