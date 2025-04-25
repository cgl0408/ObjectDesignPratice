
#include <string>
#include <unordered_map>
#include <memory>
#include "RegularMovie.hpp"
#include "NewReleaseMovie.hpp"
#include "User.hpp"
using namespace std;
class RentalStore{
private:
   unordered_map<int, shared_ptr<Movie>> movies;// input movie id to get movie data structure
   unordered_map<int, unique_ptr<User>> users;
   
public:
    RentalStore(/* args */);
    ~RentalStore();
    bool registerMovie(int id, string type, string title, string director, int publishedYear);
    bool registerUSR(int id, string uName);
    bool returnMovie(int userId, int movieId, int y, int mo, int d);
    bool rentMovie(int userId, int movieId, int y, int mo, int d);
    void showMovieFine(int userId, int movieId, int y, int mo, int d);
    void reviewRentHistory(int userId);
};

RentalStore::RentalStore(/* args */){
}

RentalStore::~RentalStore(){

}
bool RentalStore::registerMovie(int id, string type, string title, string director, int publishedYear){
    if(movies.find(id) != movies.end()){
        return false;
    }
    if(type == "Regular"){
        movies[id] = make_shared<RegularMovie>(id, title, director, publishedYear);
    } else if(type == "New Release"){
        movies[id] = make_shared<NewReleaseMovie>(id, title, director, publishedYear);
    }
    cout << "Added movie : ";
    movies[id]->displayMovieInfo();
    return true;
}

bool RentalStore::registerUSR(int uid, string uName){
    if(users.find(uid) != users.end()){
        return false;
    }
    users[uid] = make_unique<User>(uid, uName);
    cout << "Added user : ";
    users[uid]->displayUserInfo();
    return true;
}


bool RentalStore::returnMovie(int userId, int movieId, int y, int mo, int d) {
    if (users.find(userId) == users.end()) return false;

    User* usr = users[userId].get();  // ✅ 不要移走 ownership！

    if (usr->movies.find(movieId) == usr->movies.end()) return false;

    shared_ptr<Movie> movie = usr->movies[movieId];
    int fine = movie->calculateFine(y, mo, d);

    if (fine > 0) {
        cout << "罰款金額：" << fine << " 元，無法成功歸還。" << endl;
        return false;
    }

    // 歸還成功：將電影從 user 移回 store
    movies[movieId] = movie;
    usr->returnMovie(movieId);

    return true;
}

bool RentalStore::rentMovie(int userId, int movieId, int y, int mo, int d) {
    // 檢查用戶和電影是否存在
    if (users.find(userId) == users.end() || movies.find(movieId) == movies.end()) {
        cout << "User or movie not found.\n";
        return false;
    }

    User* usr = users[userId].get();

    // 檢查用戶是否已達租借上限
    if (usr->curRentNum >= 3) {
        cout << "User has reached the rental limit (3 movies).\n";
        return false;
    }

    // 檢查電影是否已被租借
    if (movies[movieId]->isRented) {
        cout << "Movie is already rented.\n";
        return false;
    }

    // 從 movies 中獲取電影，並讓用戶租借
    shared_ptr<Movie> movie = movies[movieId];
    if (movie->registerMoiveToUser(usr->userName, y, mo, d)) {
        usr->rentMovie(move(movies[movieId]), y, mo, d);  // 直接傳遞 shared_ptr，不使用 move
        if(movies[movieId]){
            cout << " 存在\n";
        } else{
            cout << "不存在\n";
        }
        movies.erase(movieId);
        return true;
    } else{
        cout << "fucking failed\n";
    }

    return false;
}


void RentalStore::showMovieFine(int userId, int movieId, int y, int mo, int d){
    if(movies.find(movieId) == movies.end() || users.find(userId) == users.end()){
        return;
    }
    User* usr = users[userId].get();
    shared_ptr<Movie> movie = usr->movies[movieId];
    int fine = movie->calculateFine(y, mo, d);
    cout << "fine = " << fine << "\n";
}

void RentalStore::reviewRentHistory(int userId){
    if(users.find(userId) == users.end()){
        return;
    }
    User* usr = users[userId].get();
    usr->reviewRentHistory();
}

int main(){
    RentalStore store;
    store.registerMovie(0,"Regular", "The Matrix", "Wachowski Sisters", 1999);
    store.registerMovie(1,"New Release", "Dune: Part Two", "Denis Villeneuve", 2024);
    store.registerUSR(0,"Alice");
    store.rentMovie(0,0,2025,3,27);
    store.rentMovie(0,1,2025,4,27);
    store.returnMovie(0,0,2025,3,29);
    store.reviewRentHistory(0);
    return 0;
}

