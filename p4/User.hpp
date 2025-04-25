#ifndef _USER_

#include "Movie.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
using namespace std;

class User{
public:
    int uid;
    string userName;
    int curRentNum;
    vector<weak_ptr<Movie>> historyRent;
    User(int uid, string userName):uid(uid), userName(userName), curRentNum(0){}
    ~User();
    void rentMovie(shared_ptr<Movie> movie, int y, int mo, int d);
    void returnMovie(int mid);
    void reviewRentHistory();
    void displayUserInfo();
    unordered_map<int, shared_ptr<Movie>> movies;
};

User::~User(){}

void User::rentMovie(shared_ptr<Movie> movie, int y, int mo, int d){
    int mid = movie->id;
    movie->registerMoiveToUser(userName, y, mo, d);
    movies[mid] = move(movie);
    historyRent.push_back(movies[mid]);
    curRentNum+=1;
}

void User::returnMovie(int mid){
    movies[mid]->returnMoive();
    movies.erase(mid);
    curRentNum-=1;
}

void User::reviewRentHistory(){
    for (weak_ptr<Movie>& wp : historyRent) {
        if (auto sp = wp.lock()) {
            sp->displayMovieInfo();  // 安全訪問
        } else {
            cout << "[此電影已刪除]" << endl;
        }
    }

}
void User::displayUserInfo(){
    cout << "ID = " << uid << ", Name = " << userName << "\n";
}

#endif
