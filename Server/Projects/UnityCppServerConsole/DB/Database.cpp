#include "Database.h"
#include "../Standard/Log.h"

Database::~Database()
{
	::mysql_close( connection );

	SafeDelete( data );
	::mysql_free_result( data );
}

bool Database::Initialize()
{
	if ( ::mysql_init( &initConnection ) == nullptr )
	{
		LOG_WARNING << "Database Initialize Failed : "_s << ::mysql_error( connection ) << LOG_END;
	}

	connection = ::mysql_real_connect( &initConnection, DB::CONFIG::Host, DB::CONFIG::User,
									   DB::CONFIG::PW, DB::CONFIG::DBName, DB::CONFIG::Port, ( const char* )NULL, 0 );
	if ( connection == nullptr )
	{
		LOG_WARNING << "Database Connect Failed : "_s << ::mysql_error( connection ) << LOG_END;
		return false;
	}

	// DB Schema Select
	if ( ::mysql_select_db( connection, DB::CONFIG::DBName ) != NULL )
	{
		LOG_WARNING << "Failed To Select "_s << DB::CONFIG::DBName << " Database : "_s << ::mysql_error( connection ) << LOG_END;
		return false;
	}

	return true;
}

bool Database::SafeQuery( const std::string& _query )
{
	if ( connection == nullptr )
	{
		LOG_WARNING << "Database Connect Failed"_s << ::mysql_error( connection ) << LOG_END;
		return false;
	}
	
	// Query
	if ( ::mysql_query( connection, _query.c_str() ) )
	{
		LOG_WARNING << "Query Failed : "_s << ::mysql_error( connection ) << _query << LOG_END;
		return false;
	}

	data = ::mysql_store_result( connection );
	if ( data == nullptr )
	{
		LOG_WARNING << "Failed To Fetch Data Through Query"_s << ::mysql_error( connection ) << LOG_END;
		return false;
	}

	return true;
}

Infomation Database::GetUserInfomation( int _uid )
{
	Infomation info;
	if ( !SafeQuery( "Call GetUserInfomation("_s + std::to_string( _uid ) + ")"_s ) )
	{
		return info;
	}
	MYSQL_ROW row = ::mysql_fetch_row( data );
	info.uid = std::atoi( row[ 0 ] );
	info.name = row[ 1 ];
	info.id = row[ 2 ];
	info.pw = row[ 3 ];
	info.startDate = row[ 4 ];
	info.lastConnectDate = row[ 5 ];

	return info;
}