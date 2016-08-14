//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, RemoveRestoreEntityHavingComponents) {

    struct Foo {
        Foo() { }
    };

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;

    EntityT e = storage.addEntity();
    e.addComponent<Foo>();

    storage.removeEntity(e);
    EntityT e1 = storage.addEntity();

    std::size_t result = 0;
    auto iterBegin = storage.begin<Foo>();
    auto iterEnd = storage.end<Foo>();
    for (; iterBegin != iterEnd; ++iterBegin) {
        ++result;
    }

    EXPECT_EQ(result, 0);
}

TEST(StorageTest, RemoveAndAndNewEntityWithComponents) {

    struct Foo {
        Foo(int i) : m_i(i) { }

        int getI() const {
            return m_i;
        }

        int m_i;
    };

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;

    EntityT e1 = storage.addEntity();
    e1.addComponent<Foo>(42);
    storage.removeEntity(e1);

    EntityT e2 = storage.addEntity();
    e2.addComponent<Foo>(43);

    std::size_t count = 0;
    for (const auto &e : storage.range<Foo>()) {
        ASSERT_TRUE(e.hasComponent<Foo>());
        ASSERT_TRUE(e.getComponent<Foo>() != nullptr);
        ASSERT_EQ(e.getComponent<Foo>()->getI(), 43);
        ++count;
    }

    EXPECT_EQ(count, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
