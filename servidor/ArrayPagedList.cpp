#include <iostream>
#include <string>
#include "ArrayPagedList.h"

using namespace std;

ArrayPagedList::ArrayPagedList(int pagesize) : pagesize(pagesize), currentpage(0), totalPages(0), currentPageSize(0) {
    pages = new ArrayPagedNode*[10]; // Supongamos un máximo de 10 páginas inicialmente
    for (int i = 0; i < 10; ++i) {
        pages[i] = new ArrayPagedNode[pagesize];
    }
}

void ArrayPagedList::insert(const string& id, const string& name, const string& artist, const string& album, const string& genre, int up_votes, int down_votes, const string& file_path) {
    if (currentPageSize >= pagesize) {
        if (totalPages < 10) { // Supongamos un máximo de 10 páginas
            totalPages++;
            currentPageSize = 0;
        }
        else {
            cout << "La lista ha alcanzado el máximo de páginas." << endl;
            return;
        }
    }

    ArrayPagedNode newNode = {id, name, artist, album, genre, up_votes, down_votes, file_path};
    pages[currentpage][currentPageSize++] = newNode;
}

void ArrayPagedList::previousPage() {
    if (currentpage > 0) {
        currentpage--;
    }
}

void ArrayPagedList::nextPage() {
    if (currentpage < totalPages - 1) {
        currentpage++;
    }
}

int ArrayPagedList::getTotalPages() {
    return totalPages;
}

ArrayPagedNode* ArrayPagedList::getCurrentPage() {
    return pages[currentpage];
}

void ArrayPagedList::printCurrentPage() {
    if (currentpage >= 0 && currentpage < totalPages) {
        cout << "Página actual " << currentpage + 1 << "/" << getTotalPages() << endl;
        for (int i = 0; i < currentPageSize; ++i) {
            cout << "Id: " << pages[currentpage][i].id << endl;
            cout << "Nombre: " << pages[currentpage][i].name << endl;
            cout << "Artista: " << pages[currentpage][i].artist << endl;
            cout << "Album: " << pages[currentpage][i].album << endl;
            cout << "Genero: " << pages[currentpage][i].genre << endl;
            cout << "Votos positivos: " << pages[currentpage][i].up_votes << endl;
            cout << "Votos negativos: " << pages[currentpage][i].down_votes << endl;
            cout << "Ruta del archivo: " << pages[currentpage][i].file_path << endl;
            cout << endl;
        }
    } else {
        cout << "La página actual no es válida." << endl;
    }
}

ArrayPagedList::~ArrayPagedList() {
    for (int i = 0; i < 10; ++i) {
        delete[] pages[i];
    }
    delete[] pages;
}
