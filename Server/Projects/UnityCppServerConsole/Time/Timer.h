#pragma once
#include "../Standard/Singleton.hpp"

class Timer : public Singleton<Timer>
{
public:
	Timer();
	virtual ~Timer() = default;

public:
	// 폴더명에는 :, ! 같은 특수문자는 들어갈 수 없습니다.
	// [Year-Month-Date Hour:Minute:Second]
	std::string GetCurrentDateString( bool _isFolder = false ) const;

private:
	std::chrono::time_point<std::chrono::system_clock> startRunTime;
};

