//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, SearchThreeRegisteredComponents) {

    struct Foo {
        Foo() { }
    };

    struct Bar {
        Bar() { }
    };

    struct Fred {
        int m_i;

        Fred(int i) : m_i(i) { }
    };

    using StorageT = Giraffe::Storage<Foo, Bar, Fred>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;

    int iMustBe = 0;
    int count = 20;
    for (int i = 0; i < count; ++i) {
        EntityT e1 = storage.addEntity();
        e1.addComponent<Foo>();
        e1.addComponent<Bar>();
        e1.addComponent<Fred>(i);

        iMustBe += i;
    }

    std::size_t result = 0;
    std::size_t fredICount = 0;

    for (const auto &e: storage.range<Foo, Bar, Fred>()) {
        ++result;
        Fred *pFred = e.getComponent<Fred>();
        fredICount += pFred->m_i;
    }

    EXPECT_EQ(result, count);
    EXPECT_EQ(fredICount, iMustBe);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
