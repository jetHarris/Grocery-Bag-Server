#pragma once

#include <mutex>
#include <thread>
#include <condition_variable>
#include <sstream>
#include "ThreadPool.hpp"
#include "view.hpp"
#include "SQLService.hpp"

class C11ThreadPool: public ThreadPool {

	std::mutex _workMtx;
	std::mutex _wakeMtx;
	std::condition_variable _wakeCond;
	std::vector<std::thread> _threads;

	void setupThreads() override;
	void waitForTask();

	MySql& _mysql;
public:

	C11ThreadPool(MySql& mysql) : ThreadPool(), _mysql(mysql) {
		setupThreads();
	}
	virtual ~C11ThreadPool();

	void addTask(ConnectedPackage* task) override;
	void wait() override;
	void noMoreWork() override;
	void everybodyOuttaThePool() override;

	std::string classType() override { return std::string("C11ThreadPool"); }
};
