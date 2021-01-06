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

struct ServerNpc : public ServerActor
{
public:
	std::string NpcId;

public:
	ServerNpc() = default;
	ServerNpc( const ServerActor& _actor, const std::string& _npcId );

public:
	template <class Archive>
	void serialize( Archive& ar )
	{
		ar( CEREAL_NVP( Serial ) );
		ar( CEREAL_NVP( Position ) );
		ar( CEREAL_NVP( Rotation ) );
		ar( CEREAL_NVP( NpcId ) );
	}
};