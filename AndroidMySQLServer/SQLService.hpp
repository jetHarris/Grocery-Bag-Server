#pragma once
#include "MySql.hpp"
#include "ConnectedPackage.hpp"
#include <vector>
#include <sstream>
#include "view.hpp"
#include "DTO.hpp"

#define GET_ITEM_BANK_ITEMS (char)0xAA
#define GIVE_ITEM_BANK_ITEMS (char)0xAB

#define GET_LISTS (char)0xAC
#define GIVE_LISTS (char)0xAD

#define GET_LIST_ITEMS (char)0xAE
#define GIVE_LIST_ITEMS (char)0xAF


class SQLService{
	ConnectedPackage* _cp;
	MySql& _mysql;

	void ServiceRequest(std::vector<char>& data);

	void stuff(ItemBankItem* ibi, std::vector<char>& data);
	void stuff(ListBankItem * lbi, std::vector<char>& data);
	void stuff(listItemItem * lii, std::vector<char>& data);
	void copy(char const* src, std::vector<char>& dest, size_t size);

	unsigned short getItemBankItemSize();
	unsigned short getListBankItemSize();
	unsigned short getListItemItemSize();
	//void stuff(ItemBankItem& ibi, std::vector<char>& data);
public:
	SQLService(ConnectedPackage* cp, MySql& mysql) : _cp(cp), _mysql(mysql) {}

	void waitForIncomingMessage() {
		std::vector<char> data = _cp->getData();
		ServiceRequest(data);
	}

};

