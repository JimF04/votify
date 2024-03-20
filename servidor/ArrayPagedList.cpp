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

int ArrayPagedList::getCurrentPageNumber() const {
    return currentpage + 1; // Sumamos 1 porque las páginas se numeran desde 1 en lugar de 0
}

ArrayPagedNode* ArrayPagedList::getPage(int pageNumber) {
    if (pageNumber >= 1 && pageNumber <= totalPages) {
        return pages[pageNumber - 1]; // Restamos 1 porque los índices de las páginas se numeran desde 0
    } else {
        return nullptr; // Devolvemos nullptr si el número de página está fuera de rango
    }
}

void printCurrentPage(ArrayPagedList* playlist) {
    int currentPageNumber = playlist->getCurrentPageNumber();
    if (playlist->getTotalPages() > 0 && currentPageNumber >= 1 && currentPageNumber <= playlist->getTotalPages()) {
        // Obtener la página actual
        ArrayPagedNode* currentPage = playlist->getCurrentPage(); // Use getCurrentPage para obtener la página actual

        // Imprimir la página actual
        std::cout << "Página actual " << currentPageNumber << "/" << playlist->getTotalPages() << std::endl;
        for (int i = 0; i < playlist->getPageSize(); ++i) {
            std::cout << "Id: " << currentPage[i].id << std::endl;
            std::cout << "Nombre: " << currentPage[i].name << std::endl;
            std::cout << "Artista: " << currentPage[i].artist << std::endl;
            std::cout << "Album: " << currentPage[i].album << std::endl;
            std::cout << "Genero: " << currentPage[i].genre << std::endl;
            std::cout << "Votos positivos: " << currentPage[i].up_votes << std::endl;
            std::cout << "Votos negativos: " << currentPage[i].down_votes << std::endl;
            std::cout << "Ruta del archivo: " << currentPage[i].file_path << std::endl;
            std::cout << std::endl;
        }
    } else {
        std::cout << "La página actual no es válida." << std::endl;
    }
}



ArrayPagedList::~ArrayPagedList() {
    for (int i = 0; i < 10; ++i) {
        delete[] pages[i];
    }
    delete[] pages;
}

int ArrayPagedList::getPageSize() const {
    return pagesize;
}

