#pragma once
#include <Winsock2.h>
#include <string>

// Global Const
static const u_short HeaderSize = 4;
static const u_short WorkerThreadCount = 6;
static const u_short OneLineOfText = 256;
static const u_short DataMaxSize = 2048;
static const u_short RecvBufferMaxSize = 10000;
static const u_short LogDataMaxSize = 1024;

namespace DB
{
	namespace CONFIG
	{
		const static char* Host = "localhost";
		const static char* User = "root";
		const static char* PW = "wnsrudtj2629";
		const static char* DBName = "project3p";
		const static unsigned int Port = 3306;
	}

	namespace ColName
	{
		const static char* Key = "key";
		const static char* NickName = "nickname";
		const static char* ID = "id";
		const static char* PW = "pw";
	}
}

namespace PATH
{
	// Path 변경시 빌드 후 이벤트 폴더 생성도 같이 변경하셔야 합니다.
	static const std::string LogPath = "../../../Log/";
}

namespace EXT
{
	static const std::string Text = ".txt";
	static const std::string Exe  = ".exe";
	static const std::string Ini  = ".ini";
}