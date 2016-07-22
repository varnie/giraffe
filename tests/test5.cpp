//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, RegisterDifferentComponentsInStorage) {

    struct Foo {
        Foo() { }
    };

    struct Bar {
        Bar() { }
    };

    using StorageT = Giraffe::Storage<Foo, Bar>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;
    //storage.registerComponentKind<Foo>();
    //storage.registerComponentKind<Bar>();

    std::size_t componentFooKindIndex = Giraffe::DerivedComponentsPool<Foo>::index;
    std::size_t componentBarKindIndex = Giraffe::DerivedComponentsPool<Bar>::index;

    EXPECT_EQ(componentFooKindIndex, 0);
    EXPECT_EQ(componentBarKindIndex, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
