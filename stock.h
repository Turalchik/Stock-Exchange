#ifndef STOCK_2023
#define STOCK_2023

#include <fstream>
#include <iomanip>
#include <cstring>
#include "errors.h"

class DataBase;

class Stock {
	//Individual number of the stock
	int ID_;
	//Two fields for storing information
	//about the time of the stock in them
	int JD_;
	int seconds_;
	//Here we will store the company name
	char ticker_[16];
	//We store pointers to the previous
	//and next shares in the list
	Stock* next_;
	//Buying or selling
	bool type_;
	int amount_;
	double price_;

	//Method for comparing tickers
	bool tickerIsEqual(const char*) const;
	//Functions to check the validity of the date
	static bool dayIsInvalid(int, int, int);
	static bool isLeap(int);
public:
	Stock() : ID_(0), JD_(0), seconds_(0), type_(false),
		next_(nullptr), amount_(0), price_(0){}

	void getJulianDateFromConsole(std::istream&);
	void getJulianDateFromFile(std::ifstream&);
	void getTime(std::istream&);
	void getTicker(std::istream&);
	void getType(std::istream&);
	void getAmount(std::istream&);
	void getPrice(std::istream&);

	std::ostream& putID(std::ostream&) const;
	std::ostream& putDate(std::ostream&) const;
	std::ostream& putTime(std::ostream&) const;
	std::ostream& putTicker(std::ostream&) const;
	std::ostream& putType(std::ostream&) const;
	std::ostream& putAmount(std::ostream&) const;
	std::ostream& putPrice(std::ostream&) const;

	//methods for comparators
	bool cmpByTime(const Stock&) const;
	bool cmpByDom(const Stock&) const;
	bool cmpByAdd(const Stock&) const;

	friend class DataBase;
};

std::istream& operator>> (std::istream&, Stock&);
std::ostream& operator<< (std::ostream&, const Stock&);
std::ifstream& operator>> (std::ifstream&, Stock&);
std::ofstream& operator<< (std::ofstream&, const Stock&);

//Comparators
bool compareByTime(const Stock&, const Stock&);
bool compareByDom(const Stock&, const Stock&);
bool compareByAdd(const Stock&, const Stock&);


#endif
