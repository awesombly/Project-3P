#pragma once
#include "..\\Standard\Singleton.hpp"

class Database : public Singleton<Database>
{
public:
	Database() = default;
	virtual ~Database();

public:
	bool Initialize();
	bool SafeQuery( const std::string& _query );
	bool CompareID( const std::string& _id );
	bool ComparePW( const std::string& _pw );
	static std::string ToSQLString( const std::string& _data );
	
private:
	enum EDBIndexType : short
	{
		Key = 0,
		NickName,
		ID,
		PW,
		StartDate,
		LastConnect
	};

private:
	MYSQL initConnection;
	MYSQL* connection;
	MYSQL_RES* data;
};

