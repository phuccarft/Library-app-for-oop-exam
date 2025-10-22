#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"
#include "member.h"
#include "student.h"
#include "teacher.h"
#include "Catalog.h"

using json = nlohmann::json;

class Library {
private:
    std::vector<Member*> members;
    std::string members_filename;
public:
    Library() : members_filename("members.json") {}
    ~Library() {
        for (Member* m : members) {
            delete m;
        }
    }
    void loadMembers();
    void saveMembers() const;
    Member* findMemberByID(const std::string& id) const;
    void addMember(const std::string& name, const std::string& id, const std::string& email, const std::string& type);
};

#endif