//
// Created by varnie on 2/25/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, EntityRemoveRecreateComponent) {

    struct Foo : public Giraffe::Component<Foo> {
        Foo() : Giraffe::Component<Foo>() { }
    };

    class FooSystem : public Giraffe::System {
        std::size_t m_found;
    public:
        FooSystem(Giraffe::Storage &storage) : Giraffe::System(storage), m_found(0) { }

        virtual void update(float f) {
            m_found = 0;
            m_storage.process<Foo>([&](const Giraffe::Entity &entity) {
                Foo *pFoo = m_storage.getComponent<Foo>(entity);
                (void) pFoo; //so that the compiler don't optimize out the line above
                ++m_found;
            });
        }

        std::size_t getFound() const {
            return m_found;
        }
    };

    Giraffe::Storage storage;
    storage.registerComponentKind<Foo>();

    std::size_t poolSize1 = storage.getPoolSize<Foo>();
    Giraffe::Entity e1 = storage.addEntity();
    e1.addComponent<Foo>();
    e1.removeComponent<Foo>();

    std::size_t poolSize2 = storage.getPoolSize<Foo>();
    Giraffe::Entity e2 = storage.addEntity();
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
