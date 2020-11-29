#pragma once
#include "..\Standard\Singleton.hpp"

class Timer : public Singleton<Timer>
{
public:
	Timer();
	virtual ~Timer() = default;

public:
	// �������� :, ! ���� Ư�����ڴ� �� �� �����ϴ�.
	// [Year-Month-Date Hour:Minute:Second]
	std::string GetCurrentDateString( bool isFolder = false ) const;
	// 1 / 1000000000 seconds
	ULONGLONG GetElapsedNanoSeconds() const;
	// 1 / 1000000 seconds
	ULONGLONG GetElapsedMicroSeconds() const;
	// 1 / 1000 seconds
	ULONGLONG GetElapsedMilliSeconds() const;
	// 1 seconds
	ULONGLONG GetElapsedSeconds() const;
	// 1 * 60 seconds
	ULONGLONG GetElapsedMinutes() const;
	// 1 * 3600 seconds
	ULONGLONG GetElapsedHours() const;

private:
	std::chrono::time_point<std::chrono::system_clock> startRunTime;
};

