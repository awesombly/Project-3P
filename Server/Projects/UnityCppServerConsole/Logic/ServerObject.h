#pragma once
#include "../Standard/Header.h"
#include "Vector.h"
#include "Quaternion.h"
#include <cereal/cereal.hpp>

struct ServerObject
{
public:
	u_int Serial;
	Vector3 Position;
	Quaternion Rotation;

public:
	ServerObject() = default;
	ServerObject( u_int _serial );
	ServerObject( u_int _serial, Vector3 _position, Quaternion _rotation );

public:
	template <class Archive>
	void serialize( Archive& ar )
	{
		ar( CEREAL_NVP( Serial ) );
		ar( CEREAL_NVP( Position ) );
		ar( CEREAL_NVP( Rotation ) );
	}
};
