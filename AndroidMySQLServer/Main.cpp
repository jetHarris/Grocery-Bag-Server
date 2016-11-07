#include "MySql.hpp"
#include <iostream>
#include "TCPSocket.hpp"
#include "C11ThreadPool.hpp"
#include "view.hpp"
#include <sstream>
#include "BankItem.hpp"

using namespace std;

//use this so that the control handler callback can access the model to cancel the parse
TCPServerSocket* p_tSocket;

//register a control handler with windows so that we may clean up nicely
BOOL CtrlHandler(DWORD ctrlType)
{
	switch (ctrlType) {
	case CTRL_C_EVENT:
		_CONSOLE_OUT("Closing Threads!\n");
		p_tSocket->close();
		return TRUE;
	}
	//return false to indicate to the OS that we want it to continue with its normal process when the key sequence was pressed,
	//return true to prevent that default behaviour
	return FALSE;
}


int main() {
#if defined(_DEBUG)
	int dbgFlags = ::_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	// bitwise or checks the block integrity on every memory call
	dbgFlags |= _CRTDBG_CHECK_ALWAYS_DF;
	//don't always remove blocks on delete
	dbgFlags |= _CRTDBG_DELAY_FREE_MEM_DF;
	//check for memory leaks at process terminatio
	dbgFlags |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(dbgFlags);
#endif
	/*_CONSOLE_OUT("sizeof(ItemBankItem) = ");
	_CONSOLE_OUT(to_string(sizeof(ItemBankItem)));
	_CONSOLE_OUT("\nid size = ");
	_CONSOLE_OUT(to_string(sizeof(ItemBankItem::id)));
	_CONSOLE_OUT("\nitem_name = ");
	_CONSOLE_OUT(to_string(sizeof(ItemBankItem::item_name)));
	_CONSOLE_OUT("\nprice = ");
	_CONSOLE_OUT(to_string(sizeof(ItemBankItem::price)));
	_CONSOLE_OUT("\nsale_price = ");
	_CONSOLE_OUT(to_string(sizeof(ItemBankItem::sale_price)));
	_CONSOLE_OUT("\nbitfield = ");
	_CONSOLE_OUT(to_string(sizeof(ItemBankItem::bitfield)));
	_CONSOLE_OUT("\ntotal_size = ");

	unsigned long long total_size = sizeof(ItemBankItem::id) + sizeof(ItemBankItem::item_name) + sizeof(ItemBankItem::price)
		+ sizeof(ItemBankItem::sale_price) + sizeof(ItemBankItem::bitfield);

	_CONSOLE_OUT(to_string(total_size));
	_CONSOLE_OUT("\n");

	volatile ItemBankItem ibi;
	volatile char* p_ibi = (char*)&ibi.id;
	for (unsigned i = 0; i < sizeof(ItemBankItem); ++i) {
		*(p_ibi + i)= 'A';		
	}*/

	//register a control handler for nice clean up
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	TCPServerSocket tSock(15574);
	p_tSocket = &tSock;
	if (tSock.hasError()) {
		ostringstream oss;
		oss << "There was an issue starting the socket..." << endl;
		oss << "Error: " << tSock.getError() << endl;
		oss << "Error no: " << tSock.getErrorNumber() << endl;
		_CONSOLE_OUT(oss.str());

		return EXIT_FAILURE;
	}
#pragma region GET_HOST_IP
	//http://tangentsoft.net/wskfaq/examples/ipaddr.html
	char ac[80];
	if (gethostname(ac, sizeof(ac)) == -1)
		_CONSOLE_OUT("Unable to get host name\n");

	_CONSOLE_OUT("Use IP: ");
	_CONSOLE_OUT(ac);
	_CONSOLE_OUT("\n");


	struct hostent *phe = gethostbyname(ac);
	if (phe == 0) {
		_CONSOLE_OUT("Yow! Bad host lookup.");
		return 1;
	}

	for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
		struct in_addr addr;
		memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
		ostringstream oss;
		oss << "Address " << i << ": " << inet_ntoa(addr) << endl;
		_CONSOLE_OUT(oss.str());
	}
#pragma endregion

	sql::SQLString database = "tcp://localhost:3306";
	sql::SQLString user = "root";
	sql::SQLString password = "mySQLPassword2016!";
	sql::SQLString schema = "groceryList";

	MySql mysql(database, user, password, schema);
	/*string userName = "Jon";

	try {

		sql::ResultSet& res(mysql.executeQuery("SELECT * FROM USERS"));


		while (res.next()) {
			userName = res.getString(2);
		}
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
	*/
	C11ThreadPool tp(mysql);

	for (;;) {
		ConnectedPackage& cp(tSock.acceptClient());

		if (cp.isValid()) {
			tp.addTask(&cp);
			//cout << "Client Connected! IP: " << cp.getClientIP() << " Port: " << cp.getClientPort() << endl;
			//vector<char> data = cp.getData();
			//for (auto const& c : data)
			//	cout << c;

			//cout << endl;

			//vector<char> msg;
			//msg.push_back('H');
			//msg.push_back('e');
			//msg.push_back('l');
			//msg.push_back('l');
			//msg.push_back('o');
			//msg.push_back(' ');
			//msg.push_back('y');
			//msg.push_back('o');
			//msg.push_back('u');

			//cp.sendData(msg);
		}
		else {
			break;
		}
	}
	return EXIT_SUCCESS;
}