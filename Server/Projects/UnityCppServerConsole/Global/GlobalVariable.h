#pragma once
#include <Winsock2.h>
#include <string>

// Global Const
static const u_short HeaderSize = 4;
static const u_short WorkerThreadCount = 6;
static const u_short OneLineOfText = 256;
static const u_short DataMaxSize = 2048;
static const u_short RecvBufferMaxSize = 10000;

namespace PATH
{
	// Path 변경시 빌드 후 이벤트 폴더 생성도 같이 변경하셔야 합니다.
<<<<<<< Updated upstream
	static const std::string LogPath = "../../../Log/";
=======
	const static std::string LogPath = "../../../Log/";
>>>>>>> Stashed changes
}

namespace EXT
{
	static const std::string Text = ".txt";
	static const std::string Exe = ".exe";
	static const std::string Ini = ".ini";
}