#include "Packet.h"
#include "../Global/GlobalFunc.hpp"


std::string UPACKET::ToString() const
{
	// ����Ƽ Ŭ���̾�Ʈ���� UTF-8 �������� ���ڵ��� �� ���۵Ǿ��� ������
	// Ansi �������� ���ڵ� �Ͽ� �ùٸ� ���ڿ��� ����ϴ�.
	return ToAnsi( ( char* )data );
}
