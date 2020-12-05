#pragma once
#include <cereal/cereal.hpp>
#include "Vector.h"
#include "Quaternion.h"

class GameObject
{
public:
	Vector3 position;
	Quaternion rotation;

public:
	GameObject() = default;
	GameObject( Vector3 _position, Quaternion _rotation );

public:
	template <class Archive>
	void serialize( Archive& ar )
	{
		ar( CEREAL_NVP( position ) );
		ar( CEREAL_NVP( rotation ) );
	}
};
