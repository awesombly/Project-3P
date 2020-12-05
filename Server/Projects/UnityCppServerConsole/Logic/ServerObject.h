#pragma once
#include <cereal/cereal.hpp>
#include "Vector.h"
#include "Quaternion.h"

struct ServerObject
{
public:
	std::string Name;
	Vector3 Position;
	Quaternion Rotation;

public:
	ServerObject() = default;
	ServerObject( Vector3 _position, Quaternion _rotation );

public:
	template <class Archive>
	void serialize( Archive& ar )
	{
		ar( CEREAL_NVP( Name ) );
		ar( CEREAL_NVP( Position ) );
		ar( CEREAL_NVP( Rotation ) );
	}
};
