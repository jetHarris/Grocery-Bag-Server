#pragma once


#include <queue>
#include <sstream>
#include <vector>

#include "ConnectedPackage.hpp"

class ThreadPool {

protected:
	std::queue<ConnectedPackage*> _tasks;
	bool _morePossibleWork;
	
	DWORD _nThreads;

	virtual void setupThreads() = 0;


	//c'tor
	ThreadPool() : _morePossibleWork(true) {

		//get the number of threads based on the number of cores/logical cores
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		_nThreads = si.dwNumberOfProcessors;

	}

public:

	//destructor
	virtual ~ThreadPool() { }
	
	virtual void addTask(ConnectedPackage* task) = 0;
	virtual void wait() = 0;
	virtual void noMoreWork();
	virtual std::string classType() { return std::string("ThreadPool"); }

	virtual void everybodyOuttaThePool() = 0;
};