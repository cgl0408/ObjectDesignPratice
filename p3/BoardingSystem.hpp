#ifndef __BOARDING_SYSTEM__
#define __BOARDING_SYSTEM__
#include "Pet.hpp"
#include <memory>
class BoardingSystem
{
private:
    /* data */
    std::unique_ptr<Pet> playA;
    std::unique_ptr<Pet> playB;
public:
    BoardingSystem(/* args */);
    void startGame();
    ~BoardingSystem();
};

#endif