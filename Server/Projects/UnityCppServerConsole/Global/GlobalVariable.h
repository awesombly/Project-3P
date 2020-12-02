#pragma once
#include <Winsock2.h>
#include <string>

// Global

// Global Const
// namespace로 묶을 이름이 생각이 안남.
const static u_short HeaderSize        = 4;
const static u_short WorkerThreadCount = 6;
const static u_short OneLineOfText     = 256;
const static u_short DataMaxSize       = 2048;
const static u_short RecvBufferMaxSize = 10000;

namespace DB
{
	namespace CONFIG
	{
		const static char* Host		   = "localhost";
		const static char* User		   = "root";
		const static char* PW		   = "wnsrudtj2629";
		const static char* DBName      = "project3p";
		const static unsigned int Port = 3306;
	}
}

namespace PATH
{
	// Path 변경시 빌드 후 이벤트 폴더 생성도 같이 변경하셔야 합니다.
	const static std::string LogPath = "..\\..\\..\\Log\\";
}

namespace EXT
{
	const static std::string Text = ".txt";
	const static std::string Exe  = ".exe";
	const static std::string Ini  = ".ini";
}