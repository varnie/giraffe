//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, RemoveRestoreEntityHavingComponents) {

    struct Foo : public Giraffe::Component<Foo> {
        Foo(): Giraffe::Component<Foo>() {}
    };

    Giraffe::Storage storage;
    storage.registerComponentKind<Foo>();

    Giraffe::Entity e = storage.addEntity();
    e.addComponent<Foo>();

    storage.removeEntity(e);
    Giraffe::Entity e1 = storage.addEntity();

    std::size_t result = 0;
    auto iterBegin = storage.begin<Foo>();
    auto iterEnd = storage.end<Foo>();
    for (; iterBegin != iterEnd; ++iterBegin) {
        ++result;
    }

    EXPECT_EQ(result, 0);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
