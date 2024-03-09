#include <iostream>
#include <vector>

#include "PaginatedList.cpp"

int main(){


    PaginatedList PaginatedList(3);

    for(int i =0;i<=10;i++) {
        PaginatedList.insert(i);
    }


    PaginatedList.printCurrentPage();


    PaginatedList.nextPage();
    PaginatedList.printCurrentPage();


    PaginatedList.previousPage();
    PaginatedList.printCurrentPage();



    return 0;

}




