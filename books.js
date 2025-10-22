console.log("TỆP BOOKS.JS ĐÃ CHẠY!");
const books_data = [
    {
        "id": "B001",
        "title": "Nhà Giả Kim",
        "author": "Paulo Coelho",
        "isAvailable": true
    },
    {
        "id": "B002",
        "title": "Đắc Nhân Tâm",
        "author": "Dale Carnegie",
        "isAvailable": false
    },
    {
        "id": "B003",
        "title": "Lập trình Hướng đối tượng C++",
        "author": "Khoa CNTT",
        "isAvailable": true
    },
    {
        "id": "B004",
        "title": "Hệ điều hành",
        "author": "Abraham Silberschatz",
        "isAvailable": true
    },
    {
        "id": "B005",
        "title": "Harry Potter và Hòn đá Phù thủy",
        "author": "J.K. Rowling",
        "isAvailable": true
    }
];
function displayBooks(books) {
    const bookListContainer = document.getElementById("book-list-container");
    const feedbackDiv = document.getElementById("feedback-message");
    bookListContainer.innerHTML = "";
    feedbackDiv.innerHTML = "";

    if (books.length === 0) {
        feedbackDiv.textContent = "Không có sách nào trong thư viện.";
        return;
    }
    books.forEach(book => {
        const card = document.createElement("div");
        card.className = "book-card";

        let statusClass = book.isAvailable ? "available" : "unavailable";
        let statusText = book.isAvailable ? "Có sẵn" : "Đã mượn";

        card.innerHTML = `
            <h3>${book.title}</h3>
            <p>Tác giả: ${book.author}</p>
            <p class="book-id">ID: ${book.id}</p>
            <p class="book-status ${statusClass}">${statusText}</p>
        `;
        
        bookListContainer.appendChild(card);
    });
}

document.addEventListener("DOMContentLoaded", () => {
    displayBooks(books_data);
});