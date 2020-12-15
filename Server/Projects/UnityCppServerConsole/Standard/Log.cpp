#include "Log.h"
#include "../Time/Timer.h"

Log::Log() : logStream( std::cout ), curLogPos( 0 )
{
	std::thread th( [&] () { Log::PrintText(); } );
	th.detach();

	types.insert( std::make_pair( ELogType::Log, std::string( "[Log]" ) ) );
	types.insert( std::make_pair( ELogType::Warning, std::string( "[Warning]" ) ) );
	types.insert( std::make_pair( ELogType::Error, std::string( "[Error]" ) ) );
	types.insert( std::make_pair( ELogType::Exception, std::string( "[Exception]" ) ) );
	types.insert( std::make_pair( ELogType::EndLine, std::string( "\n" ) ) );

	logData.resize( LogDataMaxSize );
}

bool Log::Initialize()
{
	return file.Open( PATH::LogPath + Timer::Instance().GetCurrentDateString( true ).c_str() + EXT::Text );
}

void Log::Push()
{
	Push( ::WSAGetLastError() );
}

void Log::Push( const int _errorCode )
{
	switch ( _errorCode )
	{
		case WSANOTINITIALISED:
		{
			Push( ELogType::Error, "이 기능을 사용하기전에 WSAStartup을 호출해야 합니다."_s );
		} break;
		case WSAECONNRESET:
		{
			Push( ELogType::Error, "들어오는 연결이 수락하기전에 원격에 의해 종료되었습니다."_s );
		} break;
		case WSAEFAULT:
		{
			Push( ELogType::Error, "추가 매개변수가 적거나 사용자의 메모리 공간이 유효하지 않습니다."_s );
		} break;
		case WSAEINTR:
		{
			Push( ELogType::Error, "윈도우소켓 1.1은 WSACancelBlockingCall에 의해 취소되었습니다."_s );
		} break;
		case WSAEINVAL:
		{
			Push( ELogType::Error, "Listen함수는 수락되기 전에 호출되지 않았습니다."_s );
		} break;
		case WSAEINPROGRESS:
		{
			Push( ELogType::Error, "윈도우소켓 1.1 호출이 진행 중이거나 서비스 공급자가 콜백 기능을 처리하는 중입니다."_s );
		} break;
		case WSAEMFILE:
		{
			Push( ELogType::Error, "큐가 비어있지 않으며 사용할수있는 Descriptor가 없습니다."_s );
		} break;
		case WSAENETDOWN:
		{
			Push( ELogType::Error, "네트워크 하위 시스템이 실패했습니다."_s );
		} break;
		case WSAENOBUFS:
		{
			Push( ELogType::Error, "버퍼 공간을 사용할 수 없습니다."_s );
		} break;
		case WSAENOTSOCK:
		{
			Push( ELogType::Error, "Descriptor는 소켓이 아닙니다."_s );
		} break;
		case WSAEOPNOTSUPP:
		{
			Push( ELogType::Error, "참조된 소켓은 연결 서비스를 지원하는 형식이 아닙니다."_s );
		} break;
		case WSAEWOULDBLOCK:
		{
			Push( ELogType::Error, "소켓은 Non-Blocking이 아니며 연결이 허용되지 않습니다."_s );
		} break;
	}
}

void Log::Push( ELogType _type, const std::string& _data )
{
	size_t paramSize = types[ _type ].size() + _data.size() + 1;
	if ( LogDataMaxSize - curLogPos < paramSize )
	{
		// 사이즈만큼 넣고 0으로만든다음에 다시 출력
	}

	size_t typeSize = types[ _type ].size();
	std::copy( std::begin( types[ _type ] ), std::end( types[ _type ] ), &logData[ curLogPos ] );
	curLogPos += typeSize;

	size_t dataSize = _data.size();
	std::copy( std::begin( _data ), std::end( _data ), &logData[ curLogPos ] );
	curLogPos += dataSize;

	logData[ curLogPos ] = '\n';
	++curLogPos;

	{
		std::lock_guard<std::mutex> lock( textsMutex );
		texts.emplace( logData.substr( 0, curLogPos ) );

		curLogPos = 0;
		cv.notify_one();
	}
}

Log& Log::operator << ( ELogType _type )
{
	size_t paramSize = types[ _type ].size();
	if ( LogDataMaxSize - curLogPos < paramSize )
	{
		// 사이즈만큼 넣고 0으로만든다음에 다시 출력
	}

	std::copy( std::begin( types[ _type ] ), std::end( types[ _type ] ), &logData[ curLogPos ] );
	curLogPos += paramSize;

	if ( _type == ELogType::EndLine )
	{
		std::lock_guard<std::mutex> lock( textsMutex );
		texts.emplace( logData.substr( 0, curLogPos ) );

		curLogPos = 0;
		cv.notify_one();
	}

	return *this;
}

Log& Log::operator << ( const std::string& _data )
{
	size_t paramSize = _data.size();
	if ( LogDataMaxSize - curLogPos < paramSize )
	{
		// 사이즈만큼 넣고 0으로만든다음에 다시 출력
	}

	std::copy( std::begin( _data ), std::end( _data ), &logData[ curLogPos ] );
	curLogPos += paramSize;

	return *this;
}

Log& Log::operator << ( const char* _data )
{
	size_t paramSize = ::strlen( _data );
	if ( LogDataMaxSize - curLogPos < paramSize )
	{
		// 사이즈만큼 넣고 0으로만든다음에 다시 출력
	}

	std::copy( &_data[ 0 ], &_data[ paramSize ], &logData[ curLogPos ] );
	curLogPos += paramSize;

	return *this;
}

const std::string& Log::GetType( ELogType _type )
{
	return Log::Instance().types[_type];
}

void Log::PrintText()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( textsMutex );
		cv.wait( lock, [&] () { return !texts.empty(); } );

		std::string& data = texts.front();
		logStream << data.c_str();
		if ( file.IsOpen() )
		{
			file.Write( data );
		}

		texts.pop();
		::Sleep( 1 );
	}
}