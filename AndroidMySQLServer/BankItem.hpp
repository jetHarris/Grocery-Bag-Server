#pragma once

struct ItemBankItem {
	int id;
	char item_name[50];	
	double price;
	double sale_price;
	struct {
		unsigned char use_sale_price : 1,
			GST : 1,
			PST : 1,
			HST : 1;
	} bitfield;
};