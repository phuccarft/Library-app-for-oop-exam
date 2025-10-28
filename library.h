#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
#include "member.h"
#include "Student.h"
#include "Teacher.h" 
#include "Catalog.h"
#include "Loan.h" 

using json = nlohmann::json;

class Library {
private:
    std::vector<Member*> members;
    std::string members_filename;
    std::vector<Loan*> activeLoans;
    Catalog catalog;
public:

    Library(); // <-- Constructor để khởi tạo Catalog
    ~Library();
    Member* findMemberByCredentials(const std::string& id, const std::string& email) const;
    void clearAllMembers();
    void loadMembers();
    void saveMembers() const; 
    Member* findMemberByID(const std::string& id) const;
    void addMember(const std::string& name, const std::string& id, const std::string& email, const std::string& type); 

    // Các hàm quản lý Loan
    void loadLoans(); 
    void saveLoans() const;
    Loan* borrowBook(const std::string& bookIsbn, const std::string& memberId);
    Book* findBookByIsbn(const std::string& isbn);
    bool returnBook(const std::string& memberId, const std::string& bookIsbn);
    std::vector<Book> getAllBooks() const;
    json getActiveLoansByMember(const std::string& memberId) const;
};

#endif