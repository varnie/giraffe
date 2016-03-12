//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include "../include/Giraffe.h"

TEST(StorageTest, GetRegisteredComponentFromEntity) {

    struct Foo : public Giraffe::Component<Foo> {
        Foo() : Giraffe::Component<Foo>() { }
    };

    Giraffe::Storage storage;
    storage.registerComponentKind<Foo>();

    Giraffe::Entity e = storage.addEntity();
    e.addComponent<Foo>();
    bool ok;
    try {
        auto *pComp = storage.getComponent<Foo>(e);
        ok = true;
    } catch (const std::runtime_error &exc) {
        ok = false;
    }

    ASSERT_TRUE(ok);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
