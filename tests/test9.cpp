//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, RemoveRestoreEntity) {

    Giraffe::Storage storage;
    Giraffe::Entity e1 = storage.addEntity();
    Giraffe::Entity e2 = storage.addEntity();
    Giraffe::Entity e3 = storage.addEntity();
    storage.removeEntity(e2);
    Giraffe::Entity e4 = storage.addEntity();

    EXPECT_EQ(e4.m_index, 1);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
