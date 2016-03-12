//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, AddNonRegisteredComponentToEntity) {

    struct Foo : public Giraffe::Component<Foo> {
        Foo(): Giraffe::Component<Foo>() {}
    };

    struct Bar : public Giraffe::Component<Bar> {
        Bar(): Giraffe::Component<Bar>() {}
    };

    Giraffe::Storage storage;
    Giraffe::Entity e1 = storage.addEntity();
    //storage.registerComponentKind<Foo>(); //it is intentionally absent!
    e1.addComponent<Foo>();

    Giraffe::Entity e2 = storage.addEntity();

    std::size_t componentFooKindIndex = Giraffe::DerivedComponentsPool<Foo>::index;
    std::size_t componentBarKindIndex = Giraffe::DerivedComponentsPool<Bar>::index;

    std::size_t fooCount = 0;
    for (const auto &entity: storage.range<Foo>()) {
        ++fooCount;
    }

    EXPECT_EQ(fooCount, 1);
    EXPECT_EQ(componentFooKindIndex, 0);
    EXPECT_EQ(componentBarKindIndex, Giraffe::COMPONENT_DOES_NOT_EXIST);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
