#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <unordered_map>
#include <algorithm>
#include <memory> // 新增：包含 std::unique_ptr

using namespace std;
//g++ -std=c++17 grokAns.cpp -o grokAns
// 計算時間差（簡單模擬，假設以天為單位）
int daysSince(time_t start) {
    time_t now = time(0);
    return (now - start) / (60 * 60 * 24);
}

// LibraryItem 抽象類別
class LibraryItem {
public:
    string title;
    string author;
    int publicationYear;
    string id;
    bool isBorrowed;
    string borrowedBy;
    time_t borrowStartTime;
    int borrowDuration;

public:
    //避免先默認構造再賦值的額外開銷。
    LibraryItem(string title, string author, int year, string id, int duration = 30)
        : title(title), author(author), publicationYear(year), id(id),
          isBorrowed(false), borrowedBy(""), borrowStartTime(0), borrowDuration(duration) {}

    virtual ~LibraryItem() = default; // 確保有虛析構函數

    virtual string getDetails() const = 0;

    bool borrow(const string& userId) {
        if (isBorrowed) {
            cout << "Item " << id << " is already borrowed.\n";
            return false;
        }
        isBorrowed = true;
        borrowedBy = userId;
        borrowStartTime = time(0);
        cout << "Item " << id << " borrowed by " << userId << ".\n";
        return true;
    }

    bool returnItem() {
        if (!isBorrowed) {
            cout << "Item " << id << " is not borrowed.\n";
            return false;
        }
        isBorrowed = false;
        borrowedBy = "";
        borrowStartTime = 0;
        cout << "Item " << id << " returned.\n";
        return true;
    }

    int calculateFine() const {
        if (!isBorrowed) return 0;
        int days = daysSince(borrowStartTime);
        if (days > borrowDuration) {
            return (days - borrowDuration) * 1;
        }
        return 0;
    }

    string getId() const { return id; }
    bool getIsBorrowed() const { return isBorrowed; }
    string getBorrowedBy() const { return borrowedBy; }
};

// Book 類別
class Book : public LibraryItem {
public:
    Book(string title, string author, int year, string id)
        : LibraryItem(title, author, year, id, 30) {}

    string getDetails() const override {
        return "Book: " + title + ", Author: " + author + ", Year: " + to_string(publicationYear);
    }
};

// User 抽象類別
class User {
protected:
    string userId;
    string name;
    vector<string> borrowedItems;

public:
    User(string userId, string name) : userId(userId), name(name) {}

    virtual ~User() = default; // 確保有虛析構函數

    virtual bool borrowItem(LibraryItem* item) = 0;
    virtual bool returnItem(LibraryItem* item) = 0;
    virtual void viewBorrowingHistory() const = 0;

    string getUserId() const { return userId; }
    string getName() const { return name; }
};

// RegularUser 類別
class RegularUser : public User {
public:
    RegularUser(string userId, string name) : User(userId, name) {}

    bool borrowItem(LibraryItem* item) override {
        if (borrowedItems.size() >= 5) {
            cout << "Cannot borrow more than 5 items.\n";
            return false;
        }
        if (item->borrow(userId)) {
            borrowedItems.push_back(item->getId());
            return true;
        }
        return false;
    }

    bool returnItem(LibraryItem* item) override {
        if (item->getBorrowedBy() != userId) {
            cout << "This item was not borrowed by " << userId << ".\n";
            return false;
        }
        int fine = item->calculateFine();
        if (fine > 0) {
            cout << "Fine for " << item->getId() << ": " << fine << " dollars.\n";
        }
        if (item->returnItem()) {
            borrowedItems.erase(
                remove(borrowedItems.begin(), borrowedItems.end(), item->getId()),
                borrowedItems.end());
            return true;
        }
        return false;
    }

    void viewBorrowingHistory() const override {
        cout << "Borrowing history for " << name << ":\n";
        for (const auto& itemId : borrowedItems) {
            cout << " - Item ID: " << itemId << "\n";
        }
    }
};

// Library 類別
class Library {
    unordered_map<string, unique_ptr<LibraryItem>> items; // 使用 unique_ptr
    unordered_map<string, unique_ptr<User>> users;       // 使用 unique_ptr

public:
    bool addItem(unique_ptr<LibraryItem> item) {
        if (!item) return false;
        string id = item->getId();
        if (items.find(id) != items.end()) {
            cout << "Error: Item with ID " << id << " already exists.\n";
            return false;
        }
        items[id] = move(item); // 轉移所有權給 unordered_map
        cout << "Added item: " << items[id]->getDetails() << "\n";
        return true;
    }

    void addUser(unique_ptr<User> user) {
        string id = user->getUserId();
        users[id] = move(user); // 轉移所有權給 unordered_map
        cout << "Added user: " << users[id]->getName() << "\n";
    }

    LibraryItem* findItemById(const string& id) {
        auto it = items.find(id);
        if (it != items.end()) return it->second.get(); // 返回原始指針
        return nullptr;
    }

    User* findUserById(const string& id) {
        auto it = users.find(id);
        if (it != users.end()) return it->second.get(); // 返回原始指針
        return nullptr;
    }

    // 不需要顯式析構函數，unique_ptr 會自動清理
};

// 主程式
int main() {
    Library library;

    // 使用 make_unique 創建物件
    auto book1 = make_unique<Book>("The Great Gatsby", "F. Scott Fitzgerald", 1925, "B001");
    library.addItem(move(book1));

    auto user1 = make_unique<RegularUser>("U001", "Alice");
    library.addUser(move(user1));

    User* user = library.findUserById("U001");
    LibraryItem* book = library.findItemById("B001");

    if (user && book) {
        user->borrowItem(book);
        user->viewBorrowingHistory();

        // 模擬逾期：假設借閱開始時間是 40 天前
        book->borrowStartTime = time(0) - 40 * 24 * 60 * 60;
        user->returnItem(book);
    }

    // 不需要手動 delete，unique_ptr 會自動清理
    return 0;
}