//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Storage.h"

TEST(StorageTest, SystemSearchComponent) {

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
                Foo *pFoo = _storage.getComponent<Foo>(entity);
                (void) pFoo;
                ++found;
            });
        }

        std::size_t getFound() const {
            return found;
        }
    };

    Engine::Storage storage;
    storage.registerComponentKind<Foo>();

    Engine::Entity e = storage.addEntity();
    e.addComponent<Foo>();

    FooSystem system = FooSystem(storage);
    system.update(1.0f);

    EXPECT_EQ(system.getFound(), 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
