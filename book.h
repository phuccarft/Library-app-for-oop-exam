#ifndef BOOK_H
#define BOOK_H
#include <string>
#include <sstream>

class Book {
public:
    Book(const std::string& title, const std::string& author, const std::string& isbn, int quantity, const std::string& icon) 
        : title(title), author(author), isbn(isbn), totalQuantity(quantity), availableCopies(quantity), iconName(icon) { 
    }

    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    std::string getIsbn() const { return isbn; }
    
    std::string getIconName() const { return iconName; }
    bool getIsAvailable() const { 
        return availableCopies > 0; 
    }
    int getAvailableCopies() const { return availableCopies; }
    int getTotalQuantity() const { return totalQuantity; }

    bool borrowCopy() {
        if (availableCopies > 0) {
            availableCopies--;
            return true;
        }
        return false;
    }

    void returnCopy() {
        if (availableCopies < totalQuantity) {
            availableCopies++;
        }
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << "Tieu de: " << title 
           << " | Tac gia: " << author 
           << " | ISBN: " << isbn 
           << " | Trang thai: " << availableCopies << "/" << totalQuantity << " co san";
        return ss.str();
    }

private:
    std::string title;
    std::string author;
    std::string isbn;
    int totalQuantity;
    int availableCopies;
    std::string iconName; 
};
#endif