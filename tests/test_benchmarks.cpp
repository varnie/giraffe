//
// Created by varnie on 2/25/16.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include <iostream>
#include <chrono>
#include "../include/Giraffe.h"

class Timer {
public:
    Timer() {
        m_start = std::chrono::system_clock::now();
    }

    ~Timer() { }

    void restart() {
        m_start = std::chrono::system_clock::now();
    }

    double elapsed() {
        return std::chrono::duration<double>(std::chrono::system_clock::now() - m_start).count();
    }

private:
    std::chrono::time_point<std::chrono::system_clock> m_start;
};

struct AutoTimer {
public:
    ~AutoTimer() {
        std::cout << m_timer.elapsed() << " seconds elapsed" << std::endl;
    }

private:
    Timer m_timer;
};

TEST(StorageTest, TestCreateEntities) {

    Giraffe::Storage storage;
    AutoTimer t;

    std::uint64_t count = 10000000L;
    std::cout << "creating " << count << " entities" << std::endl;

    for (std::uint64_t i = 0; i < count; i++) {
        storage.addEntity();
    }

    ASSERT_TRUE(true);
}

TEST(StorageTest, TestDestroyEntities) {

    Giraffe::Storage storage;

    std::uint64_t count = 10000000L;
    for (std::uint64_t i = 0; i < count; i++) {
        storage.addEntity();
    }

    AutoTimer t;
    std::cout << "destroying " << count << " entities" << std::endl;

    for (const auto &entity : storage.range()) {
        storage.removeEntity(entity);
    }

    ASSERT_TRUE(true);
}

TEST(StorageTest, TestEntityIteration) {

    struct Position {
        Position() { }
    };

    Giraffe::Storage storage;

    int count = 10000000;
    for (int i = 0; i < count; i++) {
        auto e = storage.addEntity();
        e.addComponent<Position>();
    }

    AutoTimer t;
    std::cout << "iterating over " << count << " entities, unpacking one component" << std::endl;

    for (const Giraffe::Entity &e: storage.range<Position>()) {
        auto *pPosition = e.getComponent<Position>();
        (void) pPosition;
        ASSERT_TRUE(pPosition != nullptr);
    }

    ASSERT_TRUE(true);
}

TEST(StorageTest, TestEntityIterationUnpackTwo) {

    struct Position {
        Position() { }
    };

    struct Duration {
        Duration() { }
    };

    Giraffe::Storage storage;

    int count = 10000000;
    for (int i = 0; i < count; i++) {
        auto e = storage.addEntity();
        e.addComponent<Position>();
        e.addComponent<Duration>();
    }

    AutoTimer t;
    std::cout << "iterating over " << count << " entities, unpacking two components" << std::endl;

    for (const Giraffe::Entity &e: storage.range<Position, Duration>()) {
        auto *pPosition = e.getComponent<Position>();
        (void) pPosition;
        ASSERT_TRUE(pPosition != nullptr);

        auto *pDuration = e.getComponent<Duration>();
        (void) pDuration;
        ASSERT_TRUE(pDuration != nullptr);
    }

    ASSERT_TRUE(true);
}

TEST(StorageTest, TestEntityIterationUnpackThree) {

    struct Position {
        Position() { }
    };

    struct Duration {
        Duration() { }
    };

    struct Weight {
        Weight() { }
    };

    Giraffe::Storage storage;

    int count = 10000000;
    for (int i = 0; i < count; i++) {
        auto e = storage.addEntity();
        e.addComponent<Position>();
        e.addComponent<Duration>();
        e.addComponent<Weight>();
    }

    AutoTimer t;
    std::cout << "iterating over " << count << " entities, unpacking three components" << std::endl;

    for (const Giraffe::Entity &e: storage.range<Position, Duration, Weight>()) {
        auto *pPosition = e.getComponent<Position>();
        (void) pPosition;
        ASSERT_TRUE(pPosition != nullptr);

        auto *pDuration = e.getComponent<Duration>();
        (void) pDuration;
        ASSERT_TRUE(pDuration != nullptr);

        auto *pWeight = e.getComponent<Weight>();
        (void) pWeight;
        ASSERT_TRUE(pWeight != nullptr);
    }

    ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
