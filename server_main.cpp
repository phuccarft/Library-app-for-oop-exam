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
using namespace httplib;

// Hàm tính ngày hẹn trả
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

void run_server(Server& svr, Library& lib) {
    // ĐỊNH TUYẾN RÕ RÀNG CHO CÁC TỆP TĨNH (STATIC FILES) ĐỂ TRÁNH LỖI 404

    // Trang chủ
    svr.Get("/", [](const Request&, Response& res) {
        res.set_file_content("index.html", "text/html");
    });
    
    // Trang mượn sách
    svr.Get("/borrow.html", [](const Request&, Response& res) {
        res.set_file_content("borrow.html", "text/html");
    });

    // Trang tài khoản người dùng MỚI (Khắc phục lỗi 404)
    svr.Get("/user_page.html", [](const Request&, Response& res) {
        res.set_file_content("user_page.html", "text/html");
    });

    // Các tệp CSS/JS
    svr.Get("/style.css", [](const Request&, Response& res) {
        res.set_file_content("style.css", "text/css");
    });
    svr.Get("/books.js", [](const Request&, Response& res) {
        res.set_file_content("books.js", "application/javascript");
    });

    // Endpoint xử lý form mượn sách
    svr.Post("/borrow-book", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            json data = json::parse(req.body);
            std::string memberName = data["memberName"];
            std::string memberId = data["memberId"];
            std::string memberType = data["memberType"]; 
            std::string memberEmail = data["memberEmail"]; 
            std::string bookIsbn = data["bookId"]; // <-- Đổi tên biến thành bookIsbn

            // 1. Thêm thành viên (nếu chưa tồn tại) và lấy thông tin member
            lib.addMember(memberName, memberId, memberEmail, memberType);
            Member* currentMember = lib.findMemberByID(memberId);
            
            // 2. Thử tạo giao dịch mượn sách
            Loan* newLoan = lib.borrowBook(bookIsbn, memberId);

            if (newLoan == nullptr) {
                // Sách không tồn tại hoặc đã được mượn
                res.status = 400;
                json error_json;
                error_json["success"] = false;
                error_json["message"] = "Sach khong co san hoac ID sach khong hop le.";
                res.set_content(error_json.dump(), "application/json");
                return;
            }

            // 3. Tính toán phí và chuẩn bị phản hồi
            double baseBorrowFee = 10000.0;
            double lateFeePerDay = (currentMember != nullptr) ? currentMember->calculateLateFee(1) : 1000.0; // Phí trễ hạn 1 ngày
            
            // 4. Lưu lại các thay đổi
            lib.saveMembers(); 
lib.saveLoans();
            
            // 5. Gửi phản hồi thành công và IN RA CONSOLE THEO ĐỊNH DẠNG BẠN MUỐN
            std::string borrowDate = get_due_date(0);
            std::string bookTitle = newLoan->book->getTitle(); 
            
            std::cout << "\n========================================";
            std::cout << "\n[GIAO DICH MOI]";
            std::cout << "\n- Thong bao: Da them thanh vien: [" << memberName << "]";
            std::cout << "\n- Dang thanh vien: " << memberType << " - Ma thanh vien: " << memberId;
            std::cout << "\n- Ten sach: " << bookTitle; 
            std::cout << "\n- Ngay muon: " << borrowDate;
            std::cout << "\n- Phi tre han: " << lateFeePerDay << " VND/ngay";
            std::cout << "\n========================================\n";

            // ... (Phần tạo JSON response_json gửi về client) ...
            json response_json;
            response_json["success"] = true;
            response_json["memberName"] = memberName;
            response_json["memberType"] = memberType;
            response_json["bookId"] = bookIsbn;
            response_json["bookTitle"] = bookTitle;
            response_json["dueDate"] = newLoan->getDueDateString();
            response_json["borrowFee"] = std::to_string(static_cast<int>(baseBorrowFee));
            response_json["lateFeeRate"] = std::to_string(static_cast<int>(lateFeePerDay)); 
            res.set_content(response_json.dump(), "application/json");

        } catch (json::parse_error& e) {
            // ... (Logic xử lý lỗi) ...
            res.status = 400;
            res.set_content("JSON khong hop le: " + std::string(e.what()), "text/plain");
        } catch (std::exception& e) {
            res.status = 500;
            res.set_content("Loi server: " + std::string(e.what()), "text/plain");
        }
    });
    svr.Post("/check-member", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            json data = json::parse(req.body);
            std::string memberId = data["memberId"];


            // 1. Tìm thành viên chỉ bằng ID
            Member* foundMember = lib.findMemberByID(memberId); 

            if (foundMember != nullptr) {
                // Đã tìm thấy thành viên (Xác thực thành công)
                json response_json;
                response_json["success"] = true;
                response_json["memberName"] = foundMember->getName();
                response_json["memberId"] = foundMember->getId();
                
                // GIẢ ĐỊNH EMAIL TẠM THỜI VÌ CHÚNG TA KHÔNG DÙNG NÓ ĐỂ XÁC THỰC
                // Bạn nên sửa lại Member class để có getEmail()
                std::string defaultEmail = foundMember->getName() + "@library.com"; 
                
                response_json["memberEmail"] = defaultEmail; 
                response_json["memberType"] = (dynamic_cast<Student*>(foundMember) != nullptr) ? "student" : "teacher";
                
                res.set_content(response_json.dump(), "application/json");

                std::cout << "[LOGIN SUCCESS] Thanh vien: " << foundMember->getName() << " da dang nhap.\n";
            } else {
                // Xác thực thất bại
                res.status = 401; 
                json error_json;
                error_json["success"] = false;
                error_json["message"] = "Ma thanh vien khong chinh xac.";
                res.set_content(error_json.dump(), "application/json");
                
                std::cout << "[LOGIN FAILED] ID: " << memberId << " - Khong tim thay.\n";
            }
        } catch (json::parse_error& e) {
            res.status = 400;
            res.set_content("JSON khong hop le.", "text/plain");
        } catch (std::exception& e) {
            res.status = 500;
            res.set_content("Loi server: " + std::string(e.what()), "text/plain");
        }
    });
    svr.Post("/admin/clear-members", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            // Kích hoạt chức năng xóa thành viên
            lib.clearAllMembers();

            json response_json;
            response_json["success"] = true;
            response_json["message"] = "Tat ca du lieu thanh vien da duoc xoa thanh cong!";
            
            res.set_content(response_json.dump(), "application/json");

        } catch (std::exception& e) {
            res.status = 500;
            res.set_content("Loi server khi xoa du lieu: " + std::string(e.what()), "text/plain");
        }
    });

    //OPTIONS handler cho endpoint mới này để tránh lỗi CORS
    svr.Options("/admin/clear-members", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.status = 204;
    });
    // Thêm OPTIONS handler cho CORS
    svr.Options("/check-member", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.status = 204;
    });
    //...
    // Giữ nguyên các định tuyến khác
    svr.Get("/admin/show-data", [&](const Request& req, Response& res) {
         res.set_content("Hay go 'showdata' trong console cua server.", "text/plain");
    });
    // ... (trong hàm run_server)

    // Trang đăng nhập mới
    svr.Get("/login_page.html", [](const Request&, Response& res) {
        res.set_file_content("login_page.html", "text/html");
    });
    
    // Trang chủ
    svr.Get("/", [](const Request&, Response& res) {
        res.set_file_content("index.html", "text/html");
    });
    
    // Trang mượn sách
    svr.Get("/borrow.html", [](const Request&, Response& res) {
        res.set_file_content("borrow.html", "text/html");
    });

    // Trang tài khoản người dùng
    svr.Get("/user_page.html", [](const Request&, Response& res) {
        res.set_file_content("user_page.html", "text/html");
    });


    std::cout << "May chu Web dang lang nghe tai http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
    std::cout << "May chu Web da dung.\n";
}

// Hàm print_data
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
    Server svr; // Sử dụng lớp Server từ namespace httplib
    Library lib; 
    lib.loadMembers();
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