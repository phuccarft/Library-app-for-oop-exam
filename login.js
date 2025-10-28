document.addEventListener("DOMContentLoaded", () => {
    if (SessionManager.isLoggedIn()) {
        window.location.href = '/';
        return;
    }

    const loginForm = document.getElementById('login-form');
    if (!loginForm) return;

    const feedbackDiv = document.getElementById('login-feedback');
    const loginButton = document.getElementById('login-button');

    loginForm.addEventListener('submit', function(event) {
        event.preventDefault();

        const memberId = document.getElementById('login-id').value.trim();
        const memberEmail = document.getElementById('login-email').value.trim();

        // Disable button and show loading
        loginButton.disabled = true;
        loginButton.innerHTML = `<i class='bx bx-loader-alt bx-spin'></i> Đang xác thực...`;
        feedbackDiv.style.display = 'none';

        // Validate inputs
        if (memberId.length < 1 || memberEmail.length < 5) {
            feedbackDiv.textContent = 'Vui lòng nhập đầy đủ ID và Email.';
            feedbackDiv.className = 'feedback-message error';
            feedbackDiv.style.display = 'block';
            loginButton.disabled = false;
            loginButton.innerHTML = `LOGIN`;
            return;
        }

        // Send login request to server
        fetch('/check-member', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                memberId: memberId,
                memberEmail: memberEmail
            })
        })
        .then(response => {
            if (!response.ok) {
                return response.json().then(data => {
                    throw new Error(data.message || 'Đăng nhập thất bại');
                });
            }
            return response.json();
        })
        .then(data => {
            if (data.success) {
                // Save session data
                SessionManager.saveSession({
                    id: data.memberId,
                    name: data.memberName,
                    email: data.memberEmail,
                    type: data.memberType
                });

                // Show success message briefly
                feedbackDiv.textContent = `Đăng nhập thành công! Chào mừng ${data.memberName}`;
                feedbackDiv.className = 'feedback-message success';
                feedbackDiv.style.display = 'block';

                // Redirect to homepage after 1 second
                setTimeout(() => {
                    window.location.href = '/';
                }, 1000);
            } else {
                throw new Error('Xác thực thất bại');
            }
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