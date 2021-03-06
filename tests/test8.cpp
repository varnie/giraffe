//
// Created by varnie on 2/23/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, StorageRetrievals) {

    struct Foo {
        Foo() { }
    };

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    class FooSystem : public Giraffe::System<StorageT> {
        std::size_t m_found;
    public:
        FooSystem(StorageT &storage) : Giraffe::System<StorageT>(storage), m_found(0) { }

        virtual void update(float f) {
            m_found = 0;
            m_storage.process<Foo>([&](const EntityT &entity) {
                ++m_found;
            });
        }

        std::size_t getFound() const {
            return m_found;
        }
    };

    StorageT storage;

    EntityT e1 = storage.addEntity();
    e1.addComponent<Foo>();
    EntityT e2 = storage.addEntity();
    e2.addComponent<Foo>();
    EntityT e3 = storage.addEntity();
    e3.addComponent<Foo>();
    EntityT e4 = storage.addEntity();

    std::size_t result1 = 0, result2 = 0, result3 = 0;
    //using storage system
    FooSystem system = FooSystem(storage);
    system.update(1.0f);
    result1 = system.getFound();

    //using storage iterators
    auto iterBegin = storage.begin<Foo>();
    auto iterEnd = storage.end<Foo>();
    for (; iterBegin != iterEnd; ++iterBegin) {
        ++result2;
    }

    //using storage range
    for (const auto &entity: storage.range<Foo>()) {
        ++result3;
    }

    EXPECT_EQ(result1, 3);
    EXPECT_EQ(result2, 3);
    EXPECT_EQ(result3, 3);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
