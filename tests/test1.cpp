//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include "../include/Giraffe.h"

TEST(StorageTest, retrieveNonRegisteredComponent_case_a) {
    //component retrieval from storage must fail if one of the following cases is true:
    //a) component kind not registered
    //b) component wasn't added to entity

    //this case checks the first case, a)

    struct Foo : public Giraffe::Component<Foo> {
        Foo() : Giraffe::Component<Foo>() { }
    };

    Giraffe::Storage storage;
    //storage.registerComponentKind<Foo>(); //intentionally commented out

    Giraffe::Entity e = storage.addEntity();

    bool mustThrow;
    try {
        auto *pComp = storage.getComponent<Foo>(e);
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
