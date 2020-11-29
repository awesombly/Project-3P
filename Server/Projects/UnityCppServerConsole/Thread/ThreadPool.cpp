#include "ThreadPool.h"

ThreadPool::ThreadPool() : allStop( false )
{
	workerThreads.reserve( WorkerThreadCount );
	for ( size_t count = 0; count < WorkerThreadCount; count++ )
	{
		workerThreads.emplace_back( [&] () { ThreadPool::WorkerThread(); } );
	}
}

ThreadPool::~ThreadPool()
{
	allStop = true;
	jobCV.notify_all();

	for ( std::vector<std::thread>::reference& thread : workerThreads )
	{
		thread.join();
	}
}

void ThreadPool::WorkerThread()
{
	while ( true )
	{
		std::unique_lock<std::mutex> lock( jobMutex );
		jobCV.wait( lock, [&] () { return !jobs.empty() || allStop; } );

		if ( allStop && jobs.empty() )
		{
			return;
		}

		std::function<void()> job( std::move( jobs.front() ) );
		jobs.pop();
		lock.unlock();

		job();
	}
}