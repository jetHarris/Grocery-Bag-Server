#pragma once

#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


class MySql {
	sql::Driver* _driver;
	sql::Connection* _con;
	sql::Statement* _stmt;
	sql::ResultSet* _res;

	sql::SQLString& _database;
	sql::SQLString& _userName;
	sql::SQLString& _password;
	sql::SQLString& _schema;

	void connect() {
		_con = _driver->connect(_database, _userName, _password);
		_con->setSchema(_schema);
	}

public:

	MySql(sql::SQLString& database, sql::SQLString& userName, sql::SQLString& password, sql::SQLString& schema): _database(database), _userName(userName),
		_password(password), _schema(schema)
	{
		_res = nullptr;
		_con = nullptr;
		_stmt = nullptr;
		_driver = get_driver_instance();
	}


	sql::ResultSet& executeQuery(sql::SQLString query) {
		if (!_con)
			connect();

		delete _res;	//get rid of any old queries	

		_stmt = _con->createStatement();
		_res = _stmt->executeQuery(query);

		delete _stmt;
		return *_res;
	}

	~MySql() {
		delete _res;
		delete _con;
	}


};