#ifndef CATALOG_H
#define CATALOG_H
#include <string>
#include <vector>
#include "Book.h"

class Catalog {
public:
    void addBook(const Book& book); 
    Book* findBookByIsbn(const std::string& isbn); 
    void displayAllBooks() const; 
    Catalog() {}  
    ~Catalog() {} 


private:
    std::vector<Book> books; 

};

#endif 