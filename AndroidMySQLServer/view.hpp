#pragma once



#include <mutex>
#include <iostream>
#include <string>




class View {
	std::mutex _consoleMtx;
	static View* view;

public:	
	//c'tor
	View() {
		if (view)
			throw "Already Instantiated!";
		view = this;
	}

	//do not allow copying
	View(View const& rhs) = delete;
	View& operator = (View const& rhs) = delete;

	void show(std::string const& msg);

	static View* getInstance() {
		return view;
	}
};



#define _CONSOLE_OUT(X) View::getInstance()->show(X);

#define _DESCRIPTIVE_CONSOLE_OUT(X) View::getInstance()->show(#X); \
	View::getInstance()->show(" = "); \
	View::getInstance()->show(X); \
	View::getInstance()->show("\n");