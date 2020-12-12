#include "Log.h"
#include "../Time/Timer.h"

Log::Log()
{
	std::thread th( [&] () { Log::PrintText(); } );
	th.detach();

	types.insert( std::make_pair( ELogType::Log, std::string( "[Log]" ) ) );
	types.insert( std::make_pair( ELogType::Warning, std::string( "[Warning]" ) ) );
	types.insert( std::make_pair( ELogType::Error, std::string( "[Error]" ) ) );
	types.insert( std::make_pair( ELogType::Exception, std::string( "[Exception]" ) ) );
	types.insert( std::make_pair( ELogType::EndLine, std::string( "\n" ) ) );
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
			Push( ELogType::Error, "�� ����� ����ϱ����� WSAStartup�� ȣ���ؾ� �մϴ�."_s );
		} break;
		case WSAECONNRESET:
		{
			Push( ELogType::Error, "������ ������ �����ϱ����� ���ݿ� ���� ����Ǿ����ϴ�."_s );
		} break;
		case WSAEFAULT:
		{
			Push( ELogType::Error, "�߰� �Ű������� ���ų� ������� �޸� ������ ��ȿ���� �ʽ��ϴ�."_s );
		} break;
		case WSAEINTR:
		{
			Push( ELogType::Error, "��������� 1.1�� WSACancelBlockingCall�� ���� ��ҵǾ����ϴ�."_s );
		} break;
		case WSAEINVAL:
		{
			Push( ELogType::Error, "Listen�Լ��� �����Ǳ� ���� ȣ����� �ʾҽ��ϴ�."_s );
		} break;
		case WSAEINPROGRESS:
		{
			Push( ELogType::Error, "��������� 1.1 ȣ���� ���� ���̰ų� ���� �����ڰ� �ݹ� ����� ó���ϴ� ���Դϴ�."_s );
		} break;
		case WSAEMFILE:
		{
			Push( ELogType::Error, "ť�� ������� ������ ����Ҽ��ִ� Descriptor�� �����ϴ�."_s );
		} break;
		case WSAENETDOWN:
		{
			Push( ELogType::Error, "��Ʈ��ũ ���� �ý����� �����߽��ϴ�."_s );
		} break;
		case WSAENOBUFS:
		{
			Push( ELogType::Error, "���� ������ ����� �� �����ϴ�."_s );
		} break;
		case WSAENOTSOCK:
		{
			Push( ELogType::Error, "Descriptor�� ������ �ƴմϴ�."_s );
		} break;
		case WSAEOPNOTSUPP:
		{
			Push( ELogType::Error, "������ ������ ���� ���񽺸� �����ϴ� ������ �ƴմϴ�."_s );
		} break;
		case WSAEWOULDBLOCK:
		{
			Push( ELogType::Error, "������ Non-Blocking�� �ƴϸ� ������ ������ �ʽ��ϴ�."_s );
		} break;
	}
}

void Log::Push( ELogType _type, const std::string& _data )
{
	std::unique_lock<std::mutex> lock( textsMutex );
	texts.push( types[_type] );
	texts.push( _data + "\n"_s );
	lock.unlock();

	cv.notify_one();
}

const Log& Log::operator << ( ELogType _type )
{
	std::unique_lock<std::mutex> lock( textsMutex );
	texts.push( types[_type] );
	lock.unlock();

	if ( _type == ELogType::EndLine )
	{
		cv.notify_one();
	}

	return *this;
}

const Log& Log::operator << ( const std::string& _data )
{
	std::unique_lock<std::mutex> lock( textsMutex );
	texts.push( _data );
	lock.unlock();

	return *this;
}

const Log& Log::operator << ( const char* _data )
{
	std::unique_lock<std::mutex> lock( textsMutex );
	texts.push( _data );
	lock.unlock();

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
		std::cout << data.c_str();
		if ( file.IsOpen() )
		{
			file.Write( data );
		}

		texts.pop();
		::Sleep( 1 );
	}
}