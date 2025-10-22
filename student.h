#ifndef STUDENT_H
#define STUDENT_H

#include "member.h"
#include <iostream> 

class Student : public Member {
public:
    Student(const std::string& name, const std::string& id, const std::string& email);
    double calculateLateFee(int daysLate) override;
    json to_json() const override;
};

#endif