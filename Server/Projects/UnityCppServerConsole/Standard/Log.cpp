#include "Log.h"
#include "../Time/Timer.h"

Log::Log()
{
	std::thread th( [&] () { Log::PrintText(); } );
	th.detach();

	types.insert( std::make_pair( ELogType::Log, std::string( "[Log]" ) ) );
	types.insert( std::make_pair( ELogType::Warning, std::string( "[Warning]" ) ) );
	types.insert( std::make_pair( ELogType::Error, std::string( "[Error]" ) ) );
	types.insert( std::make_pair( ELogType::Error, std::string( "[Exception]" ) ) );
}

bool Log::Initialize()
{
	return file.Open( PATH::LogPath + Timer::Instance().GetCurrentDateString( true ).c_str() + EXT::Text );
}

void Log::PrintText()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( textsMutex );
		cv.wait( lock, [&] () { return !texts.empty(); } );

		LogData& data = texts.front();
		const std::string& date = Timer::Instance().GetCurrentDateString();
		std::cout << data.text.c_str() << std::endl;
		if ( file.IsOpen() )
		{
			file.Write( types[data.type] + date + data.text );
		}

		texts.pop();
		::Sleep( 1 );
	}
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
	texts.push( LogData( _type, _data ) );
	lock.unlock();

	cv.notify_one();
}