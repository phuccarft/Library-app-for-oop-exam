#include "Student.h"
Student::Student(const std::string& name, const std::string& id, const std::string& email)
    : Member(name, id, email) {
}

double Student::calculateLateFee(int daysLate) {
    return daysLate * 1000.0; 
}
json Student::to_json() const {
    json j;
    j["name"] = this->name;
    j["id"] = this->id;
    j["email"] = this->email;
    j["type"] = "student"; 
    return j;
}