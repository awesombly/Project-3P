#include "ServerActor.h"

ServerActor::ServerActor( SerialType _serial )
	: Serial( _serial )
{
}

ServerActor::ServerActor( SerialType _serial, Vector3 _position, Quaternion _rotation )
	: Serial( _serial ), Position( _position ), Rotation( _rotation )
{
}

ServerNpc::ServerNpc( const ServerActor& _actor, const std::string& _npcId )
	: ServerActor( _actor ), NpcId( _npcId )
{
}