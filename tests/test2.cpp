//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include "../include/Giraffe.h"

TEST(StorageTest, retrieveNonRegisteredComponent_case_b) {

    //component retrieval from storage must fail if one of the following cases is true:
    //a) component kind not registered
    //b) component wasn't added to entity

    //this case checks the second case, b)

    struct Foo {
        int m_i;

        Foo(int i) : m_i(i) { }
    };

    Giraffe::Storage storage;
    storage.registerComponentKind<Foo>();

    Giraffe::Entity e = storage.addEntity();
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
