#ifndef ARRAY_PAGED_LIST_H
#define ARRAY_PAGED_LIST_H

#include <string>

struct ArrayPagedNode {
    std::string id;
    std::string name;
    std::string artist;
    std::string album;
    std::string genre;
    int up_votes;
    int down_votes;
    std::string file_path;
};

class ArrayPagedList {
public:
    ArrayPagedList(int pagesize);
    void insert(const std::string& id, const std::string& name, const std::string& artist, const std::string& album, const std::string& genre, int up_votes, int down_votes, const std::string& file_path);
    void previousPage();
    void nextPage();
    int getTotalPages();
    ArrayPagedNode* getCurrentPage();
    void printCurrentPage();
    ~ArrayPagedList();
    void getPagesize();
    void currentPage();
    int getPageSize() const;
    int getCurrentPageNumber() const;
    ArrayPagedNode* getPage(int pageNumber);




private:
    int pagesize;
    int currentpage;
    int totalPages;
    int currentPageSize;
    ArrayPagedNode** pages;
};

#endif // ARRAY_PAGED_LIST_H
