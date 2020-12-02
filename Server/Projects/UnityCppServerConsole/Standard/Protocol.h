﻿#pragma once
#include "..\Global\GlobalVariable.h"
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

	struct TestProtocol : public IProtocol
	{
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

		// TODO : Define으로 변경
		const static std::string Name;
		const static u_short Type;
	};
}
