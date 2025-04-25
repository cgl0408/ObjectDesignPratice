#ifndef _MOVIE_
#define _MOVIE_

#include <string>
#include <ctime>
#include <tuple>
#include <iostream>

using namespace std;

class Movie {
private:
    bool isValidDate(int year, int month, int day); // 日期合法性檢查

public:
    int id;  // 調整順序
    string title, director, rentUsrName;
    int publishedYear;
    bool isRented;
    tm rentDate{}, expiredDate{};

    // 建構子
    Movie(int id, string title, string director, int publishedYear)
        : id(id), title(title), director(director),
          publishedYear(publishedYear), isRented(false),
          rentUsrName("None") {}

    Movie();
    virtual ~Movie() = default;

    bool registerMoiveToUser(const string& usrName, int year, int month, int day);
    void returnMoive();
    virtual int calculateFine(int y, int mo, int d) = 0;
    virtual void displayMovieInfo() = 0;
};

// 預設建構子
Movie::Movie() {}


// 日期合法性檢查
bool Movie::isValidDate(int year, int month, int day) {
    tm t = {};
    t.tm_year = year - 1900;
    t.tm_mon = month - 1;
    t.tm_mday = day;

    time_t raw = mktime(&t);
    if (raw == -1) return false;

    tm* validated = localtime(&raw);
    return (validated->tm_year == t.tm_year &&
            validated->tm_mon == t.tm_mon &&
            validated->tm_mday == t.tm_mday);
}

// 註冊租借行為的方法
bool Movie::registerMoiveToUser(const string& usrName, int y, int mo, int d) {
    if (!isValidDate(y, mo, d)) {
        cerr << "無效日期：" << y << "/" << mo << "/" << d << endl;
        return false;
    }

    rentUsrName = usrName;
    isRented = true;

    rentDate.tm_year = y - 1900;
    rentDate.tm_mon = mo - 1;
    rentDate.tm_mday = d;

    time_t rentTime = mktime(&rentDate);
    time_t expireTime = rentTime + 7 * 24 * 60 * 60; // 加7天, 1秒為基本單位
    expiredDate = *localtime(&expireTime);
    return true;
}

void Movie::returnMoive() {
    rentUsrName = "";
    isRented = false;
    rentDate.tm_year = -1;
    rentDate.tm_mon = -1;
    rentDate.tm_mday = -1;
}

#endif