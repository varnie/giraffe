//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, SearchThreeRegisteredComponents) {

    struct Foo : public Giraffe::Component<Foo> {
        Foo() : Giraffe::Component<Foo>() { }
    };

    struct Bar : public Giraffe::Component<Bar> {
        Bar() : Giraffe::Component<Bar>() { }
    };

    struct Fred : public Giraffe::Component<Fred> {
        int m_i;

        Fred(int i) : Giraffe::Component<Fred>(), m_i(i) { }
    };

    Giraffe::Storage storage;
    storage.registerComponentKind<Foo>();
    storage.registerComponentKind<Bar>();
    storage.registerComponentKind<Fred>();

    int iMustBe = 0;
    int count = 20;
    for (int i = 0; i < count; ++i) {
        Giraffe::Entity e1 = storage.addEntity();
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
