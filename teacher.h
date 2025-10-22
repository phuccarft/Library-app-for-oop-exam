#ifndef TEACHER_H
#define TEACHER_H

#include "member.h"
#include <iostream> 

class Teacher : public Member {
public:
    Teacher(const std::string& name, const std::string& id, const std::string& email);
    double calculateLateFee(int daysLate) override;
    json to_json() const override;
};

#endif