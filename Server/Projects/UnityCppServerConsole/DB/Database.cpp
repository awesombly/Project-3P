#include "Database.h"
#include "..\\Standard\Log.h"

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
		Log::Instance().Push( ELogType::Warning, "Database Initialize Failed : "_s + ::mysql_error( connection ) );
	}

	connection = ::mysql_real_connect( &initConnection, DB::CONFIG::Host, DB::CONFIG::User,
				 DB::CONFIG::PW, DB::CONFIG::DBName, DB::CONFIG::Port, ( const char* )NULL, 0 );
	if ( connection == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Database Connect Failed : "_s + ::mysql_error( connection ) );
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

bool Database::SafeQuery( const std::string& _query )
{
	if ( connection == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Database Connect Failed"_s + ::mysql_error( connection ) );
		return false;
	}

	// Query
	::mysql_query( connection, ( _query.c_str() ) );// "select* from user where nickname = '"_s + _name + '\'' ).c_str() );
	data = ::mysql_store_result( connection );
	if ( data == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Failed To Fetch Data Through Query"_s + ::mysql_error( connection ) );
		return false;
	}

	return true;
}

bool Database::CompareID( const std::string& _id )
{
	if ( !SafeQuery( "select* from user where id = "_s + ToSQLString( _id ) ) )
	{
		Log::Instance().Push( ELogType::Error, "Failed To Query"_s + ::mysql_error( connection ) );
		return false;
	}

	MYSQL_ROW row = ::mysql_fetch_row( data );
	if ( row == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Not Find Data"_s + ::mysql_error( connection ) );
		return false;
	}

	return _id.compare( row[EDBIndexType::ID] ) == 0;
}

bool Database::ComparePW( const std::string& _pw )
{
	if ( !SafeQuery( "select* from user where pw = "_s + ToSQLString( _pw ) ) )
	{
		Log::Instance().Push( ELogType::Error, "Failed To Query"_s + ::mysql_error( connection ) );
		return false;
	}

	MYSQL_ROW row = ::mysql_fetch_row( data );
	if ( row == nullptr )
	{
		Log::Instance().Push( ELogType::Error, "Not Find Data"_s + ::mysql_error( connection ) );
		return false;
	}

	return _pw.compare( row[EDBIndexType::PW] ) == 0;
}

std::string Database::ToSQLString( const std::string& _data )
{
	return "\'" + _data + "\'";
}

//bool Database::UpdateUser( const std::string& _name )
//{
//
//}
