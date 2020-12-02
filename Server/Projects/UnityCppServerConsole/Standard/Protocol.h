#pragma once
#include "..\Global\GlobalVariable.h"

#pragma pack( push, 1 )
struct UPACKET
{
	UPACKET() : length( 0 ), type( 0 ), data{}
	{
		
	}

	u_short length;
	u_short type;
	byte data[ DataMaxSize ];

	std::string ToString() const;
};
#pragma pack( pop )

struct PACKET
{
	PACKET() : packet{}
	{

	}

	UPACKET packet;
	SOCKET socket;
};

namespace Protocol
{
	namespace Type
	{
		// 서버/클라 결과 동일해야함. (Sdbm Hash)
		u_short GetPacketType( const char* _name );

		const static u_short ChatMessage = GetPacketType( "ChatMessage" );
		const static u_short LoginReq = GetPacketType( "LoginReq" );
		const static u_short LoginAck = GetPacketType( "LoginAck" );
	}
}