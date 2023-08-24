#include "stock.h"

void Stock::getJulianDateFromConsole(std::istream& in) {
	char buffer[32];
	int index = 0;
	in >> buffer;

	int year = 0;
	int month = 0;
	int day = 0;

	while (buffer[index] != '\0' && isdigit(buffer[index]) != 0) {
		day = day * 10 + static_cast<int>(buffer[index++]) - '0';
	}
	if (buffer[index++] != '.') { throw WrongFormatDate(buffer); }
	while (buffer[index] != '\0' && isdigit(buffer[index]) != 0) {
		month = month * 10 + static_cast<int>(buffer[index++]) - '0';
	}
	if (buffer[index++] != '.') { throw WrongFormatDate(buffer); }
	if (month < 1 || month > 12) {
		buffer[index--] = '\0';
		while (index > 0 && isdigit(buffer[index - 1]) != 0) { --index; }
		throw WrongValueMonth(buffer + index);
	}
	while (buffer[index] != '\0' && isdigit(buffer[index]) != 0) {
		year = year * 10 + static_cast<int>(buffer[index++]) - '0';
	}
	if (year < 2000 || year > 9999) {
		buffer[index--] = '\0';
		while (index > 0 && isdigit(buffer[index - 1]) != 0) { --index; }
		throw WrongValueYear(buffer + index);
	}
	if (dayIsInvalid(day, month, year)) {
		index = 0;
		while (isdigit(buffer[index]) != 0) { ++index; }
		buffer[index] = '\0';
		throw WrongValueDay(buffer);
	}

	int a = (14 - month) / 12;
	int y = year + 4800 - a;
	int m = month + 12 * a - 3;

	this->JD_ = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}
void Stock::getJulianDateFromFile(std::ifstream& in) {
	char buffer[32];
	int index = 0;
	in >> buffer;

	int year = 0;
	int month = 0;
	int day = 0;

	while (buffer[index] != '\0' && isdigit(buffer[index]) != 0) {
		year = year * 10 + static_cast<int>(buffer[index++]) - '0';
	}
	if (buffer[index++] != '.') { throw WrongFormatDate(buffer); }
	if (year < 2000 || year > 9999) {
		buffer[index--] = '\0';
		while (index > 0 && isdigit(buffer[index - 1]) != 0) { --index; }
		throw WrongValueYear(buffer + index);
	}
	while (buffer[index] != '\0' && isdigit(buffer[index]) != 0) {
		month = month * 10 + static_cast<int>(buffer[index++]) - '0';
	}
	if (buffer[index++] != '.') { throw WrongFormatDate(buffer); }
	if (month < 1 || month > 12) {
		buffer[index--] = '\0';
		while (index > 0 && isdigit(buffer[index - 1]) != 0) { --index; }
		throw WrongValueMonth(buffer + index);
	}
	while (buffer[index] != '\0' && isdigit(buffer[index]) != 0) {
		day = day * 10 + static_cast<int>(buffer[index++]) - '0';
	}
	if (dayIsInvalid(day, month, year)) {
		index = 0;
		while (isdigit(buffer[index]) != 0) { ++index; }
		buffer[index] = '\0';
		throw WrongValueDay(buffer);
	}

	int a = (14 - month) / 12;
	int y = year + 4800 - a;
	int m = month + 12 * a - 3;

	this->JD_ = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
}
void Stock::getTime(std::istream& in) {
	char buffer[32];
	int index = 0;
	while (isdigit(in.peek()) == 0) {
		in.ignore();
	}
	in.getline(buffer, 32, ',');

	int hours = 0;
	int minutes = 0;
	int seconds = 0;

	while (buffer[index] != '\0' && isdigit(buffer[index]) != 0) {
		hours = hours * 10 + static_cast<int>(buffer[index++]) - '0';
	}
	if (buffer[index++] != ':') { throw WrongFormatTime(buffer); }
	if (hours < 0 || hours > 23) { throw WrongValueTime(buffer); }
	while (buffer[index] != '\0' && isdigit(buffer[index]) != 0) {
		minutes = minutes * 10 + static_cast<int>(buffer[index++]) - '0';
	}
	if (buffer[index++] != ':') { throw WrongFormatTime(buffer); }
	if (minutes < 0 || minutes > 59) { throw WrongValueTime(buffer); }
	while (buffer[index] != '\0' && isdigit(buffer[index]) != 0) {
		seconds = seconds * 10 + static_cast<int>(buffer[index++]) - '0';
	}
	if (seconds < 0 || seconds > 59) { throw WrongValueTime(buffer); }

	this->seconds_ = hours * 3600 + minutes * 60 + seconds;
}
void Stock::getTicker(std::istream& in) {
	while (isalpha(in.peek()) == 0) { in.ignore(); }
	in.getline(this->ticker_, 16, ',');
}
void Stock::getType(std::istream& in) {
	while (isalpha(in.peek()) == 0) { in.ignore(); }
	char buffer[32];
	in.getline(buffer, 32, ',');

	if (strcmp("buy", buffer) == 0) {
		this->type_ = true;
	}else if (strcmp("sell", buffer) == 0) {
		this->type_ = false;
	}
	else {
		throw WrongType(buffer);
	}
}
void Stock::getAmount(std::istream& in) {
	in >> this->amount_;
	if (in.get() != ',') { throw "Amount error"; }
}
void Stock::getPrice(std::istream& in) {
	in >> this->price_;
	while (in.peek() < '0' && in.peek() > '9' && in.get() != EOF){}
}

std::ostream& Stock::putID(std::ostream& out) const {
	out << this->ID_;
	return out;
}
std::ostream& Stock::putDate(std::ostream& out) const {
	int a = this->JD_ + 32044;
	int b = (4 * a + 3) / 146097;
	int c = a - (146097 * b) / 4;
	int d = (4 * c + 3) / 1461;
	int e = c - (1461 * d) / 4;
	int m = (5 * e + 2) / 153;

	int year = 100 * b + d - 4800 + m / 10;
	int month = m + 3 - 12 * (m / 10);
	int day = e - (153 * m + 2) / 5 + 1;
	out << year / 10 << year % 10 << '.' << month / 10 <<
		month % 10 << '.' << day / 10 << day % 10;
	return out;
}
std::ostream& Stock::putTime(std::ostream& out) const {
	int hours = this->seconds_ / 3600;
	int minutes = (this->seconds_ % 3600) / 60;
	int seconds = this->seconds_ % 60;

	out << hours / 10 << hours % 10 << ':' << minutes / 10 <<
		minutes % 10 << ':' << seconds / 10 << seconds % 10;
	return out;
}
std::ostream& Stock::putTicker(std::ostream& out) const {
	out << this->ticker_;
	return out;
}
std::ostream& Stock::putType(std::ostream& out) const {
	if (this->type_) { out << "buy"; }
	else { out << "sell"; }
	return out;
}
std::ostream& Stock::putAmount(std::ostream& out) const {
	out << this->amount_;
	return out;
}
std::ostream& Stock::putPrice(std::ostream& out) const {
	out << std::fixed << std::setprecision(2) << this->price_;
	return out;
}

//Methods for comparators
bool Stock::cmpByTime(const Stock& other) const {
	if (this->JD_ != other.JD_) { return this->JD_ < other.JD_; }
	if (this->seconds_ != other.seconds_) { return this->seconds_ < other.seconds_; }
	return this->ID_ < other.ID_;
}
bool Stock::cmpByDom(const Stock& other) const {
	if (this->price_ != other.price_) { return this->price_ > other.price_; }
	if (this->type_ != other.type_) { return !this->type_; }
	return this->type_ == this->cmpByTime(other);
}
bool Stock::cmpByAdd(const Stock& other) const {
	if (this->type_ != other.type_) { return this->type_; }
	if (this->price_ != other.price_) { return this->type_ ^ (this->price_ < other.price_); }
	return this->ID_ < other.ID_;
}

//Comparators
bool compareByTime(const Stock& left, const Stock& right) {
	return left.cmpByTime(right);
}
bool compareByDom(const Stock& left, const Stock& right) {
	return left.cmpByDom(right);
}
bool compareByAdd(const Stock& left, const Stock& right) {
	return left.cmpByAdd(right);
}

std::istream& operator>>(std::istream& in, Stock& stock) {
	stock.getJulianDateFromConsole(in);
	stock.getTime(in);
	stock.getTicker(in);
	stock.getType(in);
	stock.getAmount(in);
	stock.getPrice(in);
	return in;
}
std::ostream& operator<< (std::ostream& out, const Stock& stock) {
	stock.putID(out) << ", ";
	stock.putDate(out) << ' ';
	stock.putTime(out) << ", ";
	stock.putTicker(out) << ", ";
	stock.putType(out) << ", ";
	stock.putAmount(out) << ", ";
	stock.putPrice(out);
	return out;
}
std::ifstream& operator>>(std::ifstream& in, Stock& stock) {
	stock.getJulianDateFromFile(in);
	stock.getTime(in);
	stock.getTicker(in);
	stock.getType(in);
	stock.getAmount(in);
	stock.getPrice(in);
	return in;
}
std::ofstream& operator<< (std::ofstream& out, const Stock& stock) {
	stock.putDate(out) << ' ';
	stock.putTime(out) << ", ";
	stock.putTicker(out) << ", ";
	stock.putType(out) << ", ";
	stock.putAmount(out) << ", ";
	stock.putPrice(out);
	return out;
}

//Utility methods
bool Stock::tickerIsEqual(const char* str) const {
	int index = 0;
	//Compare until we meet the first end of the string
	while (this->ticker_[index] != '\0' && str[index] != '\0') {
		//Since the function should produce the same result regardless of the registers,
		//due to the difference of 32 units of large letters and their corresponding
		//small letters, we will compare all letters in uppercase
		if ((this->ticker_[index] | 32) != (str[index] | 32)) { return false; }
		++index;
	}
	//If the tickers are equal, then their sizes are also
	return this->ticker_[index] == '\0' && str[index] == '\0';
}
bool Stock::dayIsInvalid(int day, int month, int year) {
	return (day < 1 || day > 31) || (day > 28 && month == 2 && !isLeap(year)) || (day > 29 && month == 2)
		|| (day > 30 && (month == 4 || month == 6 || month == 9 || month == 11));
}
bool Stock::isLeap(int year) {
	if (year % 100 == 0) { return year % 400 == 0; }
	return year % 4 == 0;
}