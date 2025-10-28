#ifndef MEMBER_H
#define MEMBER_H

#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class Member {
protected:
    std::string name;
    std::string id;
    std::string email;

public:
    Member(const std::string& name, const std::string& id, const std::string& email)
        : name(name), id(id), email(email) {}
    
    virtual ~Member() {}
    
    virtual double calculateLateFee(int daysLate) = 0;
    virtual json to_json() const = 0;
    
    // Getters
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
};

#endif