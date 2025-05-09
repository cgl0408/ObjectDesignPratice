#include <iostream>
#include <string>
#include <unordered_map>
#include <semaphore>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>

using namespace std;

// 車輛類型和車位類型
enum class VehicleType { Motorcycle, Compact, Large };
enum class SpotType { Motorcycle, Compact, Large };

// 停車位類
class ParkingSpot {
public:
    int id;
    SpotType type;
    bool occupied = false;

    ParkingSpot(int id, SpotType type) : id(id), type(type) {}
};

// 車輛類
class Vehicle {
public:
    string licensePlate;
    VehicleType type;
    chrono::system_clock::time_point entryTime;

    Vehicle(const string& plate, VehicleType type)
        : licensePlate(plate), type(type), entryTime(chrono::system_clock::now()) {}
};

// 費用計算策略介面
class FeeStrategy {
public:
    virtual int calculateFee(int hours) const = 0;
    virtual ~FeeStrategy() = default;
};

class MotorcycleFee : public FeeStrategy {
public:
    int calculateFee(int hours) const override { return hours * 1; }
};

class CompactFee : public FeeStrategy {
public:
    int calculateFee(int hours) const override { return hours * 2; }
};

class LargeFee : public FeeStrategy {
public:
    int calculateFee(int hours) const override { return hours * 4; }
};

class ParkingSystem {
private:
    vector<ParkingSpot> spots; // 儲存所有車位
    counting_semaphore<1000> motorcycleSpots; // 支援摩托車的車位數
    counting_semaphore<1000> compactSpots;    // 支援小型車的車位數
    counting_semaphore<1000> largeSpots;      // 支援大型車的車位數
    unordered_map<string, pair<int, unique_ptr<Vehicle>>> plateToSpot; // 車牌 -> (車位ID, 車輛)
    shared_mutex rwMtx; // 讀寫鎖保護共享資料
    int spotCounter = 1; // 車位編號生成器

public:
    ParkingSystem(int moto, int compact, int large)
        : motorcycleSpots(moto + compact + large), // 摩托車可停所有車位
          compactSpots(compact + large),          // 小型車可停小型或大型車位
          largeSpots(large) {                     // 大型車只能停大型車位
        // 初始化車位
        for (int i = 0; i < moto; ++i) spots.emplace_back(spotCounter++, SpotType::Motorcycle);
        for (int i = 0; i < compact; ++i) spots.emplace_back(spotCounter++, SpotType::Compact);
        for (int i = 0; i < large; ++i) spots.emplace_back(spotCounter++, SpotType::Large);
    }

    bool parkVehicle(const string& plate, VehicleType type) {
        // 檢查是否已停車
        {
            shared_lock<shared_mutex> readLock(rwMtx);
            if (plateToSpot.count(plate)) {
                cout << "[錯誤] 車牌 " << plate << " 已停車\n";
                return false;
            }
        }

        // 嘗試分配車位
        bool parked = false;
        int spotId = -1;
        if (type == VehicleType::Motorcycle) {
            // 摩托車優先：摩托車位 -> 小型車位 -> 大型車位
            if (motorcycleSpots.try_acquire()) {
                spotId = findSpot(SpotType::Motorcycle);
            } else if (compactSpots.try_acquire()) {
                spotId = findSpot(SpotType::Compact);
            } else if (largeSpots.try_acquire()) {
                spotId = findSpot(SpotType::Large);
            }
            parked = spotId != -1;
        } else if (type == VehicleType::Compact) {
            // 小型車：小型車位 -> 大型車位
            if (compactSpots.try_acquire()) {
                spotId = findSpot(SpotType::Compact);
            } else if (largeSpots.try_acquire()) {
                spotId = findSpot(SpotType::Large);
            }
            parked = spotId != -1;
        } else if (type == VehicleType::Large) {
            // 大型車：僅大型車位
            if (largeSpots.try_acquire()) {
                spotId = findSpot(SpotType::Large);
            }
            parked = spotId != -1;
        }

        if (!parked) {
            cout << "[通知] 車牌 " << plate << " 停車失敗，無空位\n";
            return false;
        }

        // 更新車位和車輛資訊
        {
            lock_guard<shared_mutex> writeLock(rwMtx);
            markSpotOccupied(spotId);
            plateToSpot[plate] = {spotId, make_unique<Vehicle>(plate, type)};
            cout << "[通知] 車牌 " << plate << " 成功停車，位置編號: " << spotId << "\n";
        }
        return true;
    }

    void leaveVehicle(const string& plate) {
        unique_ptr<Vehicle> vehicle;
        int spotId = -1;
        SpotType spotType;

        // 檢查車輛並計算費用
        {
            lock_guard<shared_mutex> writeLock(rwMtx);
            if (!plateToSpot.count(plate)) {
                cout << "[錯誤] 車牌 " << plate << " 未找到\n";
                return;
            }
            spotId = plateToSpot[plate].first;
            vehicle = move(plateToSpot[plate].second);
            plateToSpot.erase(plate);
            spotType = getSpotType(spotId);
        }

        // 釋放車位
        if (spotType == SpotType::Motorcycle || spotType == SpotType::Compact || spotType == SpotType::Large) {
            motorcycleSpots.release(); // 摩托車可停所有車位
        }
        if (spotType == SpotType::Compact || spotType == SpotType::Large) {
            compactSpots.release(); // 小型車可停小型或大型車位
        }
        if (spotType == SpotType::Large) {
            largeSpots.release(); // 大型車僅大型車位
        }
        markSpotFree(spotId);

        // 計算費用
        int hours = chrono::duration_cast<chrono::hours>(
            chrono::system_clock::now() - vehicle->entryTime).count();
        if (hours == 0) hours = 1; // 最少收費 1 小時
        unique_ptr<FeeStrategy> feeStrategy;
        switch (vehicle->type) {
            case VehicleType::Motorcycle: feeStrategy = make_unique<MotorcycleFee>(); break;
            case VehicleType::Compact: feeStrategy = make_unique<CompactFee>(); break;
            case VehicleType::Large: feeStrategy = make_unique<LargeFee>(); break;
        }
        int fee = feeStrategy->calculateFee(hours);

        cout << "[通知] 車牌 " << plate << " 離開，停車 " << hours << " 小時，費用 $" << fee << "\n";
    }

    void searchVehicle(const string& plate) {
        shared_lock<shared_mutex> readLock(rwMtx);
        if (plateToSpot.count(plate)) {
            cout << "[搜尋] 車牌 " << plate << " 在車位 " << plateToSpot[plate].first << "\n";
        } else {
            cout << "[搜尋] 未找到車牌 " << plate << "\n";
        }
    }

    void showAvailableSpots() {
        shared_lock<shared_mutex> readLock(rwMtx);
        int moto = 0, compact = 0, large = 0;
        for (const auto& spot : spots) {
            if (!spot.occupied) {
                if (spot.type == SpotType::Motorcycle) ++moto;
                else if (spot.type == SpotType::Compact) ++compact;
                else if (spot.type == SpotType::Large) ++large;
            }
        }
        cout << "[可用車位] 摩托車位: " << moto << ", 小型車位: " << compact << ", 大型車位: " << large << "\n";
    }

private:
    int findSpot(SpotType type) {
        for (auto& spot : spots) {
            if (!spot.occupied && spot.type == type) {
                return spot.id;
            }
        }
        return -1;
    }

    void markSpotOccupied(int spotId) {
        for (auto& spot : spots) {
            if (spot.id == spotId) {
                spot.occupied = true;
                break;
            }
        }
    }

    void markSpotFree(int spotId) {
        for (auto& spot : spots) {
            if (spot.id == spotId) {
                spot.occupied = false;
                break;
            }
        }
    }

    SpotType getSpotType(int spotId) {
        for (const auto& spot : spots) {
            if (spot.id == spotId) {
                return spot.type;
            }
        }
        return SpotType::Large; // 預設值
    }
};

// 模擬車輛進出
void simulateCar(ParkingSystem& ps, const string& plate, VehicleType type) {
    this_thread::sleep_for(chrono::milliseconds(rand() % 1000));
    if (ps.parkVehicle(plate, type)) {
        this_thread::sleep_for(chrono::milliseconds(1000 + rand() % 2000));
        ps.leaveVehicle(plate);
    }
    ps.searchVehicle(plate);
    ps.showAvailableSpots();
}

int main() {
    ParkingSystem ps(2, 2, 2); // 各類型2個車位

    vector<thread> threads;
    threads.emplace_back(simulateCar, ref(ps), "AAA123", VehicleType::Compact);
    threads.emplace_back(simulateCar, ref(ps), "BBB456", VehicleType::Motorcycle);
    threads.emplace_back(simulateCar, ref(ps), "CCC789", VehicleType::Large);
    threads.emplace_back(simulateCar, ref(ps), "DDD101", VehicleType::Compact);
    threads.emplace_back(simulateCar, ref(ps), "EEE202", VehicleType::Motorcycle);
    threads.emplace_back(simulateCar, ref(ps), "FFF303", VehicleType::Large);

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}