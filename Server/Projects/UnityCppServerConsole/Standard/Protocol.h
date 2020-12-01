#pragma once
#include "..\Global\GlobalVariable.h"

#pragma pack( push, 1 )
struct UPACKET
{
	UPACKET() : length( 0 ), type( 0 ), data{} // �迭 ������ �ʱ�ȭ
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
	PACKET() : packet{}, socket{}
	{

	}

	UPACKET packet;
	SOCKET socket;
};

namespace Protocol
{
	namespace Type
	{
		// ����/Ŭ�� ��� �����ؾ���. (Sdbm Hash)
		u_short GetPacketType( const char* _name );

		const static u_short ChatMessage = GetPacketType( "ChatMessage" );
	}
}