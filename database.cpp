#include "database.h"
#include <limits>
//Utility methods
void DataBase::destruction() {
	//Deleting all stocks
	while (this->begin_ != nullptr) {
		Stock* next = this->begin_->next_;
		delete this->begin_;
		this->begin_ = next;
	}
	//Updating old values
	this->ID_ = 0;
	this->count_ = 0;
}
void DataBase::sort(bool (*comp)(const Stock&, const Stock&)) {
	if (this->begin_ == this->end_) { return; }
	bool isSorted = false;
	do {
		//Split
		DataBase tmp[2];
		int p = 0;
		tmp[p].begin_ = tmp[p].end_ = this->begin_;
		this->begin_ = this->begin_->next_;
		tmp[1 - p].begin_ = tmp[1 - p].end_ = nullptr;

		while (this->begin_ != nullptr) {
			if (comp(*this->begin_, *tmp[p].end_)) { p = 1 - p; }
			if (tmp[p].begin_ == nullptr) { tmp[p].begin_ = this->begin_; }
			else { tmp[p].end_->next_ = this->begin_; }
			tmp[p].end_ = this->begin_;
			this->begin_ = this->begin_->next_;
		}
		if (tmp[p].end_ != nullptr) { tmp[p].end_->next_ = nullptr; }
		if (tmp[1 - p].end_ != nullptr) { tmp[1 - p].end_->next_ = nullptr; }

		//Merge
		if (tmp[1 - p].begin_ != nullptr) {
			bool s1;
			bool s2;
			p = comp(*tmp[0].begin_, *tmp[1].begin_) ? 0 : 1;
			this->begin_ = this->end_ = tmp[p].begin_;
			tmp[p].begin_ = tmp[p].begin_->next_;
			while (tmp[p].begin_ != nullptr) {
				s1 = comp(*tmp[p].begin_, *this->end_);
				s2 = comp(*tmp[1 - p].begin_, *this->end_);
				if (s1 == s2) { p = comp(*tmp[0].begin_, *tmp[1].begin_) ? 0 : 1; }
				else if (s1) { p = 1 - p; }

				this->end_->next_ = tmp[p].begin_;
				this->end_ = this->end_->next_;
				tmp[p].begin_ = tmp[p].begin_->next_;
			}
			this->end_->next_ = tmp[1 - p].begin_;
			this->end_ = tmp[1 - p].end_;
			tmp[1 - p].begin_ = tmp[1 - p].end_ = nullptr;
		}
		else {
			isSorted = true;
			this->begin_ = tmp[p].begin_;
			this->end_ = tmp[p].end_;
			tmp[p].begin_ = tmp[p].end_ = nullptr;
		}
	} while (!isSorted);
}
void DataBase::executeCommand(std::istream& in, std::ostream& out) {
	while (true) {
		out << "> ";
		char buffer[32];
		in >> buffer;

		if (strcmp(buffer, "load") == 0) {
			char filename[32];
			in >> filename;
			std::ifstream inFile(filename);

			if (!inFile.is_open()) {
				out << "Can not open database" << std::endl;
				continue;
			}

			this->load(inFile);
			inFile.close();
		}
		else if (strcmp(buffer, "save") == 0) {
			char filename[32];
			in >> filename;
			std::ofstream outFile(filename);

			if (!outFile.is_open()) {
				out << "Can not write to file" << std::endl;
				continue;
			}

			this->save(outFile);
			outFile.close();
		}
		else if (strcmp(buffer, "add") == 0) {
			Stock* stock = new Stock();
			try {
				in >> *stock;
			}
			catch (WrongFormatDate& ex) {
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				out << "Wrong date format: " << ex.what() << std::endl;
				continue;
			}
			catch (WrongFormatTime& ex) {
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				out << "Wrong time format: " << ex.what() << std::endl;
				continue;
			}
			catch (WrongValueTime& ex) {
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				out << "Time value is invalid: " << ex.what() << std::endl;
				continue;
			}
			catch (WrongValueMonth& ex) {
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				out << "Month value is invalid: " << ex.what() << std::endl;
				continue;
			}
			catch (WrongValueDay& ex) {
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				out << "Day value is invalid: " << ex.what() << std::endl;
				continue;
			}
			catch (WrongType& ex) {
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				out << "Unknown command: " << ex.what() << std::endl;
				continue;
			}
			catch (WrongValueYear& ex) {
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				out << "Year value is invalid: " << ex.what() << std::endl;
				continue;
			}
			catch (...) {
				in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				out << "Unknown command" << std::endl;
				continue;
			}
			this->add(*stock);
		}
		else if (strcmp(buffer, "delete") == 0) {
			int ID;
			in >> ID;
			this->deleteID(ID);
		}
		else if (strcmp(buffer, "clear") == 0) {
			this->clear();
		}
		else if (strcmp(buffer, "print") == 0) {
			this->print(out);
		}
		else if (strcmp(buffer, "dom") == 0) {
			char ticker[16];
			in >> ticker;
			this->dom(out, ticker);
		}
		else if (strcmp(buffer, "export") == 0) {
			char filename[32];
			in >> filename;
			std::ofstream outFile(filename);

			if (!outFile.is_open()) {
				out << "Can not write to file" << std::endl;
				continue;
			}

			this->export1(outFile);
			outFile.close();
		}
		else if (strcmp(buffer, "quit") == 0) {
			break;
		}
		else {
			out << "Unknown command: " << buffer << std::endl;
		}
		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}
//Main methods
void DataBase::load(std::ifstream& in) {
	//
	this->destruction();
	//We will add shares until we reach the end of the file
	while (!in.eof()) {
		//At the first entry, begin_ will be equal to nullptr
		if (this->begin_ == nullptr) {
			this->begin_ = this->end_ = new Stock();
		}
		//Otherwise, we record a new action in the end
		else {
			this->end_->next_ = new Stock();
			//Moving the end to the next action after the last one
			this->end_ = this->end_->next_;
		}
		//We use an overloaded operator for stock
		in >> *this->end_;
		//We give an individual number of the promotion
		this->end_->ID_ = ++this->ID_;
		//We are increasing the number of our shares by 1
		++this->count_;
		//Skip all the characters until the next
		//action or until the end of the file
		while (isdigit(in.peek()) == 0 && in.get() != EOF){}
	}
}
void DataBase::save(std::ofstream& out) const {
	//We start with the first element
	Stock* current = this->begin_;
	//until we get to the last one
	while (current != nullptr) {
		out << *current << '\n';
		current = current->next_;
	}
}
void DataBase::add(Stock& stock) {
	//We sort the database as follows:
	//first, purchase orders sorted in descending order of price,
	//and then sales orders sorted in ascending order of price
	this->sort(compareByAdd);
	//Where to start, we will decide depending on the type of action being added
	if (stock.type_) {
		//Adding a purchase => need sales
		Stock* sell = this->begin_;
		//The first sale will be the minimum among all
		while (sell != nullptr && sell->type_) { sell = sell->next_; }
		//We are looking for stocks until the prices have become higher or we are not satisfied
		while (sell != nullptr && sell->price_ <= stock.price_ && stock.amount_ > 0) {
			//Checking for matching tickers
			if (stock.tickerIsEqual(sell->ticker_)) {
				//They can offer us more than we want, so we choose the minimum
				int tmp = (sell->amount_ > stock.amount_) ? stock.amount_ : sell->amount_;
				stock.amount_ -= tmp; sell->amount_ -= tmp;
			}
			//We go to look further
			sell = sell->next_;
		}
	}
	else {
		//Adding a sale => need purchases
		//The first purchase will be the maximum
		Stock* buy = this->begin_;
		//We are looking for stocks until prices have become lower and we are not satisfied
		while (buy != nullptr && buy->type_ && buy->price_ >= stock.price_ && stock.amount_ > 0) {
			//Checking for matching tickers
			if (stock.tickerIsEqual(buy->ticker_)) {
				//They can offer us more than we want, so we choose the minimum
				int tmp = (buy->amount_ > stock.amount_) ? stock.amount_ : buy->amount_;
				stock.amount_ -= tmp; buy->amount_ -= tmp;
			}
			//We go to look further
			buy = buy->next_;
		}
	}
	//We give an individual number of the promotion
	stock.ID_ = ++this->ID_;
	//and we are increasing the number of shares
	++this->count_;
	//Adding our promotion to the end of the list
	//this->end_->next_ = &stock;
	//this->end_ = this->end_->next_;
	if (this->begin_ == nullptr) {
		this->begin_ = this->end_ = &stock;
	}
	else {
		this->end_ = this->end_->next_ = &stock;
	}
}
void DataBase::deleteID(int ID) {
	//Check if the list is empty
	if (this->begin_ == nullptr) { return; }
	//We start with the first element
	Stock* beforeBeingDeleted = this->begin_;
	//The desired item can be at the top of the list
	if (this->begin_->ID_ == ID) {
		this->begin_ = this->begin_->next_;
		delete beforeBeingDeleted;
		--this->count_;
		return;
	}
	//We continue searching until we find the
	//desired item or reach the end of the list
	while (beforeBeingDeleted->next_ != nullptr &&
		beforeBeingDeleted->next_->ID_ != ID) {
		beforeBeingDeleted = beforeBeingDeleted->next_;
	}
	//Asked to delete a non-existent element
	if (beforeBeingDeleted->next_ == nullptr) { return; }
	//Reducing our number of entries
	--this->count_;
	//We "take out" the found element
	Stock* tmp = beforeBeingDeleted->next_->next_;
	//given that the previous one points to it
	delete beforeBeingDeleted->next_;
	beforeBeingDeleted->next_ = tmp;
}
void DataBase::clear() {
	//We are looking for the first relevant promotion
	while (this->begin_ != nullptr && this->begin_->amount_ == 0) {
		Stock* next = this->begin_->next_;
		delete this->begin_;
		this->begin_ = next;
	}
	//We start traversing the entire list from the beginning
	Stock* current = this->begin_;
	//We'll keep going until we reach the end
	while (current != nullptr) {
		//While the next one has zero, we will delete it
		while (current->next_ != nullptr &&
			current->next_->amount_ == 0) {
			--this->count_;
			Stock* tmp = current->next_->next_;
			delete current->next_;
			current->next_ = tmp;
		}
		//Moving on down the list
		current = current->next_;
	}
}
void DataBase::print(std::ostream& out) {
	//First, let's sort the database by time
	this->sort(compareByTime);
	//We start traversing the entire list from the beginning
	Stock* current = this->begin_;
	//Output until we reach the end
	while (current != nullptr) {
		out << *current << '\n';
		current = current->next_;
	}
}
void DataBase::dom(std::ostream& out, const char* ticker) {
	//First we sort the prices in descending order
	//(if the price is equal, first the sales orders ordered in descending order of date and time,
	//and then the purchase orders ordered in ascending order of date and time)
	this->sort(compareByDom);
	//We start traversing the entire list from the beginning
	Stock* current = this->begin_;
	//Output until we reach the end
	while (current != nullptr) {
		//Checking for relevance and matching of the ticker
		if (current->amount_ > 0 && current->tickerIsEqual(ticker)) {
			current->putType(out) << ", ";
			current->putPrice(out) << ", ";
			current->putAmount(out) << ", ";
			current->putDate(out) << ' ';
			current->putTime(out) << '\n';
		}
		current = current->next_;
	}
}
void DataBase::export1(std::ofstream& out) {
	//First, let's sort the database by time
	this->sort(compareByTime);
	//We start traversing the entire list from the beginning
	Stock* current = this->begin_;
	//Output until we reach the end
	while (current != nullptr) {
		out << *current << '\n';
		current = current->next_;
	}
}