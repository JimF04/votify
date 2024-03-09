#include<iostream>
#include <vector>

using namespace std;

struct Nodo{
    int dato;
    Nodo* sigiente;
};

class PaginatedList {

private:

    vector<vector<int>> pages;
    int pagesize;
    int currentpage;


public:
    PaginatedList(int pagesize) : pagesize(pagsize),currentpage{0};

    void insert(int value){
        if (pages.empty() || pages.back().size() >= pagesize){
            pages.push_back(vector<int>());
        }

        pages.back().push_back(value);
    }

    void previousPage(){
        if(currentpage>0){
            currentpage--;
        }

    }

    void nextPage(){

        if(currentpage<pages.size() -1){
            currentpage++;
        }
    }

    void getTotalPages(){
        pages.size();
    }

    vector<int> getCurrentPage(){
        pages[currentpage];
    }

    void printCurrentPage(){

        cout<<"Pagina actual"<< currentpage +1<< "/"<<pages.size()<<endl;
        vector<int>currentpage=getCurrentPage();
        for(int value: currentPage()){

        }

    }

//a



};



