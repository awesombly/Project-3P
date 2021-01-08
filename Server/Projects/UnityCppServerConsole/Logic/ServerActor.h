#pragma once
#include "../Standard/Header.h"
#include "../Math/Vector.h"
#include "../Math/Quaternion.h"
#include <cereal/cereal.hpp>

using SerialType = u_int;

struct ServerActor
{
public:
	SerialType Serial;
	Vector3 Position;
	Quaternion Rotation;

public:
	ServerActor() = default;
	ServerActor( SerialType _serial );
	ServerActor( SerialType _serial, Vector3 _position, Quaternion _rotation );

public:
	template <class Archive>
	void serialize( Archive& ar )
	{
		ar( CEREAL_NVP( Serial ) );
		ar( CEREAL_NVP( Position ) );
		ar( CEREAL_NVP( Rotation ) );
	}
};

struct ServerNpc
{
public:
	bool IsLocal;
	SerialType Serial;
	int State;
	std::string NpcId;
	Vector3 Target;
	Vector3 CurPosition;

public:
	ServerNpc() = default;
	ServerNpc( const std::string& _npcId, const Vector3& _target, const Vector3& _curPosition );

public:
	template <class Archive>
	void serialize( Archive& ar )
	{
		ar( CEREAL_NVP( IsLocal ) );
		ar( CEREAL_NVP( Serial ) );
		ar( CEREAL_NVP( State ) );
		ar( CEREAL_NVP( NpcId ) );
		ar( CEREAL_NVP( Target ) );
		ar( CEREAL_NVP( CurPosition ) );
	}
};