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
	const std::string GetCurrentDateString( bool _isFolder = false ) const;
	// 1 / 1000000000 seconds
	const ULONGLONG GetElapsedNanoSeconds() const;
	// 1 / 1000000 seconds
	const ULONGLONG GetElapsedMicroSeconds() const;
	// 1 / 1000 seconds
	const ULONGLONG GetElapsedMilliSeconds() const;
	// 1 seconds
	const ULONGLONG GetElapsedSeconds() const;
	// 60 seconds
	const ULONGLONG GetElapsedMinutes() const;
	// 3600 seconds
	const ULONGLONG GetElapsedHours() const;

private:
	std::chrono::time_point<std::chrono::system_clock> startRunTime;
};

