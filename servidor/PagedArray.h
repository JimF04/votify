//
// Created by jimmy on 3/25/24.
//

#ifndef PAGEDARRAY_H
#define PAGEDARRAY_H

#include <iostream>
#include <fstream>
#include "playlist.h" // Suponiendo que esta es tu lista circular doblemente enlazada

class PagedArray {
public:
    PagedArray(const std::string& iniFilePath);
    ~PagedArray();

    int& operator[](int index);
    int size();

private:
    void loadPage(int pageNumber);
    void savePage(int pageNumber);
    bool isPageLoaded(int pageNumber);
    void replaceLRUPage();

    std::string swapFilePath;
    int pageSize;
    int pageCount;
    int** pages;
    int* lruTracker;
    std::fstream swapFile;
};

#endif

