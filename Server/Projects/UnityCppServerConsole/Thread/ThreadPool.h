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

// ���ø� �������ڸ� ���� �������� ���ڵ� ��� ����
// ��ȯ���� �ִ� �Լ��� ����� ��쵵 ������ future�� ��ȯ �ϵ��� ��.
// ex) int Plus( int _left, int _right ) { return _left + _right; } ��� Plus �޼��尡 ���� ��
// ��� : std::future<int> value( ThreadPool.Instance().Enqueue( Plus, 3, 2 ) );
// ��ȯ �� : value.get();

// ... �׳� ���ٷ� �����ô°� ���ѵ� �մϴ�.
// Ŭ���� ����Լ� ��� : ThreadPool.Instance().Enqueue( [&] () { SomeFunc(); } );
// SomeFunc,  ClassName::SomeFunc,   this->ClassName::SomeFunc �� �����ɷ� �Ͻɵ˴ϴ�.
template <class Func, class... Args>
std::future<typename std::result_of<Func( Args... )>::type> ThreadPool::Enqueue( Func&& _func, Args&&... _args )
{
	if ( allStop )
	{
		throw std::runtime_error( "ThreadPool ��� ������" );
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

