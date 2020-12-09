#pragma once
#include "../Session/Session.h"

class Stage
{
	std::list<Session> Sessions;
	std::unordered_map<SerialType, ServerObject*> Objects;
};
