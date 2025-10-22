#include "library.h"
#include <iomanip>

void Library::loadMembers() {
    std::ifstream f(this->members_filename);
    if (!f.is_open()) {
        std::cout << "[Storage] Khong tim thay tep '" << this->members_filename << "'. Bat dau moi.\n";
        return;
    }

    try {
        json data = json::parse(f);
        for (Member* m : members) { delete m; }
        members.clear();
        for (const auto& item : data) {
            std::string type = item.value("type", "");
            std::string name = item.value("name", "");
            std::string id = item.value("id", "");
            std::string email = item.value("email", ""); 

            if (type == "student") {
                members.push_back(new Student(name, id, email)); 
            } else if (type == "teacher") {
                members.push_back(new Teacher(name, id, email));
            }
        }
        std::cout << "[Storage] Da tai " << members.size() << " thanh vien.\n";
        
    } catch (json::parse_error& e) {
        std::cerr << "[Storage] Loi doc file JSON: " << e.what() << std::endl;
    } catch (std::exception& e) {
        std::cerr << "[Storage] Loi khi tai thanh vien: " << e.what() << std::endl;
    }
}

void Library::saveMembers() const {
    json data = json::array();
    for (const Member* m : members) {
        if (m) {
            data.push_back(m->to_json());
        }
    }

    std::ofstream o(this->members_filename);
    o << std::setw(4) << data << std::endl;
    std::cout << "[Storage] Da luu " << members.size() << " thanh vien.\n";
}

Member* Library::findMemberByID(const std::string& id) const {
    for (Member* m : members) {
        if (m && m->getId() == id) {
            return m;
        }
    }
    return nullptr;
}

void Library::addMember(const std::string& name, const std::string& id, const std::string& email, const std::string& type) {
    if (findMemberByID(id) != nullptr) {
        std::cout << "[Logic] Thanh vien ID: " << id << " da ton tai. Khong them moi.\n";
        return;
    }

    if (type == "student") {
        members.push_back(new Student(name, id, email));
        std::cout << "[Logic] Da them Student moi: " << name << "\n";
    } else if (type == "teacher") {
        members.push_back(new Teacher(name, id, email));
        std::cout << "[Logic] Da them Teacher moi: " << name << "\n";
    } else {
        std::cout << "[Logic Error] Loai thanh vien khong hop le: " << type << "\n";
    }
}
