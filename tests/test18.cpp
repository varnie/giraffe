//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, CheckRetrievingAllEntities) {

    struct Foo : public Giraffe::Component<Foo> {
        Foo() : Giraffe::Component<Foo>() { }
    };

    Giraffe::Storage storage;

    for (int i = 0; i < 5; ++i) {
        Giraffe::Entity e = storage.addEntity();
    }

    std::size_t count1 = 0, count2 = 0, count3 = 0;
    for (auto iter = storage.begin(), iterEnd = storage.end();
        iter != iterEnd;
        ++iter) {
        ++count1;
    }

    EXPECT_EQ(count1, 5);

    for (const auto &entity: storage.range()) {
        ++count2;
    }

    EXPECT_EQ(count2, 5);

    storage.process([&](const Giraffe::Entity &entity) {
        ++count3;
    });

    EXPECT_EQ(count3, 5);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
