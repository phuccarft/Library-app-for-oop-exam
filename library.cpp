#include "library.h"
#include <iomanip>
#include <iostream> 
#include <fstream> 
using namespace std; 

// Triển khai constructor để khởi tạo Catalog và load data (từ bước trước)
Library::Library() : members_filename("members.json") {
    // Tái tạo books_data gốc của bạn và thêm vào Catalog
    catalog.addBook(Book("Lập trình C++ Nâng cao", "Khoa CNTT", "101"));
    catalog.addBook(Book("Cấu trúc dữ liệu và Giải thuật", "Khoa CNTT", "102"));
    catalog.addBook(Book("Thiết kế hệ thống cơ bản", "Khoa CNTT", "103"));
    catalog.addBook(Book("OOP với C++", "Khoa CNTT", "104"));
    catalog.addBook(Book("Một cuốn sách có tên rất dài", "Tác giả X", "105"));
    catalog.addBook(Book("Sách 6", "Tác giả Y", "106"));
}

Library::~Library() {
    for (Member* m : members) { delete m; }
    for (Loan* l : activeLoans) { delete l; }
}

void Library::loadMembers() {
    ifstream f(this->members_filename);
    if (!f.is_open()) {
        cout << "[Storage] Khong tim thay tep '" << this->members_filename << "'. Bat dau moi.\n";
        return;
    }

    try {
        json data = json::parse(f);
        for (Member* m : members) { delete m; }
        members.clear();
        for (const auto& item : data) {
            string type = item.value("type", "");
            string name = item.value("name", "");
            string id = item.value("id", "");
            string email = item.value("email", ""); 

            if (type == "student") {
                members.push_back(new Student(name, id, email)); 
            } else if (type == "teacher") {
                members.push_back(new Teacher(name, id, email));
            }
        }
        cout << "[Storage] Da tai " << members.size() << " thanh vien.\n";
        
    } catch (json::parse_error& e) {
        cerr << "[Storage] Loi doc file JSON: " << e.what() << endl; 
    } catch (std::exception& e) {
        cerr << "[Storage] Loi khi tai thanh vien: " << e.what() << endl;
    }
}

void Library::saveMembers() const {
    json data = json::array();
    for (const Member* m : members) {
        if (m) {
            data.push_back(m->to_json());
        }
    }

    ofstream o(this->members_filename);
    o << setw(4) << data << endl;
    cout << "[Storage] Da luu " << members.size() << " thanh vien.\n"; 
}

Member* Library::findMemberByID(const string& id) const { 
    for (Member* m : members) {
        if (m && m->getId() == id) {
            return m;
        }
    }
    return nullptr;
}

void Library::addMember(const string& name, const string& id, const string& email, const string& type) {
    if (findMemberByID(id) != nullptr) {
        cout << "[Logic] Thanh vien ID: " << id << " da ton tai. Khong them moi.\n";
        return;
    }

    if (type == "student") {
        members.push_back(new Student(name, id, email));
        cout << "[Logic] Da them Student moi: " << name << "\n";
    } else if (type == "teacher") {
        members.push_back(new Teacher(name, id, email));
        cout << "[Logic] Da them Teacher moi: " << name << "\n";
    } else {
        cout << "[Logic Error] Loai thanh vien khong hop le: " << type << "\n";
    }
}

Loan* Library::borrowBook(const string& bookIsbn, const string& memberId) {
    Member* member = findMemberByID(memberId);
    Book* book = catalog.findBookByIsbn(bookIsbn); // <-- Cần hàm findBookByIsbn trong Catalog
    
    if (member == nullptr) {
        cerr << "[Logic Error] Thanh vien ID: " << memberId << " khong ton tai.\n";
        return nullptr;
    }
    
    if (book == nullptr || !book->getIsAvailable()) {
        cerr << "[Logic Error] Sach ISBN: " << bookIsbn << " khong ton tai hoac khong co san.\n";
        return nullptr;
    }
    
    Loan* newLoan = new Loan(book, member);
    activeLoans.push_back(newLoan);
    
    cout << "[Logic] Da tao giao dich muon sach moi cho TV: " << member->getName() << " - Sach: " << book->getTitle() << "\n";
    return newLoan;
}

void Library::saveLoans() const {
    json data = json::array();
    for (const Loan* loan : activeLoans) {
        if (loan) {
            data.push_back(loan->to_json());
        }
    }

    ofstream o("loans.json");
    o << setw(4) << data << endl;
    cout << "[Storage] Da luu " << activeLoans.size() << " giao dich.\n";
}

void Library::loadLoans() {
}
void Library::clearAllMembers() {
    // 1. Giải phóng bộ nhớ (memory cleanup)
    for (Member* m : members) {
        delete m;
    }
    // 2. Xóa tất cả các phần tử khỏi vector
    members.clear();
    
    // 3. Lưu trạng thái trống này lại file
    saveMembers(); 
    
    std::cout << "[ADMIN] Da xoa tat ca thanh vien va luu tep members.json rong.\n";
}

Member* Library::findMemberByCredentials(const std::string& id, const std::string& email) const {
    for (Member* m : members) {
        // Kiểm tra cả ID và Email khớp nhau
        if (m && m->getId() == id) {
             if (m && m->getId() == id) { 
                 return m; 
             }
        }
    }
    return nullptr;
}