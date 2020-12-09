#include "ServerObject.h"

ServerObject::ServerObject( SerialType _serial )
	: Serial( _serial )
{
}

ServerObject::ServerObject( SerialType _serial, Vector3 _position, Quaternion _rotation )
	: Serial( _serial ), Position( _position ), Rotation( _rotation )
{
}
