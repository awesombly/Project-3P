#pragma once
#include "Header.h"
#include "../Logic/ServerActor.h"
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

using PacketType = u_short;

#define PROTOCOL_HEADER() static const std::string PacketName; static const PacketType PacketType;
#define PROTOCOL_BODY( _namespace, _name ) const std::string _namespace::_name::PacketName = #_name; const PacketType _namespace::_name::PacketType = GetPacketType( _name::PacketName.c_str() );
// 데이터 처리 없이, 타입 체크만 사용할 때
#define SIMPLE_PROTOCOL( _name ) namespace _name { const std::string PacketName = #_name; const PacketType PacketType = GetPacketType( PacketName.c_str() ); }


namespace Protocol
{
	SerialType GetNewSerial();

	// 서버/클라 결과 동일해야함. (Sdbm Hash)
	PacketType GetPacketType( const char* _name );

	interface IProtocol
	{
		// serialize()를 정의해둬야 cereal 라이브러리에서 파싱이 가능해짐
		// template<class Archive>
		// void serialize( Archive& ar )
	};

	// Both : 클라/서버 양쪽에서 사용
	// ToServer : 서버로 보내는 패킷
	// FromServer : 서버에서 온 패킷
	namespace Both
	{
		SIMPLE_PROTOCOL( ChatMessage );
		SIMPLE_PROTOCOL( SyncCrouch );
		SIMPLE_PROTOCOL( SyncGrounded );

		struct SyncTransform : public IProtocol
		{
			PROTOCOL_HEADER();

			ServerActor Actor;

			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( Actor ) );
			}
		};

		struct SyncInterpolation : public IProtocol
		{
			PROTOCOL_HEADER();

			ServerActor Actor;
			Vector3 Velocity;

			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( Actor ) );
				ar( CEREAL_NVP( Velocity ) );
			}
		};

		struct SyncNpcState : public IProtocol
		{
			PROTOCOL_HEADER();

			ServerNpc NpcInfo;

			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( NpcInfo ) );
			}
		};
	}

	namespace ToServer
	{
		struct EnterStage : public IProtocol
		{
			PROTOCOL_HEADER();

			std::string StageId;
			Vector3 SpawnPosition;

			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( StageId ) );
				ar( CEREAL_NVP( SpawnPosition ) );
			}
		};

		struct RequestNpcInfo : public IProtocol
		{
			PROTOCOL_HEADER();

			ServerNpc NpcInfo;

			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( NpcInfo ) );
			}
		};
	}

	namespace FromServer
	{
		struct Connected : public IProtocol
		{
			PROTOCOL_HEADER();

			template <class Archive>
			void serialize( Archive& /*ar*/ )
			{
			}
		};

		struct CreatePlayer : public IProtocol
		{
			PROTOCOL_HEADER();

			ServerActor Player;
			bool IsLocal;
			
			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( Player ) );
				ar( CEREAL_NVP( IsLocal ) );
			}
		};

		struct ResponseNpcInfo : public IProtocol
		{
			PROTOCOL_HEADER();

			ServerNpc NpcInfo;

			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( NpcInfo ) );
			}
		};

		struct DestroyActor : public IProtocol
		{
			PROTOCOL_HEADER();

			SerialType Serial;

			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( Serial ) );
			}
		};
	}
}
