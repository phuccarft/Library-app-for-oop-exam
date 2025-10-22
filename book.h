#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <sstream>

class Book {
public:
    Book(const std::string& title, const std::string& author, const std::string& isbn)
        : title(title), author(author), isbn(isbn), isAvailable(true) {
    }
    std::string getTitle() const { 
        return title; 
    }

    std::string getAuthor() const { 
        return author; 
    }

    std::string getIsbn() const { 
        return isbn; 
    }

    bool getIsAvailable() const { 
        return isAvailable; 
    }
    void setAvailable(bool available) {
        isAvailable = available;
    }
    std::string to_string() const {
        std::stringstream ss;
        ss << "Tieu de: " << title 
           << " | Tac gia: " << author 
           << " | ISBN: " << isbn 
           << " | Trang thai: " << (isAvailable ? "Co san" : "Da muon");
        return ss.str();
    }

private:
    std::string title;
    std::string author;
    std::string isbn;
    bool isAvailable;
};

#endif