// Hàm chuyển hướng chính, có lưu dữ liệu
const redirectToUserPage = (memberId, memberName, memberEmail, memberType) => {
    try {
        const userData = {
            id: memberId,
            name: memberName,
            email: memberEmail,
            type: memberType,
        };
        localStorage.setItem('currentUser', JSON.stringify(userData));

        const params = new URLSearchParams({
            memberId: memberId,
            memberName: memberName,
            memberEmail: memberEmail,
            memberType: memberType
        });
        
        window.location.href = `/user_page.html?${params.toString()}`; 
        
    } catch (e) {
        console.error("LỖI CHUYỂN HƯỚNG hoặc LOCAL STORAGE:", e);
    }
};

// Hàm xử lý logic đăng nhập
document.addEventListener("DOMContentLoaded", () => {
    const loginForm = document.getElementById('login-form');
    if (!loginForm) return; 

    const feedbackDiv = document.getElementById('login-feedback');
    const loginButton = document.getElementById('login-button');

    loginForm.addEventListener('submit', function(event) {
        event.preventDefault(); 
        
        // Lấy cả hai trường
        const memberId = document.getElementById('login-id').value.trim();
        const memberEmailInput = document.getElementById('login-email').value.trim(); // Lấy giá trị email người dùng nhập

        // Vô hiệu hóa nút và chuẩn bị UI
        loginButton.disabled = true;
        loginButton.innerHTML = `<i class='bx bx-loader-alt bx-spin'></i> Đang xác thực...`;
        feedbackDiv.style.display = 'none';

        if (memberId.length < 1 || memberEmailInput.length < 5) {
            feedbackDiv.textContent = 'Đăng nhập thất bại: Vui lòng nhập ID và Email.';
            // ... (xử lý lỗi UI) ...
            return;
        }
        
        // CHỈ GỬI ID THÀNH VIÊN ĐI VÌ LOGIC C++ CHỈ KIỂM TRA ID
        const dataToSend = { memberId: memberId };

        // GỌI API XÁC THỰC THỰC TẾ
        fetch('/check-member', { 
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(dataToSend)
        })
        .then(response => {
            if (!response.ok) {
                return response.json().then(data => { 
                    throw new Error(data.message); 
                });
            }
            return response.json();
        })
        .then(data => {
            // Xác thực thành công
            // Chuyển hướng và lưu dữ liệu người dùng
            // SỬ DỤNG EMAIL MÀ NGƯỜI DÙNG ĐÃ NHẬP VÀO FORM
            redirectToUserPage(data.memberId, data.memberName, memberEmailInput, data.memberType);
        })
        .catch(error => {
            feedbackDiv.textContent = `Đăng nhập thất bại: ${error.message}`;
            feedbackDiv.className = 'feedback-message error';
            feedbackDiv.style.display = 'block';
            loginButton.disabled = false;
            loginButton.innerHTML = `LOGIN`;
        });
    });
});