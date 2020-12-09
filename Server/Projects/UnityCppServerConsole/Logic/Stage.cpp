#include "Stage.h"


void Stage::BroadCast( const UPACKET& _packet ) const
{
	SessionManager::Instance().BroadCast( _packet, sessions );
}

void Stage::BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const
{
	SessionManager::Instance().BroadCastExceptSelf( _packet, _session, sessions );
}
