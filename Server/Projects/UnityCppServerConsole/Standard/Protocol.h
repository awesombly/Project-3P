#pragma once
#include "Header.h"
#include "../Logic/GameObject.h"
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

#define PROTOCOL_HEADER() const static std::string PacketName; const static u_short PacketType;
#define PROTOCOL_BODY( _namespace, _name ) const std::string _namespace::_name::PacketName = #_name; const u_short _namespace::_name::PacketType = GetPacketType( _name::PacketName.c_str() );
// 데이터 처리 없이, 타입 체크만 사용할 때
#define SIMPLE_PROTOCOL( _name ) namespace _name { const std::string PacketName = #_name; const u_short PacketType = GetPacketType( PacketName.c_str() ); }


namespace Protocol
{
	// 서버/클라 결과 동일해야함. (Sdbm Hash)
	u_short GetPacketType( const char* _name );

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
		SIMPLE_PROTOCOL( SyncTransform );

		struct TestProtocol : public IProtocol
		{
			PROTOCOL_HEADER();

			int Level;
			std::string Id;
			//std::map< int/*SlotIndex*/, std::string/*EquipId*/ > Equipments;

			struct Item
			{
				int Count;
				std::string Id;

				Item() = default;
				Item( const std::string& _id, int _count )
					: Id( _id )
					, Count( _count )
				{ }

				template <class Archive>
				void serialize( Archive& ar )
				{
					ar( CEREAL_NVP( Count ) );
					ar( CEREAL_NVP( Id ) );
				}
			};
			std::vector< Item > ItemList;

			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( Level ) );
				ar( CEREAL_NVP( Id ) );
				//ar( CEREAL_NVP( Equipments ) );
				ar( CEREAL_NVP( ItemList ) );
			}
		};
	}

	namespace ToServer
	{
		SIMPLE_PROTOCOL( EnterStage );
	}

	namespace FromServer
	{
		struct CreatePlayer : public IProtocol
		{
			PROTOCOL_HEADER();

			std::string Name;
			Vector3 Position;
			Vector3 Direction;
			bool IsLocal;
			
			template <class Archive>
			void serialize( Archive& ar )
			{
				ar( CEREAL_NVP( Name ) );
				ar( CEREAL_NVP( Position ) );
				ar( CEREAL_NVP( Direction ) );
				ar( CEREAL_NVP( IsLocal ) );
			}
		};
	}
}
