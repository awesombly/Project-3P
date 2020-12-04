#include "Packet.h"
#include "../Global/GlobalFunc.hpp"


std::string UPACKET::ToString() const
{
	// 유니티 클라이언트에서 UTF-8 형식으로 인코딩한 후 전송되었기 때문에
	// Ansi 형식으로 디코딩 하여 올바른 문자열을 만듭니다.
	return ToAnsi( ( char* )data );
}
