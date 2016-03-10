//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Storage.h"

TEST(StorageTest, EntityHasComponents) {

    struct Foo : public Engine::Component<Foo> {
        Foo(): Engine::Component<Foo>() {}
    };

    struct Bar : public Engine::Component<Bar> {
        Bar(): Engine::Component<Bar>() {}
    };

    struct Fred : public Engine::Component<Fred> {
        Fred(): Engine::Component<Fred>() {}
    };

    Engine::Storage storage;
    storage.registerComponentKind<Foo>();
    storage.registerComponentKind<Bar>();
    //note, not registered component kind
    //storage.registerComponentKind<Fred>();

    Engine::Entity e1 = storage.addEntity();
    e1.addComponent<Foo>();
    e1.addComponent<Bar>();

    EXPECT_EQ(e1.hasComponent<Foo>(), true);
    EXPECT_EQ(e1.hasComponent<Bar>(), true);
    EXPECT_EQ(e1.hasComponent<Fred>(), false);

    //now add non registered component
    e1.addComponent<Fred>();
    EXPECT_EQ(e1.hasComponent<Fred>(), true);

    //now remove some component, Fred
    e1.removeComponent<Fred>();
    EXPECT_EQ(e1.hasComponent<Fred>(), false);

    //now remove entity
    storage.removeEntity(e1);
    EXPECT_EQ(e1.hasComponent<Foo>(), false);
    EXPECT_EQ(e1.hasComponent<Bar>(), false);
    EXPECT_EQ(e1.hasComponent<Fred>(), false);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
