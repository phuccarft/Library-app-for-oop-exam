#include "Teacher.h"
Teacher::Teacher(const std::string& name, const std::string& id, const std::string& email)
    : Member(name, id, email) {
}

double Teacher::calculateLateFee(int daysLate) {
    return daysLate * 500.0;
}

json Teacher::to_json() const {
    json j;
    j["name"] = this->name;
    j["id"] = this->id;
    j["email"] = this->email;
    j["type"] = "teacher";
    return j;
}