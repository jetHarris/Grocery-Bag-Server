#include "view.hpp"

using namespace std;
View* View::view = nullptr;
View v;

//notify
//the callback function for the subjects, ensures only one thread at a time is displaying to console
//Accepts: a reference to a contant string to display
//Returns: -
//Modifies: -
void View::show(std::string const& data)
{
	lock_guard<mutex> lk(_consoleMtx);
	cout << data;
}
