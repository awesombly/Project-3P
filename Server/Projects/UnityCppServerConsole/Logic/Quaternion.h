#pragma once
#include <cereal/cereal.hpp>

struct Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;

public:
	Quaternion();
	Quaternion( float _x, float _y, float _z, float _w );

	static const Quaternion Zero;

public:
	template <class Archive>
	void serialize( Archive& ar )
	{
		ar( CEREAL_NVP( x ) );
		ar( CEREAL_NVP( y ) );
		ar( CEREAL_NVP( z ) );
		ar( CEREAL_NVP( w ) );
	}
};
