#include "ServerObject.h"

ServerObject::ServerObject( u_int _serial )
	: Serial( _serial )
{
}

ServerObject::ServerObject( u_int _serial, Vector3 _position, Quaternion _rotation )
	: Serial( _serial ), Position( _position ), Rotation( _rotation )
{
}
