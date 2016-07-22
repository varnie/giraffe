//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, CheckRetrievingAllEntities) {

    struct Foo {
        Foo() { }
    };

    using StorageT = Giraffe::Storage<>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;

    for (int i = 0; i < 5; ++i) {
        EntityT e = storage.addEntity();
    }

    std::size_t count1 = 0, count2 = 0;

    for (const auto &entity: storage.range()) {
        ++count1;
    }

    EXPECT_EQ(count1, 5);

    storage.process([&](const EntityT &entity) {
        ++count2;
    });

    EXPECT_EQ(count2, 5);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
