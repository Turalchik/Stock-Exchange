#include <iostream>
#include "database.h"

int main(){
    
    DataBase DB;

    DB.executeCommand(std::cin, std::cout);

    return 0;
}
