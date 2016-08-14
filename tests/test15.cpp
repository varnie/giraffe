//
// Created by varnie on 2/25/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, EntityRemoveRecreateComponent) {

    struct Foo {
        Foo() { }
    };

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    class FooSystem : public Giraffe::System<StorageT> {
        std::size_t m_found;
    public:
        FooSystem(StorageT &storage) : Giraffe::System<StorageT>(storage), m_found(0) { }

        virtual void update(float f) {
            m_found = 0;
            m_storage.process<Foo>([&](const EntityT &entity) {
                Foo *pFoo = m_storage.getComponent<Foo>(entity);
                (void) pFoo; //so that the compiler don't optimize out the line above
                ++m_found;
            });
        }

        std::size_t getFound() const {
            return m_found;
        }
    };

    StorageT storage;

    std::size_t poolSize1 = storage.getPoolSize<Foo>();
    EntityT e1 = storage.addEntity();
    e1.addComponent<Foo>();
    e1.removeComponent<Foo>();

    std::size_t poolSize2 = storage.getPoolSize<Foo>();
    EntityT e2 = storage.addEntity();
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
