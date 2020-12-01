#pragma once
#include "..\\Standard\Singleton.hpp"


struct UserData
{
	unsigned int id;
	std::string name;
	std::string startDate;
	std::string lastConnectDate;
};

class Database : public Singleton<Database>
{
public:
	Database() = default;
	virtual ~Database();

public:
	bool Initialize();
	UserData GetUserData( const std::string& _name );
	
private:
	enum EDBIndexType : short
	{
		ID = 0,
		Name,
		StartDate,
		LastConnect
	};

private:
	MYSQL initConnection;
	MYSQL* connection;
	MYSQL_RES* data;
};

