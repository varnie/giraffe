//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, EntityRemoveNonRegisteredAndNonAddedToEntityComponent) {

    struct Foo {
        Foo() { }
    };

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;
    EntityT e = storage.addEntity();

    e.removeComponent<Foo>();

    ASSERT_TRUE(true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
