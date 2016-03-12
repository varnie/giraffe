//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, SearchThreeRegisteredComponents) {

    struct Foo : public Giraffe::Component<Foo> {
        Foo(): Giraffe::Component<Foo>() {}
    };

    struct Bar : public Giraffe::Component<Bar> {
        Bar(): Giraffe::Component<Bar>() {}
    };

    struct Fred : public Giraffe::Component<Fred> {
        int i;
        Fred(int i): Giraffe::Component<Fred>(), i(i) {}
    };

    Giraffe::Storage storage;
    storage.registerComponentKind<Foo>();
    storage.registerComponentKind<Bar>();
    storage.registerComponentKind<Fred>();

    int iMustBe = 0;
    for (int i = 0; i < 20; ++i) {
        Giraffe::Entity e1 = storage.addEntity();
        e1.addComponent<Foo>();
        e1.addComponent<Bar>();
        e1.addComponent<Fred>(i);

        iMustBe += i;
    }

    std::size_t result = 0;
    std::size_t fredICount = 0;

    for (const Giraffe::Entity &e: storage.range<Foo, Bar, Fred>()) {
        ++result;
        Fred *pFred = e.getComponent<Fred>();
        fredICount += pFred->i;
    }

    EXPECT_EQ(result, 20);
    EXPECT_EQ(fredICount, iMustBe);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
