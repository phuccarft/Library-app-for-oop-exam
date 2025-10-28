console.log('auth.js loaded');

document.addEventListener('DOMContentLoaded', function() {
    const loginForm = document.getElementById('login-form');
    const feedbackDiv = document.getElementById('login-feedback');
    const loginButton = document.getElementById('login-button');

    if (!loginForm) {
        console.error('Login form not found!');
        return;
    }

    if (Auth.isLoggedIn()) {
        console.log('Already logged in, redirecting...');
        window.location.href = '/';
        return;
    }

    loginForm.addEventListener('submit', async function(e) {
        e.preventDefault();
        console.log('Login form submitted');

        const memberId = document.getElementById('login-id').value.trim();
        const memberEmail = document.getElementById('login-email').value.trim();

        console.log('Attempting login with:', { memberId, memberEmail });

        // Validate
        if (!memberId || !memberEmail) {
            showFeedback('Vui lòng nhập đầy đủ ID và Email', 'error');
            return;
        }

        // Disable button
        loginButton.disabled = true;
        loginButton.innerHTML = `<i class='bx bx-loader-alt bx-spin'></i> Đang xác thực...`;
        feedbackDiv.style.display = 'none';

        try {
            const response = await fetch('/check-member', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    memberId: memberId,
                    memberEmail: memberEmail
                })
            });

            console.log('Server response status:', response.status);

            const data = await response.json();
            console.log('Server response data:', data);

            if (response.ok && data.success) {
                // Save session
                Auth.saveUser({
                    id: data.memberId,
                    name: data.memberName,
                    email: data.memberEmail,
                    type: data.memberType
                });

                // Show success
                showFeedback(`✓ Đăng nhập thành công! Chào ${data.memberName}`, 'success');

                // Redirect
                setTimeout(() => {
                    window.location.href = '/';
                }, 1000);
            } else {
                throw new Error(data.message || 'Đăng nhập thất bại');
            }
        } catch (error) {
            console.error('Login error:', error);
            showFeedback(`✗ Lỗi: ${error.message}`, 'error');
            loginButton.disabled = false;
            loginButton.innerHTML = 'LOGIN';
        }
    });

    function showFeedback(message, type) {
        feedbackDiv.textContent = message;
        feedbackDiv.className = `feedback-message ${type}`;
        feedbackDiv.style.display = 'block';
    }
});