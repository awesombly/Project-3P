#include "ServerActor.h"

ServerActor::ServerActor( SerialType _serial )
	: Serial( _serial )
{
}

ServerActor::ServerActor( SerialType _serial, Vector3 _position, Quaternion _rotation )
	: Serial( _serial ), Position( _position ), Rotation( _rotation )
{
}

ServerNpc::ServerNpc( const std::string& _npcId, const Vector3& _target, const Vector3& _curPosition )
	: NpcId( _npcId ), Target( _target ), CurPosition( _curPosition )
{
}