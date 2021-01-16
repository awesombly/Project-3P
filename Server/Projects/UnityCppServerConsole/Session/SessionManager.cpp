#include "SessionManager.h"
#include "../Standard/Log.h"
#include "../Logic/Stage.h"

SessionManager::~SessionManager()
{
	SessionContainer::iterator pair( std::begin( sessions ) );
	while ( pair != std::end( sessions ) )
	{
		SafeDelete( pair->second );
		++pair;
	}
	sessions.clear();
}

Session* SessionManager::Find( const SOCKET& _socket ) const
{
	const auto& sessionIter = sessions.find( _socket );
	if ( sessionIter == std::cend( sessions ) )
	{
		return nullptr;
	}

	return sessionIter->second;
}

SessionContainer SessionManager::GetSessions() const
{
	return sessions;
}

void SessionManager::Push( Session* _session )
{
	LOG << "Enter Session " << _session->GetAddressString() << "( " << _session->GetPortString() << " )" << ELogType::EndLine;
	
	cs.Lock();
	sessions[_session->GetSocket()] = _session;
	cs.UnLock();
}

void SessionManager::Erase( Session* _session )
{
	LOG << "Leave Session " << _session->GetAddressString() << "( " << _session->GetPortString() << " )" << ELogType::EndLine;

	SOCKET key = _session->GetSocket();
	cs.Lock();
	_session->ClosedSocket();
	SafeDelete( _session );
	sessions.erase( key );
	cs.UnLock();
}

void SessionManager::BroadCast( const UPACKET& _packet, const SessionContainer& _sessions )
{
	for ( const std::pair<SOCKET, Session*>& pair : _sessions )
	{
		Session* session = pair.second;
		session->Send( _packet );
	}
}

void SessionManager::BroadCastExceptSelf( const UPACKET& _packet, const Session* _session, const SessionContainer& _sessions )
{
	for ( const std::pair<SOCKET, Session*>& pair : _sessions )
	{
		Session* session = pair.second;
		if ( session == _session )
		{
			continue;
		}

		session->Send( _packet );
	}
}

void SessionManager::BroadCast( const UPACKET& _packet ) const
{
	BroadCast( _packet, sessions );
}

void SessionManager::BroadCastExceptSelf( const UPACKET& _packet, const Session* _session ) const
{
	BroadCastExceptSelf( _packet, _session, sessions );
}

void SessionManager::EnterStage( Session* _session, const std::string& _stageId )
{
	if ( _session == nullptr )
	{
		LOG_ERROR << "Session is null." << ELogType::EndLine;
		return;
	}

	if ( _session->logicData.CurrentStage != nullptr )
	{
		_session->logicData.CurrentStage->LeaveStage( _session, false );
	}

	Stage* stage = stages[ _stageId ];
	if ( stage == nullptr )
	{
		stage = stages[ _stageId ] = new Stage( _stageId );
	}

	stage->Push( _session );
	if ( _session->logicData.Player != nullptr )
	{
		stage->Push( _session->logicData.Player );
	}
	
	_session->logicData.CurrentStage = stage;
}
