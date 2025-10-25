mở my_server.exe để chạy
để my_server.exe chạy nền xong đở mở google r paste: "http://localhost:8080"
là dùng đc
Cách xóa cache:
-mở console trên brower
-nhập lệnh:
fetch('/admin/clear-members', {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json'
    },
    // Body rỗng vì server C++ không yêu cầu dữ liệu nào
    body: JSON.stringify({}) 
})
.then(response => {
    if (response.ok) {
        console.log("XÓA DỮ LIỆU THÀNH CÔNG! members.json đã được đặt lại.");
        alert("XÓA DỮ LIỆU THÀNH CÔNG!");
        return response.json();
    } else {
        console.error("LỖI:", response.status, response.statusText);
        alert("LỖI KHI XÓA DỮ LIỆU. Vui lòng kiểm tra Console Server C++.");
    }
})
.catch(error => {
    console.error('Lỗi mạng:', error);
});
Lệnh tạo sv nếu cần
"g++ server_main.cpp library.cpp Student.cpp Teacher.cpp Catalog.cpp Loan.cpp -o my_server.exe -lws2_32 -lssl -lcrypto -pthread -std=c++17"