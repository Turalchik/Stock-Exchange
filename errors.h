#ifndef STOCK_ERROR_2023
#define STOCK_ERROR_2023

#include <iostream>

class WrongFormatDate : public std::invalid_argument {
public:
    WrongFormatDate(const char* out) : std::invalid_argument(out) {}
};

class WrongFormatTime : public std::invalid_argument {
public:
    WrongFormatTime(const char* out) : std::invalid_argument(out) {}
};

class WrongValueTime : public std::invalid_argument {
public:
    WrongValueTime(const char* out) : std::invalid_argument(out) {}
};

class WrongValueYear : public std::invalid_argument {
public:
    WrongValueYear(const char* out) : std::invalid_argument(out) {}
};

class WrongValueMonth : public std::invalid_argument {
public:
    WrongValueMonth(const char* out) : std::invalid_argument(out) {}
};

class WrongValueDay : public std::invalid_argument {
public:
    WrongValueDay(const char* out) : std::invalid_argument(out) {}
};

class WrongType : public std::invalid_argument {
public:
    WrongType(const char* out) : std::invalid_argument(out) {}
};

#endif
