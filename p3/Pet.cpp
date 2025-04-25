#include "Pet.hpp"
Pet::Pet(std::string name, int hp, int attack): name(name), hp(hp), attack(attack){}

bool Pet::attakTarget(Pet* opponent){
    if(hp == 0) return false;
    opponent->hp = std::max(attackSkill->attack(this, opponent), 0);
    return true;
}

bool Pet::healSelf(){
    if(hp == 0) return false;
    healSkill->heal(this);
    return true;
}
 