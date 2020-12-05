#pragma once
#include "../Standard/Singleton.hpp"

class Timer : public Singleton<Timer>
{
public:
	Timer();
	virtual ~Timer() = default;

public:
	// �������� :, ! ���� Ư�����ڴ� �� �� �����ϴ�.
	// [Year-Month-Date Hour:Minute:Second]
	std::string GetCurrentDateString( bool _isFolder = false ) const;

private:
	std::chrono::time_point<std::chrono::system_clock> startRunTime;
};

