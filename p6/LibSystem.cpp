#ifndef LIBSYSTEM_H
#define LIBSYSTEM_H

#include <string>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <vector>
#include <utility> // for std::move

using namespace std;

#endif // LIBSYSTEM_H


class Book {
public:
    string title;
    string author;
    int ISBN;
    int copyId;
    bool isAvailable = true;
    Book(string title, string author, int ISBN, int copyId) : title(title), author(author), ISBN(ISBN), copyId(copyId) {}
    Book(/* args */);
    ~Book();
};

Book::Book(/* args */) {}
Book::~Book() {}

class User {
public:
    int userId;
    string name;
    string address;

    User(int uid, string name, string address) : userId(uid), name(std::move(name)), address(std::move(address)) {}
    User(/* args */);
    virtual ~User();
};

User::User(/* args */) {}
User::~User() {}

class LibrarySystem {
public:
    unordered_map<int, unordered_map<int, shared_ptr<Book>>> books; // ISBN -> <copy_book_id, copy_Book>
    unordered_map<int, unique_ptr<User>> users; // userId -> User, could be Reader or Librarian
    unordered_map<int, vector<string>> borrowHistory; // <ISBN, copy_book_id>
    LibrarySystem(/* args */);
    ~LibrarySystem();
    bool addLibrarian(int userId, string name, string address);
    bool addReader(int userId, string name, string address);
    bool removeUser(int userId);
    void showAllBooks();
    int getBookCount(int ISBN);
    void showBorrowHistory(int ISBN);
};

class Reader : public User {
public:
    unordered_map<int, shared_ptr<Book>> borrowedBooks; // copy_book_id -> copy_Book
    Reader(int uid, string name, string address) : User(uid, name, address) {}
    bool borrowBook(int ISBN, int copyId, LibrarySystem& librarySystem); // 確保聲明與定義一致
    bool returnBook(int copyId, LibrarySystem& librarySystem);
};

bool Reader::borrowBook(int ISBN, int copyId, LibrarySystem& librarySystem) {
    // 確保定義與聲明一致
    cout << "Borrowing book with ISBN: " << ISBN << " and copy ID: " << copyId << endl;
    if (librarySystem.books.find(ISBN) == librarySystem.books.end()) {
        cout << "Book not found!" << endl;
        return false;
    }
    if (borrowedBooks.size() >= 5) {
        cout << "You have already borrowed 5 books!" << endl;
        return false;
    }
    if (librarySystem.books[ISBN].find(copyId) == librarySystem.books[ISBN].end()) {
        cout << "Book copy not found!" << endl;
        return false;
    }
    if (!librarySystem.books[ISBN][copyId]->isAvailable) {
        cout << "Book copy is not available!" << endl;
        return false;
    }

    // Borrow the book
    cout << "Borrowing book with copy ID: " << copyId << endl;
    if (borrowedBooks.find(copyId) != borrowedBooks.end()) {
        cout << "You have already borrowed this book!" << endl;
        return false;
    }
    // Move the book to borrowed books
    string history = "ISBN = " + to_string(ISBN) + ", Copy ID = " + to_string(copyId) + ", User ID = " + to_string(userId);
    librarySystem.borrowHistory[ISBN].push_back(history);
    borrowedBooks[copyId] = move(librarySystem.books[ISBN][copyId]);
    if (borrowedBooks[copyId]) {
        borrowedBooks[copyId]->isAvailable = false; // Mark as unavailable only after successful move
    }
    cout << "Book borrowed successfully!" << endl;
    cout << "Book title: " << borrowedBooks[copyId]->title << endl; // 修正錯誤存取
    return true;
}

bool Reader::returnBook(int copyId, LibrarySystem& librarySystem) {
    cout << "Returning book with copy ID: " << copyId << endl;
    if (borrowedBooks.find(copyId) == borrowedBooks.end()) {
        cout << "You have not borrowed this book!" << endl;
        return false;
    }

    // 保存必要的數據
    int ISBN = borrowedBooks[copyId]->ISBN;
    string title = borrowedBooks[copyId]->title;

    // 將書籍移回 librarySystem.books
    librarySystem.books[ISBN][copyId] = move(borrowedBooks[copyId]);

    // 確保移動後的指針有效
    if (librarySystem.books[ISBN][copyId]) {
        librarySystem.books[ISBN][copyId]->isAvailable = true;
        cout << "Book returned successfully!" << endl;
        cout << "Book title: " << title << endl;
    } else {
        cout << "Error: Failed to return the book!" << endl;
        return false;
    }

    // 從 borrowedBooks 中移除
    borrowedBooks.erase(copyId);
    cout << "Book removed from borrowed books!" << endl;
    cout << "You have " << borrowedBooks.size() << " books left." << endl;
    return true;
}

class Librarian : public User {
public:
    Librarian(int uid, string name, string address) : User(uid, name, address) {}
    bool addBook(int ISBN, int copyId, string bookName, string author, LibrarySystem& librarySystem);
    bool removeBook(int ISBN, LibrarySystem& librarySystem);
};

bool Librarian::addBook(int ISBN, int copyId, string bookName, string author, LibrarySystem& librarySystem) {
    cout << "Adding book: " << bookName << endl;
    if (librarySystem.books.find(ISBN) == librarySystem.books.end()) {
        librarySystem.books[ISBN] = unordered_map<int, shared_ptr<Book>>();
    }
    if (librarySystem.books[ISBN].find(copyId) != librarySystem.books[ISBN].end()) {
        cout << "Book already exists!" << endl;
        return false;
    }
    // Create a new book and add it to the library system
    librarySystem.books[ISBN][copyId] = make_shared<Book>(bookName, author, ISBN, copyId);
    return true;
}

bool Librarian::removeBook(int ISBN, LibrarySystem& librarySystem) {
    if (librarySystem.books.find(ISBN) == librarySystem.books.end()) {
        cout << "Book not found!" << endl;
        return false;
    }
    cout << "Removing book: " << ISBN << endl;
    librarySystem.books.erase(ISBN);
    return true;
}



LibrarySystem::LibrarySystem(/* args */) {}
LibrarySystem::~LibrarySystem() {}

bool LibrarySystem::addLibrarian(int userId, string name, string address) {
    if (users.find(userId) != users.end()) {
        cout << "User already exists!" << endl;
        return false;
    }
    users[userId] = make_unique<Librarian>(userId, name, address);
    cout << "Librarian added: " << name << endl;
    return true;
}

bool LibrarySystem::addReader(int userId, string name, string address) {
    if (users.find(userId) != users.end()) {
        cout << "User already exists!" << endl;
        return false;
    }
    users[userId] = make_unique<Reader>(userId, name, address);
    cout << "Reader added: " << name << endl;
    return true;
}

bool LibrarySystem::removeUser(int userId) {
    if (users.find(userId) == users.end()) {
        cout << "User not found!" << endl;
        return false;
    }
    users.erase(userId);
    cout << "User removed: " << userId << endl;
    return true;
}

void LibrarySystem::showAllBooks() {
    cout << "All books in the library:" << endl;
    for (auto& book : books) {
        cout << "ISBN: " << book.first << endl;
        for (auto& copy : book.second) {
            cout << "Copy ID: " << copy.first << ", Title: " << copy.second->title << ", Author: " << copy.second->author << endl;
        }
    }
}

int LibrarySystem::getBookCount(int ISBN) {
    if (books.find(ISBN) == books.end()) {
        cout << "Book not found!" << endl;
        return 0;
    }
    return books[ISBN].size();
}

void LibrarySystem::showBorrowHistory(int ISBN) {
    if (borrowHistory.find(ISBN) == borrowHistory.end()) {
        cout << "No borrow history for this book!" << endl;
        return;
    }
    cout << "Borrow history for book with ISBN: " << ISBN << endl;
    for (const auto& record : borrowHistory[ISBN]) {
        cout << record << endl; // 顯示借閱歷史
    }
}

int main() {

    /*
    unordered_map<int, unordered_map<int, <shared_ptr<copy_Book>>>> books; ISBN -> <copy_book_id, copy_Book>
    if we erase a book, we need to erase the copy of the book

    */
    //librarian->addBook("The Great Gatsby", "F. Scott Fitzgerald");
    //librarian->removeBook("The Great Gatsby");
    //reader->borrowBook("The Great Gatsby");
    //librarian->returnBook("The Great Gatsby");

    LibrarySystem librarySystem;
    librarySystem.addLibrarian(1, "Alice", "123 Main St");
    librarySystem.addReader(2, "Bob", "456 Elm St");
    librarySystem.addReader(3, "Charlie", "789 Oak St");
    librarySystem.addReader(4, "David", "101 Pine St");
    librarySystem.addReader(5, "Eve", "202 Maple St");
    librarySystem.addReader(6, "Frank", "303 Birch St");
    librarySystem.removeUser(6);

    // 檢查 dynamic_cast 是否成功
    Librarian* librarian = dynamic_cast<Librarian*>(librarySystem.users[1].get());
    if (librarian) {
        librarian->addBook(123, 1, "The Great Gatsby", "F. Scott Fitzgerald", librarySystem);
        librarian->addBook(123, 2, "The Great Gatsby", "F. Scott Fitzgerald", librarySystem);
        librarian->addBook(123, 3, "The Great Gatsby", "F. Scott Fitzgerald", librarySystem);
        librarian->addBook(123, 4, "The Great Gatsby", "F. Scott Fitzgerald", librarySystem);
        librarian->addBook(123, 5, "The Great Gatsby", "F. Scott Fitzgerald", librarySystem);
    } else {
        cout << "User with ID 1 is not a Librarian!" << endl;
    }

    Reader* reader = dynamic_cast<Reader*>(librarySystem.users[2].get());
    if (reader) {
        reader->borrowBook(123, 1, librarySystem);
        reader->returnBook(1, librarySystem);
    } else {
        cout << "User with ID 2 is not a Reader!" << endl;
    }

    librarySystem.showAllBooks();
    librarySystem.showBorrowHistory(123);
    librarySystem.removeUser(2);
    librarySystem.removeUser(3);

    return 0;
}