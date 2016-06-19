//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, CheckAddingTheSameComponentToEntity) {

    struct Foo {
        Foo() { }
    };

    Giraffe::Storage storage;
    storage.registerComponentKind<Foo>();

    Giraffe::Entity e = storage.addEntity();
    e.addComponent<Foo>();
    e.addComponent<Foo>();

    std::size_t poolSize = storage.getPoolSize<Foo>();

    EXPECT_EQ(poolSize, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
