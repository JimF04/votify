#ifndef PAGEDARRAY_H
#define PAGEDARRAY_H

#include "playlist.h" // Se asume que la clase Playlist ya está implementada

class PagedArray {
public:
    PagedArray(const string& configFile);
    ~PagedArray();

    int& operator[](int index);

    void addSong(nodo* song);

private:
    bool isPageLoaded(int pageIndex);
    void loadPage(int pageIndex);
    void replacePage(int pageIndex);
    void swapToDisk(int pageIndex);
    void readPageFromDisk(int pageIndex);
    void writePageToDisk(int pageIndex);
    void addNewPage();
    int pageSize;
    string swapPath;
    int* array;
    bool* loadedPages;
    int totalPages;
    int currentPageIndex;
};

#endif
