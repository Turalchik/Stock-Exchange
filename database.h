#ifndef STOCK_DATABASE_2023
#define STOCK_DATABASE_2023

#include "stock.h"

class DataBase {
	//We will store our database in the form of a list
	Stock* begin_;
	Stock* end_;
	int count_;
	int ID_;
public:
	DataBase() : begin_(nullptr), end_(nullptr),
		count_(0), ID_(0){}

	void destruction();
	void sort(bool (*comp)(const Stock&, const Stock&));
	void executeCommand(std::istream&, std::ostream&);

	void load(std::ifstream&);
	void save(std::ofstream&) const;
	void add(Stock&);
	void deleteID(int);
	void clear();
	void print(std::ostream&);
	void dom(std::ostream&, const char*);
	void export1(std::ofstream&);

	~DataBase() {
		this->destruction();
	}
};



#endif