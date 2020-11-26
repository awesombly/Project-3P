#pragma once
#include <windows.h>

#pragma pack( push, 1 )
struct PACKET
{
	u_short length;
	u_short type;
	u_char data[PROTOCOL::MsgMaxCount];
};
#pragma pack( pop )

namespace PROTOCOL
{
	const static u_short HeaderSize = 4;
	const static u_short MsgMaxCount = 2048;
}

namespace PACKET
{
	const static u_short ChatMessage = 1000;
}