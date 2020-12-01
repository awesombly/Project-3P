#include "Database.h"
#include "..\\Standard\Log.h"

Database::~Database()
{
	::mysql_close( connection );
	SafeDelete( connection );

	::mysql_free_result( data );
	SafeDelete( data );
}

bool Database::Initialize()
{
	if ( ::mysql_init( &initConnection ) == nullptr )
	{
		Log::Instance().Push( ELogType::Warning, "Database Initialize Failed : "_s + ::mysql_error( connection ) );
	}

	connection = ::mysql_real_connect( &initConnection, DB::CONFIG::Host, DB::CONFIG::User,
				 DB::CONFIG::PW, DB::CONFIG::DBName, DB::CONFIG::Port, ( const char* )NULL, 0 );
	if ( connection == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Database Connection Failed : "_s + ::mysql_error( connection ) );
		return false;
	}

	// DB Schema Select
	if ( ::mysql_select_db( connection, DB::CONFIG::DBName ) != NULL )
	{
		Log::Instance().Push( ELogType::Warning, "Failed To Select "_s + DB::CONFIG::DBName + " Database : "_s + ::mysql_error( connection ) );
		return false;
	}

	return true;
}

UserData Database::GetUserData( const std::string& _name )
{
	UserData result;
	if ( connection == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Database Connection Failed"_s + ::mysql_error( connection ) );
		return result;
	}

	// Query
	::mysql_query( connection, ( "select* from user where name = '"_s + _name + '\'' ).c_str() );
	data = ::mysql_store_result( connection );
	if ( data == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Failed To Fetch Data Through Query"_s + ::mysql_error( connection ) );
		return result;
	}

	// DB에서 받은 데이터는 문자열로 들어옵니다.
	// 따라서 문자열을 맞는 데이터형으로 변환해줍니다.
	MYSQL_ROW row = ::mysql_fetch_row( data );
	result.id = std::stoi( row[EDBIndexType::ID] );
	result.name = row[EDBIndexType::Name];
	result.startDate = row[EDBIndexType::StartDate];
	result.lastConnectDate = row[EDBIndexType::LastConnect];

	return result;
}