#include "httplib.h"
#include "library.h"         
#include "nlohmann/json.hpp" 
#include <iostream>
#include <string>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <thread>   
#include <memory>   

std::string get_due_date(int days_to_add) {
    auto now = std::chrono::system_clock::now();
    auto due_time = now + std::chrono::hours(24 * days_to_add);
    std::time_t due_time_t = std::chrono::system_clock::to_time_t(due_time);
    
    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &due_time_t);
#else
    localtime_r(&tm_buf, &due_time_t);
#endif
    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%d-%m-%Y");
    return ss.str();
}
void run_server(httplib::Server& svr, Library& lib) {
    svr.Post("/borrow-book", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            json data = json::parse(req.body);
            std::string memberName = data["memberName"];
            std::string memberId = data["memberId"];
            std::string memberType = data["memberType"]; 
            std::string memberEmail = data["memberEmail"]; 
            std::string bookId = data["bookId"];

            double baseBorrowFee = 10000.0;
            double lateFeePerDay = 0.0;
            Member* currentMember = nullptr;

            try {
                lib.addMember(memberName, memberId, memberEmail, memberType);
                currentMember = lib.findMemberByID(memberId);
                
                if (currentMember != nullptr) {
                    lateFeePerDay = currentMember->calculateLateFee(1); 
                } else {
                    lateFeePerDay = 1000.0;
                }

                lib.saveMembers();
                
            } catch (std::exception& e) {
                std::cerr << "[Logic Error] Loi khi thuc thi logic: " << e.what() << std::endl;
            }

            std::string borrowDate = get_due_date(0);
            std::string dueDate = get_due_date(14);
            std::cout << "\n========================================";
            std::cout << "\n[GIAO DICH MOI]";
            std::cout << "\n- Thong bao: Da them thanh vien: [" << memberName << "]";
            std::cout << "\n- Dang thanh vien: " << memberType << " - Ma thanh vien: " << memberId;
            std::cout << "\n- Ten sach: " << "Book ID: " + bookId;
            std::cout << "\n- Ngay muon: " << borrowDate;
            std::cout << "\n- Phi tre han: " << lateFeePerDay << " VND/ngay";
            std::cout << "\n========================================\n";
            json response_json;
            response_json["success"] = true;
            response_json["memberName"] = memberName;
            response_json["memberType"] = memberType;
            response_json["bookId"] = bookId;
            response_json["dueDate"] = dueDate;
            response_json["borrowFee"] = std::to_string(static_cast<int>(baseBorrowFee));
            response_json["lateFeeRate"] = std::to_string(static_cast<int>(lateFeePerDay)); 
            res.set_content(response_json.dump(), "application/json");

        } catch (json::parse_error& e) {
            res.status = 400;
            res.set_content("JSON khong hop le: " + std::string(e.what()), "text/plain");
        } catch (std::exception& e) {
            res.status = 500;
            res.set_content("Loi server: " + std::string(e.what()), "text/plain");
        }
    });
    svr.Get("/admin/show-data", [&](const httplib::Request& req, httplib::Response& res) {
         res.set_content("Hay go 'showdata' trong console cua server.", "text/plain");
    });

    std::cout << "May chu Web dang lang nghe tai http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
    std::cout << "May chu Web da dung.\n";
}

void print_data() {
    std::cout << "\n========================================\n";
    std::cout << "[Admin] Yeu cau xem du lieu 'members.json':\n\n";
    
    std::ifstream f("members.json");
    if (!f.is_open()) {
        std::cout << "[Admin] >> Khong the mo tep members.json.\n";
        return;
    }
    std::stringstream buffer;
    buffer << f.rdbuf();
    std::string file_content = buffer.str();
    f.close();

    if (file_content.empty()) {
        std::cout << "[Admin] >> Tep members.json bi trong.\n";
    } else {
        std::cout << file_content;
    }
    std::cout << "\n========================================\n";
}

int main(void) {
    httplib::Server svr;
    Library lib; 
    lib.loadMembers();
    
    svr.set_base_dir(".");
    std::thread server_thread(run_server, std::ref(svr), std::ref(lib));
    server_thread.detach(); 

    std::cout << "May chu da khoi dong.\n";
    std::cout << "Go 'showdata' de xem du lieu, 'exit' de thoat.\n";
    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command); 

        if (command == "showdata") {
            print_data();
        } else if (command == "exit") {
            std::cout << "Dang dung may chu...\n";
            svr.stop(); 
            break; 
        } else if (!command.empty()) {
            std::cout << "Lenh khong hop le. Go 'showdata' hoac 'exit'.\n";
        }
    }

    return 0;
}