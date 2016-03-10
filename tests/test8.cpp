//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Storage.h"

TEST(StorageTest, StorageRetrievals) {

    struct Foo : public Engine::Component<Foo> {
        Foo(): Engine::Component<Foo>() {}
    };

    class FooSystem : public Engine::System {
        std::size_t found;
    public:
        FooSystem(Engine::Storage &storage): Engine::System(storage), found(0) {}

        virtual void update(float f) {
            found = 0;
            _storage.process<Foo>([&](const Engine::Entity &entity) {
                ++found;
            });
        }

        std::size_t getFound() const {
            return found;
        }
    };

    Engine::Storage storage;
    storage.registerComponentKind<Foo>();

    Engine::Entity e1 = storage.addEntity();
    e1.addComponent<Foo>();
    Engine::Entity e2 = storage.addEntity();
    e2.addComponent<Foo>();
    Engine::Entity e3 = storage.addEntity();
    e3.addComponent<Foo>();
    Engine::Entity e4 = storage.addEntity();

    std::size_t result1 = 0, result2 = 0, result3 = 0;
    //using storage system
    FooSystem system = FooSystem(storage);
    system.update(1.0f);
    result1 = system.getFound();

    //using storage iterators
    auto iterBegin = storage.begin<Foo>();
    auto iterEnd = storage.end<Foo>();
    for (; iterBegin != iterEnd; ++iterBegin) {
        ++result2;
    }

    //using storage range
    for (const auto &entity: storage.range<Foo>()) {
        ++result3;
    }

    EXPECT_EQ(result1, 3);
    EXPECT_EQ(result2, 3);
    EXPECT_EQ(result3, 3);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
