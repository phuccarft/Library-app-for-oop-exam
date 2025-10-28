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

    svr.set_base_dir("./FE");

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
    svr.Post("/get-borrow-preview", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            json data = json::parse(req.body);
            std::string memberId = data["memberId"];
            std::string memberType = data["memberType"];
            std::string bookIsbn = data["bookId"];

            // 1. Find the book
            Book* book = lib.findBookByIsbn(bookIsbn);
            if (book == nullptr || !book->getIsAvailable()) {
                res.status = 404;
                json error_json;
                error_json["success"] = false;
                error_json["message"] = "Sach khong ton tai hoac da duoc muon het.";
                res.set_content(error_json.dump(), "application/json");
                return;
            }

            // 2. Find the member (or use temporary info)
            Member* currentMember = lib.findMemberByID(memberId);
            double lateFeePerDay = 1000.0; // Default

            if (currentMember != nullptr) {
                // Member exists, use their fee structure
                lateFeePerDay = currentMember->calculateLateFee(1);
            } else {
                // Member doesn't exist yet, calculate fee based on form type
                if (memberType == "student") {
                    lateFeePerDay = 1000.0; // Student::calculateLateFee(1)
                } else if (memberType == "teacher") {
                    lateFeePerDay = 500.0;  // Teacher::calculateLateFee(1)
                }
            }

            double baseBorrowFee = 10000.0;
            std::string bookTitle = book->getTitle();

            // 3. Send preview data back
            json response_json;
            response_json["success"] = true;
            response_json["bookTitle"] = bookTitle;
            response_json["borrowFee"] = std::to_string(static_cast<int>(baseBorrowFee));
            response_json["lateFeeRate"] = std::to_string(static_cast<int>(lateFeePerDay));
            response_json["availability"] = "Co san (" + std::to_string(book->getAvailableCopies()) + " con lai)";
            res.set_content(response_json.dump(), "application/json");

        } catch (std::exception& e) {
            res.status = 500;
            res.set_content("Loi server: " + std::string(e.what()), "text/plain");
        }
    });

    // OPTIONS handler for the new preview endpoint
    svr.Options("/get-borrow-preview", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.status = 204;
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
            std::string bookIsbn = data["bookId"];

            // 1. Thêm thành viên (nếu chưa tồn tại) và lấy thông tin member
            lib.addMember(memberName, memberId, memberEmail, memberType);
            Member* currentMember = lib.findMemberByID(memberId);
            
            // 2. Thử tạo giao dịch mượn sách
            Loan* newLoan = lib.borrowBook(bookIsbn, memberId);

            if (newLoan == nullptr) {
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
            res.status = 400;
            res.set_content("JSON khong hop le: " + std::string(e.what()), "text/plain");
        } catch (std::exception& e) {
            res.status = 500;
            res.set_content("Loi server: " + std::string(e.what()), "text/plain");
        }
    });
    svr.Post("/return-book", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            json data = json::parse(req.body);
            std::string memberId = data["memberId"];
            std::string bookIsbn = data["bookId"];

            if (memberId.empty() || bookIsbn.empty()) {
                throw std::runtime_error("Member ID and Book ID are required.");
            }

            bool success = lib.returnBook(memberId, bookIsbn);

            if (success) {
                json response_json;
                response_json["success"] = true;
                response_json["message"] = "Sach da duoc tra thanh cong!";
                res.set_content(response_json.dump(), "application/json");
            } else {
                res.status = 404;
                json error_json;
                error_json["success"] = false;
                error_json["message"] = "Khong tim thay giao dich muon phu hop de tra.";
                res.set_content(error_json.dump(), "application/json");
            }

        } catch (std::exception& e) {
            res.status = 500;
            res.set_content("Loi server: " + std::string(e.what()), "text/plain");
        }
    });

    svr.Options("/return-book", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.status = 204;
    });
    svr.Post("/get-my-loans", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            json data = json::parse(req.body);
            std::string memberId = data["memberId"];
            if (memberId.empty()) {
                throw std::runtime_error("Member ID is required.");
            }
            json loansJson = lib.getActiveLoansByMember(memberId);
            res.set_header("Cache-Control", "no-store");
            res.set_content(loansJson.dump(), "application/json");

        } catch (std::exception& e) {
            res.status = 500;
            res.set_content("Loi server: " + std::string(e.what()), "text/plain");
        }
    });
    svr.Options("/get-my-loans", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.status = 204;
    });
    svr.Post("/check-member", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            json data = json::parse(req.body);
            std::string memberId = data["memberId"];
            std::string memberEmail = data["memberEmail"]; 

            // Use the fixed findMemberByCredentials that checks BOTH ID and Email
            Member* foundMember = lib.findMemberByCredentials(memberId, memberEmail);

            if (foundMember != nullptr) {
                // Authentication successful
                json response_json;
                response_json["success"] = true;
                response_json["memberName"] = foundMember->getName();
                response_json["memberId"] = foundMember->getId();
                response_json["memberEmail"] = foundMember->getEmail(); 
                response_json["memberType"] = (dynamic_cast<Student*>(foundMember) != nullptr) 
                    ? "student" : "teacher";
                
                res.set_content(response_json.dump(), "application/json");
                
                std::cout << "[LOGIN SUCCESS] Member: " << foundMember->getName() 
                        << " (ID: " << memberId << ", Email: " << memberEmail << ")\n";
            } else {
                // Authentication failed
                res.status = 401;
                json error_json;
                error_json["success"] = false;
                error_json["message"] = "ID hoac Email khong chinh xac."; 
                res.set_content(error_json.dump(), "application/json");
                
                std::cout << "[LOGIN FAILED] ID: " << memberId 
                        << ", Email: " << memberEmail << " - Not found or mismatch.\n";
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
    // OPTIONS handler cho CORS
    svr.Options("/check-member", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.status = 204;
    });

    svr.Get("/admin/show-data", [&](const Request& req, Response& res) {
         res.set_content("Hay go 'showdata' trong console cua server.", "text/plain");
    });

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
    svr.Get("/session.js", [](const Request&, Response& res) {
    res.set_file_content("session.js", "application/javascript");
    });

    svr.Get("/auth.js", [](const Request&, Response& res) {
    res.set_file_content("auth.js", "application/javascript");
    });
    svr.Get("/get-all-books", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            std::vector<Book> allBooks = lib.getAllBooks();
            json response_json = json::array();

            for (const auto& book : allBooks) {
                response_json.push_back({
                    {"id", book.getIsbn()},
                    {"title", book.getTitle()},
                    {"author", book.getAuthor()},
                    {"iconName", book.getIconName()},
                    {"isAvailable", book.getIsAvailable()},
                    {"availableCopies", book.getAvailableCopies()},
                    {"totalQuantity", book.getTotalQuantity()}
                });
            }
            res.set_content(response_json.dump(), "application/json");
        } catch (std::exception& e) {
            res.status = 500;
            res.set_content(e.what(), "text/plain");
        }
    });

    // OPTIONS handler for the new endpoint
    svr.Options("/get-all-books", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "GET, OPTIONS");
        res.status = 204;
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