//
// Created by varnie on 2/25/16.
//

#include <gtest/gtest.h>
#include "../include/Storage.h"

TEST(StorageTest, EntityRemoveRecreateComponent) {

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
                (void) pFoo; //so that the compiler don't optimize out the line above
                ++found;
            });
        }

        std::size_t getFound() const {
            return found;
        }
    };

    Engine::Storage storage;
    storage.registerComponentKind<Foo>();

    std::size_t poolSize1 = storage.getPoolSize<Foo>();

    Engine::Entity e1 = storage.addEntity();
    e1.addComponent<Foo>();
    e1.removeComponent<Foo>();

    std::size_t poolSize2 = storage.getPoolSize<Foo>();
       
    Engine::Entity e2 = storage.addEntity();
    e2.addComponent<Foo>();

    std::size_t poolSize3 = storage.getPoolSize<Foo>();

    FooSystem system = FooSystem(storage);
    system.update(1.0f);

    ASSERT_EQ(poolSize1, 0);
    ASSERT_EQ(poolSize2, 0);
    ASSERT_EQ(poolSize3, 1);

    EXPECT_EQ(system.getFound(), 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
