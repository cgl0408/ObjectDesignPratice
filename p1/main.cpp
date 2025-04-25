#include <string>
#include <memory>
#include <iostream>

class Summoner;

// 完整定義 ElementalPool 及子類別
class ElementalPool {
public:
    Summoner* self = nullptr;
    Summoner* oppoenet = nullptr;

    ElementalPool(){};
    void init(Summoner* selt, Summoner* oppoenet){
        this->self = selt;
        this->oppoenet = oppoenet;
    }
    virtual void elementAttack() = 0;
};

// 須完整定義 Spirit（因為會在 Summoner 裡實作）
class Spirit {
public:
    ElementalPool* element = nullptr;
    Spirit(ElementalPool* element): element(element){}
    void attack(){
        element->elementAttack();
    }
};



class Summoner {
public:
    std::unique_ptr<Spirit> spirit;
    ElementalPool* element;
    std::string name, elementType;
    int hp, energy, shield;
    bool canMove;

    Summoner(std::string name, int hp, int energy, std::string elementType, ElementalPool* element);
    void attack();
};





class BurningImpact : public ElementalPool {
    void elementAttack() override {
        int afterShield = 25;
        if(oppoenet->shield > 0){
            oppoenet->shield = std::max(oppoenet->shield - 25, 0);
            afterShield = std::max(25 - oppoenet->shield, 0);
        }
        oppoenet->hp -= afterShield;
    }
};

class FrozenBarrier : public ElementalPool {
    void elementAttack() override {
        self->shield += 10;
        oppoenet->canMove = false;
    }
};

class LightningChain : public ElementalPool {
    void elementAttack() override {
        oppoenet->hp -= 15;
        self->hp -= 5;
    }
};

// 實作 Summoner 建構子與成員函數（因為 Spirit 現在已知）
Summoner::Summoner(std::string name, int hp, int energy, std::string elementType, ElementalPool* element)
    : name(name), hp(hp), energy(energy), elementType(elementType), element(element) {
    if(energy >= 10)
        spirit = std::unique_ptr<Spirit>(new Spirit(element));
    canMove = true;
    shield = 0;
}

void Summoner::attack() {
    if(spirit == nullptr && energy >= 10){
        spirit = std::unique_ptr<Spirit>(new Spirit(element));
    }
    if(spirit != nullptr && canMove){
        spirit->attack();
    } else {
        std::cout << "currently can't move! " << std::endl;
    }
    canMove = true;
    energy += 5;
}

// BattleSystem 不動即可
class BattleSystem {
public:
    std::shared_ptr<Summoner> s1;
    std::shared_ptr<Summoner> s2;
    std::shared_ptr<ElementalPool> burningElement;
    std::shared_ptr<ElementalPool> frozenElement;
    std::shared_ptr<ElementalPool> lightningElement;

    BattleSystem(){
        burningElement = std::make_shared<BurningImpact>();
        s1 = std::make_shared<Summoner>("Ashka", 70, 30, "Fire", burningElement.get());

        frozenElement = std::make_shared<FrozenBarrier>();
        s2 = std::make_shared<Summoner>("Frostine", 60, 25, "Ice", frozenElement.get());

        burningElement->init(s1.get(), s2.get());
        frozenElement->init(s1.get(), s2.get());
    }

    void printBoard(){
        std::cout << s1->name << " hp = " << s1->hp << std::endl;
        std::cout << s2->name << " hp = " << s2->hp << std::endl;
    }

    void startGame(){
        printBoard();
        s1->attack();
        s2->attack();
        printBoard();
    }
};

int main() {
    BattleSystem board;
    board.startGame();
    return 0;
}
