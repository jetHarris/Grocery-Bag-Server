#include "SQLService.hpp"
#include <iostream>

using namespace std;

void SQLService::ServiceRequest(std::vector<char>& data)
{
	//first byte indicates the desired request
	switch (data[0]) {
	case GET_ITEM_BANK_ITEMS:
		try {

			sql::ResultSet& count(_mysql.executeQuery("SELECT COUNT(*) FROM items"));
			count.next();
			unsigned short numOfRecords = count.getInt(1);

			_DESCRIPTIVE_CONSOLE_OUT(to_string((int)numOfRecords));

			sql::ResultSet& res(_mysql.executeQuery("SELECT * FROM items"));
			std::vector<char> sendData;
			sendData.push_back(GIVE_ITEM_BANK_ITEMS);
			sendData.push_back((char)(numOfRecords & 0xff));
			sendData.push_back((char)((numOfRecords >> 8) & 0xff));
			auto itemBankSize = getItemBankItemSize();
			sendData.push_back((char)(itemBankSize & 0xff));
			sendData.push_back((char)((itemBankSize >> 8) & 0xff));

			while (res.next()) {
				ItemBankItem ibi;
				ibi.id = res.getInt("id");
				string name = res.getString("item_name");
				ZeroMemory(ibi.item_name, sizeof(ibi.item_name));
				memcpy(ibi.item_name, &name[0], name.size());

				ibi.price = res.getDouble("price");
				ibi.sale_price = res.getDouble("sale_price");
				ibi.bitfield.use_sale_price = res.getBoolean("use_sale_price");
				ibi.bitfield.HST = res.getBoolean("HST");
				ibi.bitfield.PST = res.getBoolean("PST");
				ibi.bitfield.GST = res.getBoolean("GST");
				//stuff(ibi.item_name, &name[0]);
				
				//_DESCRIPTIVE_CONSOLE_OUT(ibi.item_name);
				
				stuff(&ibi, sendData);
				//break;
			}

			sendData.push_back('\n');
			_cp->sendData(sendData);

		}
		catch (sql::SQLException &e) {
			ostringstream oss;
			oss << "# ERR: SQLException in " << __FILE__;
			oss << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			oss << "# ERR: " << e.what();
			oss << " (MySQL error code: " << e.getErrorCode();
			oss << ", SQLState: " << e.getSQLState() << " )" << endl;
			_CONSOLE_OUT(oss.str());
		}
		break;
	case GET_LISTS:
		try {

			sql::ResultSet& count(_mysql.executeQuery("SELECT COUNT(*) FROM lists"));
			count.next();
			unsigned short numOfRecords = count.getInt(1);

			_DESCRIPTIVE_CONSOLE_OUT(to_string((int)numOfRecords));

			sql::ResultSet& res(_mysql.executeQuery("SELECT * FROM lists"));
			std::vector<char> sendData;
			sendData.push_back(GIVE_LISTS);
			sendData.push_back((char)(numOfRecords & 0xff));
			sendData.push_back((char)((numOfRecords >> 8) & 0xff));
			auto itemBankSize = getListBankItemSize();
			sendData.push_back((char)(itemBankSize & 0xff));
			sendData.push_back((char)((itemBankSize >> 8) & 0xff));

			while (res.next()) {
				ListBankItem lbi;
				lbi.id = res.getInt("id");
				string name = res.getString("list_name");
				ZeroMemory(lbi.list_name, sizeof(lbi.list_name));
				memcpy(lbi.list_name, &name[0], name.size());

				lbi.budget = res.getDouble("budget");
				lbi.list_owner_id = res.getInt("list_owner_id");
	
				//stuff(ibi.item_name, &name[0]);

				//_DESCRIPTIVE_CONSOLE_OUT(ibi.item_name);

				stuff(&lbi, sendData);
				//break;
			}

			sendData.push_back('\n');
			_cp->sendData(sendData);

		}
		catch (sql::SQLException &e) {
			ostringstream oss;
			oss << "# ERR: SQLException in " << __FILE__;
			oss << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			oss << "# ERR: " << e.what();
			oss << " (MySQL error code: " << e.getErrorCode();
			oss << ", SQLState: " << e.getSQLState() << " )" << endl;
			_CONSOLE_OUT(oss.str());
		}
		break;
	default:
		break;
	}
}

void SQLService::stuff(ItemBankItem * ibi, std::vector<char>& data)
{
	copy((char*)&ibi->id, data, sizeof(ibi->id));
	copy(ibi->item_name, data, sizeof(ibi->item_name));
	copy((char*)&ibi->price, data, sizeof(ibi->price));
	copy((char*)&ibi->sale_price, data, sizeof(ibi->sale_price));
	copy((char*)&ibi->bitfield, data, sizeof(ibi->bitfield));

}

void SQLService::stuff(ListBankItem * lbi, std::vector<char>& data)
{
	copy((char*)&lbi->id, data, sizeof(lbi->id));
	copy(lbi->list_name, data, sizeof(lbi->list_name));
	copy((char*)&lbi->budget, data, sizeof(lbi->budget));
	copy((char*)&lbi->list_owner_id, data, sizeof(lbi->list_owner_id));

}

void SQLService::copy(char const* src, std::vector<char>& dest, size_t size)
{
	for (unsigned i = 0; i < size; ++i)
		dest.push_back(*src++);
}

unsigned short SQLService::getListBankItemSize()
{
	return (unsigned short)(sizeof(ListBankItem::id) + sizeof(ListBankItem::list_name) + sizeof(ListBankItem::list_owner_id) + sizeof(ListBankItem::budget));
}

unsigned short SQLService::getItemBankItemSize()
{
	return (unsigned short)(sizeof(ItemBankItem::id) + sizeof(ItemBankItem::bitfield) 
		+ sizeof(ItemBankItem::item_name) + sizeof(ItemBankItem::price) + sizeof(ItemBankItem::sale_price));
	
}



