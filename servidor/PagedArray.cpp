//
// Created by jimmy on 3/25/24.
//

#include "PagedArray.h"

#include "PagedArray.h"
#include "INIReader.h"

PagedArray::PagedArray(const std::string& iniFilePath) {
    // Leer el archivo .INI y configurar los parámetros
    // Supongamos que se inicializan correctamente pageSize, pageCount y swapFilePath
    // También se inicializa swapFile correctamente
    INIReader reader(iniFilePath);

    pageSize = reader.GetInteger("PagedArray", "pageSize", 0);
    pageCount = reader.GetInteger("PagedArray", "pageCount", 0);
    swapFilePath = reader.Get("PagedArray", "swapFilePath", "");

    pages = new int*[pageCount];
    lruTracker = new int[pageCount];
    for (int i = 0; i < pageCount; ++i) {
        pages[i] = new int[pageSize];
        lruTracker[i] = 0;
    }
}

PagedArray::~PagedArray() {
    for (int i = 0; i < pageCount; ++i) {
        if (isPageLoaded(i)) {
            savePage(i);
            delete[] pages[i];
        }
    }
    delete[] pages;
    delete[] lruTracker;
    swapFile.close();
}

int& PagedArray::operator[](int index) {
    int pageNumber = index / pageSize;
    int pageIndex = index % pageSize;

    if (!isPageLoaded(pageNumber)) {
        loadPage(pageNumber);
    }

    lruTracker[pageNumber] = 0; // Reiniciar contador LRU

    return pages[pageNumber][pageIndex];
}

int PagedArray::size() {
    return pageSize * pageCount;
}

void PagedArray::loadPage(int pageNumber) {
    if (isPageLoaded(pageNumber)) {
        return;
    }

    // Desplazarse a la posición apropiada en el archivo de intercambio
    swapFile.seekg(pageNumber * pageSize * sizeof(int), std::ios::beg);

    // Leer página desde el archivo de intercambio
    swapFile.read(reinterpret_cast<char*>(pages[pageNumber]), pageSize * sizeof(int));
}

void PagedArray::savePage(int pageNumber) {
    // Desplazarse a la posición apropiada en el archivo de intercambio
    swapFile.seekp(pageNumber * pageSize * sizeof(int), std::ios::beg);

    // Escribir página en el archivo de intercambio
    swapFile.write(reinterpret_cast<char*>(pages[pageNumber]), pageSize * sizeof(int));
}

bool PagedArray::isPageLoaded(int pageNumber) {
    return pages[pageNumber] != nullptr;
}

void PagedArray::replaceLRUPage() {
    int lruPage = 0;
    int maxLRU = lruTracker[0];
    for (int i = 1; i < pageCount; ++i) {
        if (lruTracker[i] > maxLRU) {
            lruPage = i;
            maxLRU = lruTracker[i];
        }
    }

    savePage(lruPage);
    delete[] pages[lruPage];
    pages[lruPage] = nullptr;
}
