//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Storage.h"

TEST(StorageTest, RegisterDifferentComponentsInStorage) {

    struct Foo : public Engine::Component<Foo> {
        Foo(): Engine::Component<Foo>() {}
    };

    struct Bar : public Engine::Component<Bar> {
        Bar(): Engine::Component<Bar>() {}
    };

    Engine::Storage storage;
    storage.registerComponentKind<Foo>();
    storage.registerComponentKind<Bar>();

    std::size_t componentFooKindIndex = Engine::DerivedComponentsPoolNEW<Foo>::index;
    std::size_t componentBarKindIndex = Engine::DerivedComponentsPoolNEW<Bar>::index;

    EXPECT_EQ(componentFooKindIndex, 0);
    EXPECT_EQ(componentBarKindIndex, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
