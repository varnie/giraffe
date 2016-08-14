//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, EntityHasComponents) {

    struct Foo {
        Foo() { }
    };

    struct Bar {
        Bar() { }
    };

    struct Fred {
        Fred() { }
    };

    using StorageT = Giraffe::Storage<Foo, Bar, Fred>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;

    EntityT e1 = storage.addEntity();
    e1.addComponent<Foo>();
    e1.addComponent<Bar>();

    EXPECT_EQ(e1.hasComponent<Foo>(), true);
    EXPECT_EQ(e1.hasComponent<Bar>(), true);
    EXPECT_EQ(e1.hasComponent<Fred>(), false);

    //now add that absent component
    e1.addComponent<Fred>();
    EXPECT_EQ(e1.hasComponent<Fred>(), true);

    //now remove some component, Fred
    e1.removeComponent<Fred>();
    EXPECT_EQ(e1.hasComponent<Fred>(), false);

    //now remove entity
    storage.removeEntity(e1);

    EXPECT_EQ(e1.isValid(), false);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
