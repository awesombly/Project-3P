#pragma once
#include "../Standard/Protocol.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>


#pragma pack( push, 1 )
struct UPACKET
{
	UPACKET() : length( 0 ), type( 0 ), data{} // �迭 ������ �ʱ�ȭ
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
	void SetData( const Type& _protocol )
	{
		std::ostringstream stream;
		{
			cereal::JSONOutputArchive archive( stream, cereal::JSONOutputArchive::Options::NoIndent() );
			archive( cereal::make_nvp( _protocol.Name, _protocol ) );
		}

		std::string jsonData = std::move( stream.str() );

		// ����Ƽ Json�̶� ������ �޶� �о����� �ʴ´�.. ���̺귯���� �����ϴ��� �ؾ��ҵ�
		// ��, �� �׿� ���ڵ� ����
		const size_t startPosition = jsonData.find( ':' ) + 1;
		const size_t endPosition = jsonData.rfind( '}' ) - startPosition;
		jsonData = jsonData.substr( startPosition, endPosition );

		type = _protocol.Type;
		length = jsonData.length();
		memcpy_s( ( char* )data, DataMaxSize, jsonData.c_str(), length );
		length += HeaderSize;
	}

	template <class Type>
	Type GetParsedData() const
	{
		// ���� �� �ֵ��� �׿� ���ڵ� �߰�
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
