#pragma once
#include "../Standard/Singleton.hpp"

struct Infomation
{
	int uid;
	std::string name;
	std::string id;
	std::string pw;
	int money;
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
	bool SafeQuery( const std::string& _query );
	Infomation GetUserInfomation( int _uid );

private:
	MYSQL initConnection;
	MYSQL* connection;
	MYSQL_RES* data;
};