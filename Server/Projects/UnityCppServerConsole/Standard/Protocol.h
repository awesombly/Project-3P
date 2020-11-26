#pragma once
#include <windows.h>

namespace PROTOCOL
{
	const static u_short HeaderSize = 4;
	const static u_short OneLineOfText = 256;
	const static u_short MsgMaxSize = 2048;
	const static u_short RecvMaxSize = 10000;
}

namespace PACKET_TYPE
{
	const static u_short ChatMessage = 1000;
}

#pragma pack( push, 1 )
struct UPACKET
{
	UPACKET() : length( 0 ), type( 0 ), data{} // �迭 ������ �ʱ�ȭ
	{
		
	}

	u_short length;
	u_short type;
	u_char data[PROTOCOL::MsgMaxSize];
};
#pragma pack( pop )

struct PACKET
{
	PACKET() : packet{}, socket{}
	{

	}

	UPACKET packet;
	SOCKET socket;
};