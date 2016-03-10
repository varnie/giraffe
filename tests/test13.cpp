//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Storage.h"

TEST(StorageTest, SearchThreeRegisteredComponents) {

    struct Foo : public Engine::Component<Foo> {
        Foo(): Engine::Component<Foo>() {}
    };

    struct Bar : public Engine::Component<Bar> {
        Bar(): Engine::Component<Bar>() {}
    };

    struct Fred : public Engine::Component<Fred> {
        int i;
        Fred(int i): Engine::Component<Fred>(), i(i) {}
    };

    Engine::Storage storage;
    storage.registerComponentKind<Foo>();
    storage.registerComponentKind<Bar>();
    storage.registerComponentKind<Fred>();

    int iMustBe = 0;
    for (int i = 0; i < 20; ++i) {
        Engine::Entity e1 = storage.addEntity();
        e1.addComponent<Foo>();
        e1.addComponent<Bar>();
        e1.addComponent<Fred>(i);

        iMustBe += i;
    }

    std::size_t result = 0;
    std::size_t fredICount = 0;

    for (const Engine::Entity &e: storage.range<Foo, Bar, Fred>()) {
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
