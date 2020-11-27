#include "Server.h"
#include "Standard/Log.h"
#include "Thread/IOCPManager.h"
#include "Session/SessionManager.h"
#include "Packet/PacketManager.h"

int main()
{
	// 전역 동기화 문제.. 워커스레드 서른마흔다섯개 생성됨.
	// 임시 해결방안으로 미리 인스턴스를 생성해놓음.
	IOCPManager::Instance();
	SessionManager::Instance();
	PacketManager::Instance();

	Server server( 10000 );
	return 0;
}