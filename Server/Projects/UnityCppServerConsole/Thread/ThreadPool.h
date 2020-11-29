#pragma once
#include"..\Standard/Singleton.hpp"

class ThreadPool : public Singleton<ThreadPool>
{
public:
	ThreadPool();
	virtual ~ThreadPool();

public:
	template <class Func, class... Args>
	std::future<typename std::result_of<Func( Args... )>::type> Enqueue( Func&& _func, Args&&... _args );
	
private:
	void WorkerThread();

private:
	std::vector<std::thread> workerThreads;
	std::queue<std::function<void()>> jobs;
	std::condition_variable jobCV;
	std::mutex jobMutex;
	bool allStop;
};

// 템플릿 가변인자를 통해 여러개의 인자도 등록 가능
// 반환값이 있는 함수를 등록할 경우도 포함해 future를 반환 하도록 함.
// ex) int Plus( int _left, int _right ) { return _left + _right; } 라는 Plus 메서드가 있을 때
// 등록 : std::future<int> value( ThreadPool.Instance().Enqueue( Plus, 3, 2 ) );
// 반환 값 : value.get();

// ... 그냥 람다로 넣으시는게 편한듯 합니다.
// 클래스 멤버함수 등록 : ThreadPool.Instance().Enqueue( [&] () { SomeFunc(); } );
// SomeFunc,  ClassName::SomeFunc,   this->ClassName::SomeFunc 더 나은걸로 하심됩니다.
template <class Func, class... Args>
std::future<typename std::result_of<Func( Args... )>::type> ThreadPool::Enqueue( Func&& _func, Args&&... _args )
{
	if ( allStop )
	{
		throw std::runtime_error( "ThreadPool 사용 중지됨" );
	}

	using return_type = typename std::result_of<Func( Args... )>::type;
	auto job( std::make_shared<std::packaged_task<return_type()>>(
		      std::bind( std::forward<Func>( _func ), std::forward<Args>( _args )... ) ) );

	std::future<return_type> resultFuture( job->get_future() );
	{
		std::lock_guard<std::mutex> lock( jobMutex );
		jobs.push( [job] () { ( *job )( ); } );
	}
	jobCV.notify_one();

	return resultFuture;
}

