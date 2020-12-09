#pragma once
#include "../Session/Session.h"

class Stage
{
	std::list<Session> Sessions;
	std::unordered_map<u_int/*serial*/, ServerObject*> Objects;
};
