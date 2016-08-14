//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include "../include/Giraffe.h"

TEST(StorageTest, retrieveNonAddedToEntityComponent) {

    //component retrieval from storage must fail if
    //component wasn't added to entity

    struct Foo {
        int m_i;

        Foo(int i) : m_i(i) { }
    };

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;

    EntityT e = storage.addEntity();
    //e.addComponent<Foo>(); //intentionally commented out

    bool mustThrow;
    try {
        auto *pComp = e.getComponent<Foo>();
        mustThrow = false;
    } catch (const std::runtime_error &exc) {
        mustThrow = true;
    }

    ASSERT_TRUE(mustThrow);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
