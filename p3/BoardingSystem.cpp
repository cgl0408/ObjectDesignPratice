#include "BoardingSystem.hpp"
#include <iostream>
BoardingSystem::BoardingSystem(){
    playA = std::make_unique<Pet>("Dragon", 50, 20);
    playB = std::make_unique<Pet>("Phoenix", 15, 15);
}

void BoardingSystem::startGame(){
    playA->attakTarget(playB.get());
    playB->healSelf();
    playB->attakTarget(playA.get());
    std::cout << "After battle:" << std::endl;
    std::cout << playA->name << " hp: " << playA->hp << std::endl;
    std::cout << playB->name << " hp: " << playB->hp << std::endl;
}