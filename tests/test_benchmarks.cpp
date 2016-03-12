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
        _start = std::chrono::system_clock::now();
    }

    ~Timer() { }

    void restart() {
        _start = std::chrono::system_clock::now();
    }

    double elapsed() {
        return std::chrono::duration<double>(std::chrono::system_clock::now() - _start).count();
    }

private:
    std::chrono::time_point<std::chrono::system_clock> _start;
};

struct AutoTimer {
public:
    ~AutoTimer() {
        std::cout << timer_.elapsed() << " seconds elapsed" << std::endl;
    }

private:
    Timer timer_;
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
    std::vector<Giraffe::Entity> entities;
    for (std::uint64_t i = 0; i < count; i++) {
        entities.push_back(storage.addEntity());
    }

    AutoTimer t;
    std::cout << "destroying " << count << " entities" << std::endl;

    for (auto e : entities) {
        storage.removeEntity(e);
    }

    ASSERT_TRUE(true);
}

TEST(StorageTest, TestEntityIteration) {

    struct Position : public Giraffe::Component<Position> {
        Position() : Giraffe::Component<Position>() { }
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

    struct Position : public Giraffe::Component<Position> {
        Position() : Giraffe::Component<Position>() { }
    };

    struct Duration : public Giraffe::Component<Duration> {
        Duration() : Giraffe::Component<Duration>() { }
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

    struct Position : public Giraffe::Component<Position> {
        Position() : Giraffe::Component<Position>() { }
    };

    struct Duration : public Giraffe::Component<Duration> {
        Duration() : Giraffe::Component<Duration>() { }
    };

    struct Weight : public Giraffe::Component<Weight> {
        Weight() : Giraffe::Component<Weight>() { }
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
