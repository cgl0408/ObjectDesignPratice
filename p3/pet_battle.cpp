#include <iostream>
#include <memory>
#include <string>

// 前向聲明
class Pet;

// 技能基類
class Skill {
public:
    virtual void attack(Pet* self, Pet* opponent) = 0;
    virtual void heal(Pet* self) = 0;
    virtual ~Skill() = default;
};

// 寵物類（提前定義，避免不完整類型問題）
class Pet {
public:
    std::string name;
    int hp;
    int attack;
    std::unique_ptr<Skill> skill;

    Pet(std::string name, int hp, int attack, std::unique_ptr<Skill> skill)
        : name(name), hp(hp), attack(attack), skill(std::move(skill)) {}

    bool attackTarget(Pet* opponent) {
        if (hp == 0) return false;
        skill->attack(this, opponent);
        return true;
    }

    bool healSelf() {
        if (hp == 0) return false;
        skill->heal(this);
        return true;
    }
};

// 攻擊技能（現在 Pet 已定義）
class AttackSkill : public Skill {
public:
    void attack(Pet* self, Pet* opponent) override {
        opponent->hp = std::max(0, opponent->hp - self->attack);
    }
    void heal(Pet* self) override {
        // 無治療效果
    }
};

// 治療技能
class HealSkill : public Skill {
public:
    void attack(Pet* self, Pet* opponent) override {
        // 無攻擊效果
    }
    void heal(Pet* self) override {
        int healAmount = self->attack / 2; // 取整，例如 15 / 2 = 7
        self->hp += healAmount;
    }
};

// 戰鬥系統
class BoardingSystem {
private:
    std::unique_ptr<Pet> playA;
    std::unique_ptr<Pet> playB;

public:
    BoardingSystem() {
        playA = std::unique_ptr<Pet>(new Pet("Dragon", 50, 20, std::unique_ptr<Skill>(new AttackSkill())));
        playB = std::unique_ptr<Pet>(new Pet("Phoenix", 40, 15, std::unique_ptr<Skill>(new HealSkill())));
    }

    void startGame() {
        std::cout << "Initial: " << playA->name << " HP=" << playA->hp << ", " << playB->name << " HP=" << playB->hp << std::endl;

        // p1 攻擊 p2
        playA->attackTarget(playB.get());
        // p2 如果活著，執行治療和攻擊
        if (playB->hp > 0) {
            playB->healSelf();
            playB->attackTarget(playA.get());
        }
        // 輸出結果
        std::cout << "After battle:" << std::endl;
        std::cout << playA->name << " HP: " << playA->hp << std::endl;
        std::cout << playB->name << " HP: " << playB->hp << std::endl;
    }

    ~BoardingSystem() {}
};

int main() {
    BoardingSystem game;
    game.startGame();
    return 0;
}