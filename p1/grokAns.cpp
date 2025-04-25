#include <iostream>
#include <memory>
#include <string>
#include <map>

// 前向聲明
class Summoner;

// 能力基類
class Ability {
public:
    virtual void use(Summoner* self, Summoner* opponent) = 0;
    virtual ~Ability() = default;
};

// 燃燒衝擊
class BurningImpact : public Ability {
public:
    void use(Summoner* self, Summoner* opponent) override {
        std::cout << self->name << " uses Burning Impact!" << std::endl;
        int damage = 20; // 基礎傷害
        int burningDamage = 5; // 燃燒傷害（假設觸發）
        int totalDamage = damage + burningDamage;

        // 先扣除護盾
        int remainingDamage = totalDamage;
        if (opponent->shield > 0) {
            int absorbed = std::min(opponent->shield, totalDamage);
            opponent->shield -= absorbed;
            remainingDamage = totalDamage - absorbed;
            std::cout << opponent->name << " absorbs " << absorbed << " damage with shield!" << std::endl;
        }
        // 扣除血量
        opponent->hp -= remainingDamage;
        opponent->hp = std::max(0, opponent->hp); // 確保血量不低於 0
        std::cout << opponent->name << " takes " << damage << " damage + " << burningDamage << " burning damage." << std::endl;
    }
};

// 凍結屏障
class FrozenBarrier : public Ability {
public:
    void use(Summoner* self, Summoner* opponent) override {
        std::cout << self->name << " uses Frozen Barrier!" << std::endl;
        self->shield += 10;
        opponent->isFrozen = true; // 凍結對手
        std::cout << self->name << " gains 10 shield and freezes " << opponent->name << "." << std::endl;
    }
};

// 閃電鏈
class LightningChain : public Ability {
public:
    void use(Summoner* self, Summoner* opponent) override {
        std::cout << self->name << " uses Lightning Chain!" << std::endl;
        opponent->hp -= 15;
        opponent->hp = std::max(0, opponent->hp);
        self->hp -= 5; // 假設觸發連鎖
        self->hp = std::max(0, self->hp);
        std::cout << opponent->name << " takes 15 damage, " << self->name << " takes 5 chain damage." << std::endl;
    }
};

// 元素池
class ElementalPool {
private:
    std::map<std::string, std::shared_ptr<Ability>> abilities;

public:
    ElementalPool() {
        abilities["Fire"] = std::shared_ptr<Ability>(new BurningImpact());
        abilities["Ice"] = std::shared_ptr<Ability>(new FrozenBarrier());
        abilities["Thunder"] = std::shared_ptr<Ability>(new LightningChain());
    }

    std::shared_ptr<Ability> getAbility(const std::string& elementType) {
        auto it = abilities.find(elementType);
        if (it != abilities.end()) {
            return it->second;
        }
        return nullptr;
    }
};

// 精靈
class Spirit {
private:
    std::shared_ptr<Ability> ability;

public:
    Spirit(const std::string& elementType, ElementalPool& pool) {
        ability = pool.getAbility(elementType);
    }

    void useAbility(Summoner* self, Summoner* opponent) {
        if (ability) {
            ability->use(self, opponent);
        }
    }
};

// 召喚師
class Summoner {
public:
    std::string name;
    int hp;
    int energy;
    int shield;
    std::string elementType;
    bool isFrozen;
    std::unique_ptr<Spirit> spirit;
    ElementalPool& pool;

    Summoner(std::string name, int hp, int energy, std::string elementType, ElementalPool& pool)
        : name(name), hp(hp), energy(energy), shield(0), elementType(elementType), isFrozen(false), pool(pool) {}

    bool canSummon() const {
        return energy >= 10 && !isFrozen;
    }

    void summonAndAct(Summoner* opponent) {
        if (!canSummon()) {
            std::cout << name << " cannot act (Energy: " << energy << ", Frozen: " << (isFrozen ? "Yes" : "No") << ")!" << std::endl;
            return;
        }
        energy -= 10;
        spirit = std::unique_ptr<Spirit>(new Spirit(elementType, pool));
        std::cout << name << " summons " << elementType << " Spirit (Energy: " << (energy + 10) << " -> " << energy << ")!" << std::endl;
        spirit->useAbility(this, opponent);
    }

    void recoverEnergy() {
        energy += 5;
    }

    void resetStatus() {
        isFrozen = false;
    }
};

// 對戰系統
class BattleSystem {
private:
    std::unique_ptr<Summoner> s1;
    std::unique_ptr<Summoner> s2;
    ElementalPool pool;

public:
    BattleSystem() {
        s1 = std::unique_ptr<Summoner>(new Summoner("Ashka", 70, 30, "Fire", pool));
        s2 = std::unique_ptr<Summoner>(new Summoner("Frostine", 60, 25, "Ice", pool));
    }

    void printStatus() {
        std::cout << s1->name << " HP: " << s1->hp << ", Energy: " << s1->energy << ", Shield: " << s1->shield << ", Frozen: " << (s1->isFrozen ? "Yes" : "No") << std::endl;
        std::cout << s2->name << " HP: " << s2->hp << ", Energy: " << s2->energy << ", Shield: " << s2->shield << ", Frozen: " << (s2->isFrozen ? "Yes" : "No") << std::endl;
    }

    void startGame() {
        std::cout << "Initial: " << std::endl;
        printStatus();

        // 第一回合
        s1->summonAndAct(s2.get());
        if (s2->hp > 0) {
            s2->summonAndAct(s1.get());
        }

        // 回合結束
        s1->recoverEnergy();
        s2->recoverEnergy();
        s1->resetStatus();
        s2->resetStatus();
        std::cout << "End of turn: " << s1->name << " Energy +5 (" << (s1->energy - 5) << " -> " << s1->energy << "), "
                  << s2->name << " Energy +5 (" << (s2->energy - 5) << " -> " << s2->energy << ")" << std::endl;

        std::cout << "After battle:" << std::endl;
        printStatus();
    }
};

int main() {
    BattleSystem game;
    game.startGame();
    return 0;
}