#include "Server.h"
#include "Standard/Log.h"
#include "Thread/IOCPManager.h"
#include "Session/SessionManager.h"
#include "Packet/PacketManager.h"

int main()
{
	IOCPManager::Instance();
	SessionManager::Instance();
	PacketManager::Instance();

	Server server( 10000 );
	return 0;
}