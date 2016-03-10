//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Storage.h"

TEST(StorageTest, AddNonRegisteredComponentToEntity) {

    struct Foo : public Engine::Component<Foo> {
        Foo(): Engine::Component<Foo>() {}
    };

    struct Bar : public Engine::Component<Bar> {
        Bar(): Engine::Component<Bar>() {}
    };

    Engine::Storage storage;
    Engine::Entity e1 = storage.addEntity();
    //storage.registerComponentKind<Foo>(); //it is intentionally absent!
    e1.addComponent<Foo>();

    Engine::Entity e2 = storage.addEntity();

    std::size_t componentFooKindIndex = Engine::DerivedComponentsPoolNEW<Foo>::index;
    std::size_t componentBarKindIndex = Engine::DerivedComponentsPoolNEW<Bar>::index;

    std::size_t fooCount = 0;
    for (const auto &entity: storage.range<Foo>()) {
        ++fooCount;
    }

    EXPECT_EQ(fooCount, 1);
    EXPECT_EQ(componentFooKindIndex, 0);
    EXPECT_EQ(componentBarKindIndex, Engine::COMPONENT_DOES_NOT_EXIST);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
