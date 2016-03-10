//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include <stdexcept>
#include "../include/Storage.h"

TEST(StorageTest, GetRegisteredComponentFromEntity) {

    struct Foo : public Engine::Component<Foo> {
        Foo(): Engine::Component<Foo>() {}
    };

    Engine::Storage storage;
    storage.registerComponentKind<Foo>();

    Engine::Entity e = storage.addEntity();
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
