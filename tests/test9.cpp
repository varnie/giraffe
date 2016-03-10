//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Storage.h"

TEST(StorageTest, RemoveRestoreEntity) {

    Engine::Storage storage;
    Engine::Entity e1 = storage.addEntity();
    Engine::Entity e2 = storage.addEntity();
    Engine::Entity e3 = storage.addEntity();
    storage.removeEntity(e2);
    Engine::Entity e4 = storage.addEntity();

    EXPECT_EQ(e4._index, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
