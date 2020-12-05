#include "Quaternion.h"

const Quaternion Quaternion::Identity = { 0.0f, 0.0f, 0.0f, 1.0f };

Quaternion::Quaternion()
	: x( 0.0f ), y( 0.0f ), z( 0.0f ), w( 1.0f )
{
}

Quaternion::Quaternion( float _x, float _y, float _z, float _w )
	: x( _x ), y( _y ), z( _z ), w( _w )
{
}