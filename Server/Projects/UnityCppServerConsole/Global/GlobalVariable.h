#pragma once
#include <windows.h>
#include <string>

// Global

// Global Const
// namespace�� ���� �̸��� ������ �ȳ�.
const static u_short HeaderSize = 4;
const static u_short WorkerThreadCount = 6;
const static u_short OneLineOfText = 256;
const static u_short DataMaxSize = 2048;
const static u_short RecvBufferMaxSize = 10000;

namespace PATH
{
	// Path ����� ���� �� �̺�Ʈ ���� ������ ���� �����ϼž� �մϴ�.
	const static std::string LogPath = "..\\..\\..\\Log\\";
}

namespace EXT
{
	const static std::string Text = ".txt";
	const static std::string Exe = ".exe";
	const static std::string Ini = ".Ini";
}