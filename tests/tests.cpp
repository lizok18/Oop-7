#include <gtest/gtest.h>
#include "npc.hpp"
#include "knight.hpp"
#include "dragon.hpp"
#include "pegasus.hpp"
#include "factory.hpp"
#include "observer.hpp"
#include <memory>
#include <sstream>

// ==================== ТЕСТЫ ДЛЯ NPC ====================

TEST(NPCTest, CreationAndBasicProperties) {
    Knight knight("Артур", 10.0, 20.0);
    
    EXPECT_EQ(knight.getType(), NPCType::KNIGHT);
    EXPECT_DOUBLE_EQ(knight.getX(), 10.0);
    EXPECT_DOUBLE_EQ(knight.getY(), 20.0);
    EXPECT_EQ(knight.getName(), "Артур");
    EXPECT_TRUE(knight.isAlive());
    EXPECT_EQ(knight.getSymbol(), 'K');
}

TEST(NPCTest, DistanceCalculation) {
    Knight knight1("K1", 0.0, 0.0);
    Knight knight2("K2", 3.0, 4.0);
    
    EXPECT_DOUBLE_EQ(knight1.distanceTo(&knight2), 5.0);
}

TEST(NPCTest, KillMethod) {
    Knight knight("Рыцарь", 10.0, 20.0);
    
    EXPECT_TRUE(knight.isAlive());
    knight.kill();
    EXPECT_FALSE(knight.isAlive());
}

TEST(NPCTest, SetPosition) {
    Knight knight("Рыцарь", 0.0, 0.0);
    
    knight.setPosition(50.0, 60.0);
    EXPECT_DOUBLE_EQ(knight.getX(), 50.0);
    EXPECT_DOUBLE_EQ(knight.getY(), 60.0);
}

// ==================== ТЕСТЫ ДЛЯ ФАБРИКИ ====================

TEST(FactoryTest, CreateAllTypes) {
    auto knight = Factory::create(NPCType::KNIGHT, "Артур", 10.0, 20.0);
    auto dragon = Factory::create(NPCType::DRAGON, "Дракон", 30.0, 40.0);
    auto pegasus = Factory::create(NPCType::PEGASUS, "Пегас", 50.0, 60.0);
    
    EXPECT_NE(knight, nullptr);
    EXPECT_NE(dragon, nullptr);
    EXPECT_NE(pegasus, nullptr);
    
    EXPECT_EQ(knight->getType(), NPCType::KNIGHT);
    EXPECT_EQ(dragon->getType(), NPCType::DRAGON);
    EXPECT_EQ(pegasus->getType(), NPCType::PEGASUS);
}

// ==================== ТЕСТЫ ДЛЯ НАБЛЮДАТЕЛЕЙ ====================

TEST(ObserverTest, ConsoleObserverOutput) {
    auto consoleObserver = ConsoleObserver::get();
    
    auto knight = std::make_shared<Knight>("Атакующий", 0.0, 0.0);
    auto dragon = std::make_shared<Dragon>("Защитник", 1.0, 1.0);
    
    // Просто проверяем что не падает
    EXPECT_NO_THROW({
        consoleObserver->on_fight(knight, dragon, true);
    });
}

TEST(ObserverTest, FileObserverSingleton) {
    auto fileObserver1 = FileObserver::get();
    auto fileObserver2 = FileObserver::get();
    
    // Должны быть одинаковые указатели (синглтон)
    EXPECT_EQ(fileObserver1, fileObserver2);
}

// ==================== ТЕСТЫ КУБИКОВ ====================

TEST(DiceTest, DiceRollRange) {
    for (int i = 0; i < 100; i++) {
        int roll = NPC::rollDice();
        EXPECT_GE(roll, 1);
        EXPECT_LE(roll, 6);
    }
}

// ==================== ТЕСТЫ ДЛЯ ДВИЖЕНИЯ ====================

TEST(MovementTest, MoveWithinBounds) {
    Knight knight("Рыцарь", 50.0, 50.0);
    
    // Несколько движений
    for (int i = 0; i < 10; i++) {
        double oldX = knight.getX();
        double oldY = knight.getY();
        
        knight.move();
        
        EXPECT_GE(knight.getX(), 0.0);
        EXPECT_LE(knight.getX(), 99.0);
        EXPECT_GE(knight.getY(), 0.0);
        EXPECT_LE(knight.getY(), 99.0);
        
        // Обычно позиция меняется, но не всегда
        // EXPECT_NE(knight.getX(), oldX);
        // EXPECT_NE(knight.getY(), oldY);
    }
}

// ==================== ТЕСТЫ ГРАНИЧНЫХ СЛУЧАЕВ ====================

TEST(EdgeCasesTest, SelfDistance) {
    Knight knight("Рыцарь", 10.0, 20.0);
    
    // Расстояние до себя должно быть 0
    EXPECT_DOUBLE_EQ(knight.distanceTo(&knight), 0.0);
}

TEST(EdgeCasesTest, DeadNPCCannotMove) {
    Knight knight("Рыцарь", 50.0, 50.0);
    double x = knight.getX();
    double y = knight.getY();
    
    knight.kill();
    knight.move(); // Мертвый не должен двигаться
    
    EXPECT_DOUBLE_EQ(knight.getX(), x);
    EXPECT_DOUBLE_EQ(knight.getY(), y);
}

// ==================== ПРОСТЫЕ ТЕСТЫ БОЕВ ====================

TEST(SimpleFightTest, FightMethodExists) {
    auto knight = std::make_shared<Knight>("K", 0.0, 0.0);
    auto dragon = std::make_shared<Dragon>("D", 1.0, 1.0);
    
    // Просто проверяем что метод существует и не падает
    EXPECT_NO_THROW({
        bool result = knight->fight(dragon);
        // Не проверяем результат, только что не падает
    });
}

TEST(SimpleFightTest, AcceptMethodExists) {
    auto knight = std::make_shared<Knight>("K", 0.0, 0.0);
    auto dragon = std::make_shared<Dragon>("D", 1.0, 1.0);
    
    // Просто проверяем что метод существует
    EXPECT_NO_THROW({
        bool result = dragon->accept(knight);
    });
}

// ==================== MAIN ====================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "=== ТЕСТЫ NPC СИСТЕМЫ ===" << std::endl;
    std::cout << "Тестируется: базовый функционал, фабрика, наблюдатели" << std::endl;
    std::cout << "=========================" << std::endl;
    
    return RUN_ALL_TESTS();
}