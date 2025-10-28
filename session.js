const Auth = {
    STORAGE_KEY: 'library_user',

    saveUser(userData) {
        try {
            localStorage.setItem(this.STORAGE_KEY, JSON.stringify(userData));
            console.log('✓ User session saved:', userData.name);
            return true;
        } catch (e) {
            console.error('✗ Failed to save session:', e);
            return false;
        }
    },

    getUser() {
        try {
            const data = localStorage.getItem(this.STORAGE_KEY);
            return data ? JSON.parse(data) : null;
        } catch (e) {
            console.error('✗ Failed to get session:', e);
            return null;
        }
    },

    isLoggedIn() {
        return this.getUser() !== null;
    },

    logout() {
        localStorage.removeItem(this.STORAGE_KEY);
        console.log('✓ User logged out');
    },

    updateNavbar() {
        const user = this.getUser();
        const loginBtn = document.querySelector('.navbar-login-btn');
        
        if (loginBtn && user) {
            loginBtn.innerHTML = `<i class='bx bx-user-circle'></i> ${user.name}`;
            loginBtn.href = '/user_page.html';
            loginBtn.onclick = null;
        }
    }
};

// Auto-update on page load
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', () => Auth.updateNavbar());
} else {
    Auth.updateNavbar();
}