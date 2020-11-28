#pragma once
#include "..\Global\GlobalVariable.h"

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
	byte data[DataMaxSize];
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