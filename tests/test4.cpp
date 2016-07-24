//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, RegisterTheSameComponentInStorage) {

    struct Foo {
        Foo() { }
    };

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;
    //for (int i = 0; i < 10; ++i) {
    //    storage.registerComponentKind<Foo>();
    //}

    std::size_t componentKindIndex = Giraffe::ComponentsPool<Foo>::index;

    EXPECT_EQ(componentKindIndex, 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
