#pragma once
#include "..\Global\GlobalVariable.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
// 각 타입 serialize시 필요
#include <cereal/types/array.hpp>
#include <cereal/types/atomic.hpp>
#include <cereal/types/valarray.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/polymorphic.hpp>

namespace Protocol
{
	interface IProtocol;
}

#pragma pack( push, 1 )
struct UPACKET
{
	UPACKET() : length( 0 ), type( 0 ), data{} // 배열 유니폼 초기화
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

		// 유니티 Json이랑 형식이 달라서 읽어지지 않는다.. 라이브러리를 수정하던가 해야할듯
		// 앞, 뒤 잉여 문자들 제거
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

namespace Protocol
{
	namespace Type
	{
		// 서버/클라 결과 동일해야함. (Sdbm Hash)
		u_short GetPacketType( const char* _name );

		// 별도로 구조체 생성없이 타입 체크만 필요한 것들
		const static u_short ChatMessage = GetPacketType( "ChatMessage" );
	}

	interface IProtocol
	{
		// serialize()를 정의해둬야 cereal 라이브러리에서 파싱이 가능해짐
		// template<class Archive>
		// void serialize( Archive& ar )
	};
}
