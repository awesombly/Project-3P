#pragma once
#include "../Standard/Protocol.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>


#pragma pack( push, 1 )
struct UPACKET
{
	UPACKET() : length( 0 ), type( 0 ), data{}
	{
	}

	template <class Type>
	UPACKET( const Type& _protocol ) : UPACKET()
	{
		SetData( _protocol );
	}

	u_short length;
	u_short type;
	byte data[ DataMaxSize ];

	template <class Type>
	void SetData( const Type& _protocol, bool useEncoding = false )
	{
		std::ostringstream stream;
		{
			cereal::JSONOutputArchive archive( stream, cereal::JSONOutputArchive::Options::NoIndent() );
			archive( cereal::make_nvp( _protocol.PacketName, _protocol ) );
		}

		std::string jsonData = std::move( stream.str() );

		// 유니티 Json이랑 형식이 달라서 읽어지지 않는다.. 라이브러리를 수정하던가 해야할듯
		// 앞, 뒤 잉여 문자들 제거
		const size_t startPosition = jsonData.find( ':' ) + 1;
		const size_t endPosition = jsonData.rfind( '}' ) - startPosition;
		if ( useEncoding )
		{
			// 한글 입력시 유니티에서 깨져서 보임
			jsonData = ToUFT8( jsonData.substr( startPosition, endPosition ).c_str() );
		}
		else
		{
			jsonData = jsonData.substr( startPosition, endPosition );
		}

		type = _protocol.PacketType;
		length = ( u_short )jsonData.length();
		memcpy_s( ( char* )data, DataMaxSize, jsonData.c_str(), length );
		length += HeaderSize;
	}

	template <class Type>
	Type GetParsedData() const
	{
		// 읽을 수 있도록 잉여 문자들 추가
		static const std::string prefix = "{\"pt\": ";
		static const std::string suffix = "}";
		const std::string& jsonData = prefix + ToString() + suffix;

		std::istringstream stream( jsonData );
		cereal::JSONInputArchive archive( stream );

		Type protocol;
		archive( protocol );

		return protocol;
	}

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
