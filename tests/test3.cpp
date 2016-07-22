//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include "../include/Giraffe.h"

TEST(StorageTest, GetRegisteredComponentFromEntity) {

    struct Foo {
        Foo() { }
    };

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;
    //storage.registerComponentKind<Foo>();

    EntityT e = storage.addEntity();
    e.addComponent<Foo>();
    bool ok;
    try {
        auto *pComp = e.getComponent<Foo>();;
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
