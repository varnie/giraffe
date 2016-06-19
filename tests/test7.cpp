//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, EntityRemoveComponent) {

    struct Foo {
        Foo() { }
    };

    class FooSystem : public Giraffe::System {
        std::size_t m_found;
    public:
        FooSystem(Giraffe::Storage &storage) : Giraffe::System(storage), m_found(0) { }

        virtual void update(float f) {
            m_found = 0;
            m_storage.process<Foo>([&](const Giraffe::Entity &entity) {
                Foo *pFoo = m_storage.getComponent<Foo>(entity);
                (void) pFoo;
                ++m_found;
            });
        }

        std::size_t getFound() const {
            return m_found;
        }
    };

    Giraffe::Storage storage;
    storage.registerComponentKind<Foo>();

    Giraffe::Entity e = storage.addEntity();
    e.addComponent<Foo>();
    e.removeComponent<Foo>();

    FooSystem system = FooSystem(storage);
    system.update(1.0f);

    EXPECT_EQ(system.getFound(), 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
