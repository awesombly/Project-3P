#pragma once
#include "../Standard/Header.h"
#include "Vector.h"
#include "Quaternion.h"
#include <cereal/cereal.hpp>

using SerialType = u_int;

struct ServerObject
{
public:
	SerialType Serial;
	Vector3 Position;
	Quaternion Rotation;

public:
	ServerObject() = default;
	ServerObject( SerialType _serial );
	ServerObject( SerialType _serial, Vector3 _position, Quaternion _rotation );

public:
	template <class Archive>
	void serialize( Archive& ar )
	{
		ar( CEREAL_NVP( Serial ) );
		ar( CEREAL_NVP( Position ) );
		ar( CEREAL_NVP( Rotation ) );
	}
};
