//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Storage.h"

TEST(StorageTest, RegisterTheSameComponentInStorage) {

    struct Foo : public Engine::Component<Foo> {
        Foo(): Engine::Component<Foo>() {}
    };

    Engine::Storage storage;
    for (int i = 0; i < 10; ++i) {
        storage.registerComponentKind<Foo>();
    }

    std::size_t componentKindIndex = Engine::DerivedComponentsPoolNEW<Foo>::index;

    EXPECT_EQ(componentKindIndex, 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
