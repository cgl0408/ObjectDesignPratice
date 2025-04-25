#ifndef _NewReleaseMovie_
#include "Movie.hpp"
class NewReleaseMovie: public Movie{
public:
    // 添加建構子，調用基類建構子
    NewReleaseMovie(int id, std::string title, std::string director, int publishedYear)
        : Movie(id, title, director, publishedYear) {}
    int calculateFine(int y, int mo, int d) override;
    void displayMovieInfo() override;
};
int NewReleaseMovie::calculateFine(int y, int mo, int d){
    tm returnDate = {};
    returnDate.tm_year = y - 1900;
    returnDate.tm_mon = mo - 1;
    returnDate.tm_mday = d;

    time_t returnTime = mktime(&returnDate);
    time_t dueTime = mktime(&expiredDate);

    if (returnTime <= dueTime) return 0;

    double diffDays = difftime(returnTime, dueTime) / (60 * 60 * 24);
    return 2*static_cast<int>(diffDays);  // 每天罰2元
}

void NewReleaseMovie::displayMovieInfo(){
    cout << "New Release Movie: Movie Name = " << title << ", Movie ID = "<< id << "\n"; 
}
#endif