#ifndef LOAN_H
#define LOAN_H
#include "Book.h"
#include "Member.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

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
        // 14 ngày mượn sách
        dueDate = borrowDate + (14 * 24h); 
        book->borrowCopy();
    }
    
    std::string getDueDateString() const;
    json to_json() const; 

    void returnBook() {
        isReturned = true;
        book->returnCopy();
    }
    int getDaysLate() const; // Tính ngày trả trễ (để tính phí)
};

#endif