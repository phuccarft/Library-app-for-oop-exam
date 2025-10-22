#ifndef LOAN_H
#define LOAN_H
#include "Book.h"
#include "Member.h"
#include <chrono> 
#include <string>
#include <sstream>
#include <iomanip> 

class Loan {
public:
    Book* book;     
    Member* member; 
    std::chrono::system_clock::time_point borrowDate;
    std::chrono::system_clock::time_point dueDate;
    bool isReturned;
    Loan(Book* b, Member* m) 
        : book(b), member(m), isReturned(false) {
        
        borrowDate = std::chrono::system_clock::now();
        using namespace std::chrono_literals; 
        dueDate = borrowDate + (14 * 24h); 
        book->setAvailable(false);
    }
    void returnBook() {
        isReturned = true;
        book->setAvailable(true);
    }
    int getDaysLate() const {
        if (isReturned) return 0;

        auto now = std::chrono::system_clock::now();
        if (now > dueDate) {
            auto diff = std::chrono::duration_cast<std::chrono::hours>(now - dueDate);
            return (diff.count() / 24) + 1;
        }
        return 0;
    }
};

#endif
