#include <iostream>
#include "httplib.h"         
#include "nlohmann/json.hpp"  

using namespace httplib;
using json = nlohmann::json;
json books_data = json::array({
    {
        {"id", "B001"},
        {"title", "Nhà Giả Kim"},
        {"author", "Paulo Coelho"},
        {"isAvailable", true}
    },
    {
        {"id", "B002"},
        {"title", "Đắc Nhân Tâm"},
        {"author", "Dale Carnegie"},
        {"isAvailable", false}
    },
    {
        {"id", "B003"},
        {"title", "Lập trình Hướng đối tượng C++"},
        {"author", "Khoa CNTT"},
        {"isAvailable", true}
    },
    {
        {"id", "B004"},
        {"title", "Hệ điều hành"},
        {"author", "Abraham Silberschatz"},
        {"isAvailable", true}
    }
});


int main() {
    Server svr;
    svr.Get("/get-books", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        std::string response_body = books_data.dump();
        res.set_content(response_body, "application/json");

        std::cout << "[GET /get-books] Đã gửi danh sách sách" << std::endl;
    });
    svr.Post("/add-member", [&](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");

        try {
            auto data = json::parse(req.body);
            std::string name = data["name"];
            std::string id = data["id"];
            std::cout << "[POST /add-member] Added member: " << name << " (ID: " << id << ")" << std::endl;
            json response_json = {
                {"message", "Thêm thành viên '" + name + "' thành công!"}
            };
            res.set_content(response_json.dump(), "application/json");

        } catch (json::parse_error& e) {
            res.status = 400; 
            json error_json = { {"message", "Lỗi: JSON không hợp lệ."} };
            res.set_content(error_json.dump(), "application/json");
        }
    });

    svr.Options("/add-member", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        res.status = 204;
    });
    
    svr.Options("/get-books", [](const Request& req, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Access-Control-Allow-Methods", "GET, OPTIONS");
        res.status = 204;
    });

    std::cout << " Library host server run at: http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
    
    return 0;
}