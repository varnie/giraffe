//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, RegisterTheSameComponentInStorage) {

    struct Foo : public Giraffe::Component<Foo> {
        Foo() : Giraffe::Component<Foo>() { }
    };

    Giraffe::Storage storage;
    for (int i = 0; i < 10; ++i) {
        storage.registerComponentKind<Foo>();
    }

    std::size_t componentKindIndex = Giraffe::DerivedComponentsPool<Foo>::index;

    EXPECT_EQ(componentKindIndex, 0);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
