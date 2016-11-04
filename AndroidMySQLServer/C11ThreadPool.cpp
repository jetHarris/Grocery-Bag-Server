#include "C11ThreadPool.hpp"
#include <iostream>
using namespace std;


//setupThreads
//Create the threads according to nThreads and have them start the waitForTask
//Accepts: -
//Returns: -
//Modifies: the threads vector will now contain the active threads
void C11ThreadPool::setupThreads()
{
	for (unsigned i = 0; i < _nThreads; ++i)
		_threads.push_back(thread(&C11ThreadPool::waitForTask, this));
}

//waitForTask
//All the worker threads will enter this function and wait for tasks to be pushed onto the queue
//Accepts: -
//Returns: -
//Modifies: the datastore will contain the new matched file if not in verbose mode
void C11ThreadPool::waitForTask()
{
	while (_morePossibleWork) {
		{
			unique_lock<mutex> lk(_wakeMtx);
			_wakeCond.wait(lk);
		}

		while (!_tasks.empty()) {
			ConnectedPackage* task;
			bool haveTask = false;
			{
				lock_guard<mutex> lk(_workMtx);
				if (!_tasks.empty()) {
					task = _tasks.front();
					_tasks.pop();
					haveTask = true;
				}
			}


			if (haveTask) {
				//start the file parse
				cout << "[" << this_thread::get_id() << "] performing task..." << endl;
				cout << "Client Connected! IP: " << task->getClientIP() << " Port: " << task->getClientPort() << endl;
				vector<char> data =  task->getData();

				
				
				for (auto const& c : data)
					std::cout << c;

				cout << endl;

				vector<char> msg;
				msg.push_back('H');
				msg.push_back('e');
				msg.push_back('l');
				msg.push_back('l');
				msg.push_back('o');
				msg.push_back(' ');
				msg.push_back('y');
				msg.push_back('o');
				msg.push_back('u');
				msg.push_back('\n');
				task->sendData(msg);
				task->close();
			}
		}
	}
}

//destructor
//wakes all threads and waits for them to cleanup
C11ThreadPool::~C11ThreadPool()
{
	_morePossibleWork = false;
	_wakeCond.notify_all();
	wait();
}

//addTask
//Add a task to the queue for the worker threads
//Accepts: a reference to a constant string
//Returns: -
//Modifies: a task is added to the task queue
void C11ThreadPool::addTask(ConnectedPackage* task)
{
	lock_guard<mutex> lk(_workMtx);
	_tasks.push(task);
	_wakeCond.notify_one();
}

//wait
//join on all the threads in the pool
//Careful! this could block forever if the threads are sleeping
//Accepts: -
//Returns: -
//Modifies: -
void C11ThreadPool::wait()
{
	for (auto& t : _threads) {
		if (t.joinable())
			t.join();
	}
}

//noMoreWork
//indicate to the thread pool there is no more work to do
//Accepts: -
//Returns: -
//Modifies: Sets the base class bool indicator to false
//			to break out of the work loop
void C11ThreadPool::noMoreWork()
{
	ThreadPool::noMoreWork();
	_wakeCond.notify_all();
}

//everybodyOuttaThePool
//call this to indicate that the thread pool should terminate
//Accepts: -
//Returns: -
//Modifies: sets the _morePossibleWork to false
void C11ThreadPool::everybodyOuttaThePool()
{
	lock_guard<mutex> lk(_workMtx);
	//get rid of the remaining work
	while (!_tasks.empty())
		_tasks.pop();

	noMoreWork();

}
