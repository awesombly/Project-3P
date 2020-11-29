#pragma once
#include "..\Global\GlobalVariable.h"

namespace PACKET_TYPE
{
	// 서버/클라 결과 동일해야함. (Sdbm Hash)
	u_short GetPacketType( const char* _name );

	const static u_short ChatMessage( GetPacketType( "ChatMessage" ) );
}

#pragma pack( push, 1 )
struct UPACKET
{
	UPACKET() : length( 0 ), type( 0 ), data{} // 배열 유니폼 초기화
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