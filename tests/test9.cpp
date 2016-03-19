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
    EXPECT_FALSE(e2.isValid());

    Giraffe::Entity e4 = storage.addEntity();
    EXPECT_EQ(e4.m_index, 1);
    EXPECT_EQ(e4.m_version, 1);
    EXPECT_FALSE(e2.isValid());
    EXPECT_TRUE(e4.isValid());

    storage.removeEntity(e4);
    Giraffe::Entity e5 = storage.addEntity();
    EXPECT_EQ(e5.m_index, 1);
    EXPECT_EQ(e5.m_version, 2);
    EXPECT_FALSE(e2.isValid());
    EXPECT_FALSE(e4.isValid());
    EXPECT_TRUE(e5.isValid());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
