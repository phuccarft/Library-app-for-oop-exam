#include "Catalog.h"
#include <iostream>

void Catalog::addBook(const Book& book) {
    books.push_back(book);
}

Book* Catalog::findBookByIsbn(const std::string& isbn) {
    for (auto& book : books) { 
        if (book.getIsbn() == isbn) {
            return &book; 
        }
    }
    return nullptr;
}


void Catalog::displayAllBooks() const {
    if (books.empty()) {
        std::cout << "The catalog is currently empty." << std::endl;
        return;
    }

    std::cout << "--- Library Catalog ---" << std::endl;
    for (const Book& book : books) {

        std::cout << book.to_string() << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
}
