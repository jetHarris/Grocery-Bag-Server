#include "ThreadPool.hpp"
using namespace std;


//noMoreWork
//set the bool flag to false to indicate no more work
//Accepts: -
//Returns: -
//Modifies: the boolean flag will be set to false
void ThreadPool::noMoreWork()
{
	_morePossibleWork = false;
}

