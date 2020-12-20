#include "Database.h"
#include "../Standard/Log.h"

Database::~Database()
{
	::mysql_close( connection );

	SafeDelete( data );
	::mysql_free_result( data );
}

const bool Database::Initialize()
{
	if ( ::mysql_init( &initConnection ) == nullptr )
	{
		LOG_WARNING << "Database Initialize Failed : "_s << ::mysql_error( connection );
	}

	connection = ::mysql_real_connect( &initConnection, DB::CONFIG::Host, DB::CONFIG::User,
									   DB::CONFIG::PW, DB::CONFIG::DBName, DB::CONFIG::Port, ( const char* )NULL, 0 );
	if ( connection == nullptr )
	{
		LOG_WARNING << "Database Connect Failed : "_s << ::mysql_error( connection );
		return false;
	}

	// DB Schema Select
	if ( ::mysql_select_db( connection, DB::CONFIG::DBName ) != NULL )
	{
		LOG_WARNING << "Failed To Select "_s << DB::CONFIG::DBName << " Database : "_s << ::mysql_error( connection );
		return false;
	}

	return true;
}

const bool Database::SafeQuery( const std::string& _query )
{
	if ( connection == nullptr )
	{
		LOG_WARNING << "Database Connect Failed"_s << ::mysql_error( connection );
		return false;
	}

	// Query
	if ( ::mysql_query( connection, ( _query.c_str() ) ) == FALSE )
	{
		LOG_WARNING << "Query Failed"_s << ::mysql_error( connection );
		return false;
	}

	data = ::mysql_store_result( connection );
	if ( data == nullptr )
	{
		LOG_WARNING << "Failed To Fetch Data Through Query"_s << ::mysql_error( connection );
		return false;
	}

	return true;
}

const bool Database::CompareID( const std::string& _id )
{
	if ( !SafeQuery( "select* from user where id = "_s + ToSQLString( _id ) ) )
	{
		LOG_WARNING << "Failed To Query"_s << ::mysql_error( connection );
		return false;
	}

	MYSQL_ROW row = ::mysql_fetch_row( data );
	if ( row == nullptr )
	{
		LOG_WARNING << "Not Find Data"_s << ::mysql_error( connection );
		return false;
	}

	return _id.compare( row[ EDBIndexType::ID ] ) == 0;
}

const bool Database::ComparePW( const std::string& _pw )
{
	if ( !SafeQuery( "select* from user where pw = "_s + ToSQLString( _pw ) ) )
	{
		LOG_WARNING << "Failed To Query"_s << ::mysql_error( connection );
		return false;
	}

	MYSQL_ROW row = ::mysql_fetch_row( data );
	if ( row == nullptr )
	{
		LOG_WARNING << "Not Find Data"_s << ::mysql_error( connection );
		return false;
	}

	return _pw.compare( row[ EDBIndexType::PW ] ) == 0;
}

const std::string Database::ToSQLString( const std::string& _data )
{
	return "\'" + _data + "\'";
}