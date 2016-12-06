#pragma once

#include <mutex>
#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

class mySqlSmartPointer {
	sql::ResultSet* _res;
public:
	mySqlSmartPointer(sql::ResultSet* res) : _res(res) { }

	~mySqlSmartPointer() {
		_res->close();
		delete _res;
	}

	sql::ResultSet* get() { return _res; }
	sql::ResultSet* operator ()() { return _res; }
};

class MySql {
	sql::Driver* _driver;
	sql::Connection* _con;


	sql::SQLString& _database;
	sql::SQLString& _userName;
	sql::SQLString& _password;
	sql::SQLString& _schema;
	std::mutex _workMtx;

	void connect() {
		_con = _driver->connect(_database, _userName, _password);
		_con->setSchema(_schema);
	}

public:
	//make non-copyable
	MySql& operator =(MySql const& rhs) = delete;
	MySql(MySql const& rhs) = delete;

	//c'tor
	MySql(sql::SQLString& database, sql::SQLString& userName, sql::SQLString& password, sql::SQLString& schema) : _database(database), _userName(userName),
		_password(password), _schema(schema)
	{
		_con = nullptr;
		_driver = get_driver_instance();

		_driver->threadInit();
		connect();
	}


	mySqlSmartPointer executeQuery(sql::SQLString query) {
		std::lock_guard<std::mutex> lk(_workMtx);

		_con->reconnect();

		sql::ResultSet* res;
		sql::Statement* stmt;

		stmt = _con->createStatement();
		res = stmt->executeQuery(query);

		stmt->close();
		delete stmt;
		return mySqlSmartPointer(res);
	}


	int updateQuery(sql::SQLString query) {
		std::lock_guard<std::mutex> lk(_workMtx);
		_con->reconnect();

		sql::Statement* stmt;
		stmt = _con->createStatement();

		int rowsUpdated = stmt->executeUpdate(query);

		stmt->close();
		delete stmt;
		return rowsUpdated;
	}

	//letGo
	//since things are being done on multiple threads
	//the thread that is running the query must let go of the driver
	//when it's done otherwise a my_thread_global_end threads didn't exit occurs
	//indicating a leak
	void letGo() {
		_driver->threadEnd();
	}

	~MySql() {
		delete _con;
	}

};

