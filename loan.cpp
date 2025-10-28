#include "Loan.h"
#include <ctime>
#include <iostream>
#include <chrono>

std::string Loan::getDueDateString() const {
    std::time_t due_time_t = std::chrono::system_clock::to_time_t(dueDate);
    
    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &due_time_t);
#else
    localtime_r(&tm_buf, &due_time_t);
#endif
    std::stringstream ss;
    // Định dạng ngày theo yêu cầu: %d-%m-%Y
    ss << std::put_time(&tm_buf, "%d-%m-%Y");
    return ss.str();
}

json Loan::to_json() const {
    json j;
    j["bookIsbn"] = book->getIsbn();
    j["bookTitle"] = book->getTitle();
    j["memberId"] = member->getId();
    // Lưu thời gian dưới dạng timestamp
    j["borrowDate"] = std::chrono::system_clock::to_time_t(borrowDate);
    j["dueDate"] = std::chrono::system_clock::to_time_t(dueDate);
    j["isReturned"] = isReturned;
    return j;
}

int Loan::getDaysLate() const {
    if (isReturned) return 0;
    auto now = std::chrono::system_clock::now();
    if (now > dueDate) {
        // Tính chênh lệch và làm tròn lên thành ngày
        auto diff = std::chrono::duration_cast<std::chrono::hours>(now - dueDate);
        return (diff.count() / 24) + 1;
    }
    return 0;
}