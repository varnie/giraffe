//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, AddNonRegisteredComponentToEntity) {

    struct Foo {
        Foo() { }
    };

    struct Bar {
        Bar() { }
    };

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;
    EntityT e1 = storage.addEntity();
    e1.addComponent<Foo>();

    EntityT e2 = storage.addEntity();

    std::size_t componentFooKindIndex = Giraffe::ComponentsPool<Foo>::index;
    std::size_t componentBarKindIndex = Giraffe::ComponentsPool<Bar>::index;

    std::size_t fooCount = 0;
    for (const auto &entity: storage.range<Foo>()) {
        ++fooCount;
    }

    EXPECT_EQ(fooCount, 1);
    EXPECT_EQ(componentFooKindIndex, 0);
    EXPECT_EQ(componentBarKindIndex, Giraffe::COMPONENT_DOES_NOT_EXIST);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
