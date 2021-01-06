#include "Log.h"
#include "../Time/Timer.h"

Log::Log() : logStream( std::cout ), curLogPos( 0 )
{
	std::thread th( [&] () { Log::LogProcess(); } );
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

void Log::WSAError()
{
	WSAErrorToString( ::WSAGetLastError() );
}

Log& Log::operator << ( ELogType _type )
{
	size_t paramSize = types[ _type ].size();
	std::copy( std::begin( types[ _type ] ), std::end( types[ _type ] ), &logData[ curLogPos ] );
	curLogPos += paramSize;

	if ( _type == ELogType::EndLine )
	{
		size_t writePos = 0;
		if ( file.IsOpen() && logData[ 0 ] == '#' )
		{
			file.Write( std::move( logData.substr( 0, curLogPos ) ) );
			writePos = logData.find_first_of( '#', 1 ) + ( size_t )1;
			if ( writePos == std::string::npos )
			{
				curLogPos = 0;
				return *this;
			}
		}

		{
			std::lock_guard<std::mutex> lock( textsMutex );
			texts.emplace( logData.substr( writePos, curLogPos - writePos ) );
		}
		curLogPos = 0;
		cv.notify_one();
	}

	return *this;
}

Log& Log::operator << ( const std::string& _data )
{
	size_t paramSize = _data.size();
	std::copy( std::begin( _data ), std::end( _data ), &logData[ curLogPos ] );
	curLogPos += paramSize;

	return *this;
}

Log& Log::operator << ( const char* _data )
{
	size_t paramSize = ::strlen( _data );
	std::copy( &_data[ 0 ], &_data[ paramSize ], &logData[ curLogPos ] );
	curLogPos += paramSize;

	return *this;
}

Log& Log::operator << ( int _data )
{
	std::string data = std::to_string( _data );
	size_t paramSize = data.size();
	std::copy( &data[ 0 ], &data[ paramSize ], &logData[ curLogPos ] );
	curLogPos += paramSize;

	return *this;
}

Log& Log::operator << ( unsigned __int64 _data )
{
	std::string data = std::to_string( _data );
	size_t paramSize = data.size();
	std::copy( &data[ 0 ], &data[ paramSize ], &logData[ curLogPos ] );
	curLogPos += paramSize;

	return *this;
}

std::string Log::ToString( float _value, int _maxDecimalPoint )
{
	std::ostringstream out;
	out << std::setprecision( _maxDecimalPoint ) << _value;

	return out.str();
}

Log& Log::operator << ( const Vector3& _data )
{
	std::string data;
	data.reserve( 128 );
	data.append( "( " );
	data.append( ToString( _data.x ) ).append( ", " );
	data.append( ToString( _data.y ) ).append( ", " );
	data.append( ToString( _data.z ) ).append( " )" );

	size_t paramSize = data.size();
	std::copy( &data[ 0 ], &data[ paramSize ], &logData[ curLogPos ] );
	curLogPos += paramSize;

	return *this;
}

Log& Log::operator << ( const Quaternion& _data )
{
	std::string data;
	data.reserve( 128 );
	data.append( "( " );
	data.append( ToString( _data.x ) ).append( ", " );
	data.append( ToString( _data.y ) ).append( ", " );
	data.append( ToString( _data.z ) ).append( ", " );
	data.append( ToString( _data.w ) ).append( " )" );

	size_t paramSize = data.size();
	std::copy( &data[ 0 ], &data[ paramSize ], &logData[ curLogPos ] );
	curLogPos += paramSize;

	return *this;
}

const std::string& Log::GetType( ELogType _type )
{
	return Log::Instance().types[_type];
}

void Log::WSAErrorToString( const int _errorCode )
{
	switch ( _errorCode )
	{
		case WSANOTINITIALISED:
		{
			PushLog( ELogType::Error, "이 기능을 사용하기전에 WSAStartup을 호출해야 합니다."_s );
		} break;
		case WSAECONNRESET:
		{
			PushLog( ELogType::Error, "들어오는 연결이 수락하기전에 원격에 의해 종료되었습니다."_s );
		} break;
		case WSAEFAULT:
		{
			PushLog( ELogType::Error, "추가 매개변수가 적거나 사용자의 메모리 공간이 유효하지 않습니다."_s );
		} break;
		case WSAEINTR:
		{
			PushLog( ELogType::Error, "윈도우소켓 1.1은 WSACancelBlockingCall에 의해 취소되었습니다."_s );
		} break;
		case WSAEINVAL:
		{
			PushLog( ELogType::Error, "Listen함수는 수락되기 전에 호출되지 않았습니다."_s );
		} break;
		case WSAEINPROGRESS:
		{
			PushLog( ELogType::Error, "윈도우소켓 1.1 호출이 진행 중이거나 서비스 공급자가 콜백 기능을 처리하는 중입니다."_s );
		} break;
		case WSAEMFILE:
		{
			PushLog( ELogType::Error, "큐가 비어있지 않으며 사용할수있는 Descriptor가 없습니다."_s );
		} break;
		case WSAENETDOWN:
		{
			PushLog( ELogType::Error, "네트워크 하위 시스템이 실패했습니다."_s );
		} break;
		case WSAENOBUFS:
		{
			PushLog( ELogType::Error, "버퍼 공간을 사용할 수 없습니다."_s );
		} break;
		case WSAENOTSOCK:
		{
			PushLog( ELogType::Error, "Descriptor는 소켓이 아닙니다."_s );
		} break;
		case WSAEOPNOTSUPP:
		{
			PushLog( ELogType::Error, "참조된 소켓은 연결 서비스를 지원하는 형식이 아닙니다."_s );
		} break;
		case WSAEWOULDBLOCK:
		{
			PushLog( ELogType::Error, "소켓은 Non-Blocking이 아니며 연결이 허용되지 않습니다."_s );
		} break;
	}
}

void Log::PushLog( ELogType _type, const std::string& _data )
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

void Log::LogProcess()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( textsMutex );
		cv.wait( lock, [&] () { return !texts.empty(); } );

		std::string& data = texts.front();
		logStream << data.c_str();

		texts.pop();
		::Sleep( 1 );
	}
}