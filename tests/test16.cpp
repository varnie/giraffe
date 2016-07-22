//
// Created by varnie on 2/25/16.
//

#include <gtest/gtest.h>
#include "../include/Giraffe.h"

TEST(StorageTest, ComponentsDestructorsInvocation1) {

    struct Foo {
        Foo(int &result) : m_pInt(new int(29)), m_result(result) {

        }

        Foo(const Foo &other) : m_pInt(new int(*other.m_pInt)), m_result(other.m_result) {

        }

        Foo &operator=(const Foo &other) {
            delete m_pInt;
            m_pInt = new int(*other.m_pInt);
            m_result = other.m_result;
            return *this;
        }

        ~Foo() {
            delete m_pInt;
            m_result++;
        }

        int *m_pInt;
        int &m_result;
    };

    int result = 0;
    {
        using StorageT = Giraffe::Storage<Foo>;
        using EntityT = Giraffe::Entity<StorageT>;

        StorageT storage;
        //storage.registerComponentKind<Foo>();
        EntityT e = storage.addEntity();
    }

    //checks that no destructors of components were called, because no components were added
    ASSERT_EQ(result, 0);
}

TEST(StorageTest, ComponentsDestructorsInvocation2) {

    struct Foo {
        Foo(int &result) : m_pInt(new int(29)), m_result(result) {

        }

        Foo(const Foo &other) : m_pInt(new int(*other.m_pInt)), m_result(other.m_result) {

        }

        Foo &operator=(const Foo &other) {
            delete m_pInt;
            m_pInt = new int(*other.m_pInt);
            m_result = other.m_result;
            return *this;
        }

        ~Foo() {
            delete m_pInt;
            m_result++;
        }

        int *m_pInt;
        int &m_result;
    };

    int result = 0;
    {
        using StorageT = Giraffe::Storage<Foo>;
        using EntityT = Giraffe::Entity<StorageT>;

        StorageT storage;
        //storage.registerComponentKind<Foo>();
        EntityT e = storage.addEntity();
        e.addComponent<Foo>(result);
    }
    //checks that 1 destructor of components was called at the end of scope, because only 1 component was added
    ASSERT_EQ(result, 1);
}

TEST(StorageTest, ComponentsDestructorsInvocation3) {

    struct Foo {
        Foo(int &result) : m_pInt(new int(29)), m_result(result) {

        }

        Foo(const Foo &other) : m_pInt(new int(*other.m_pInt)), m_result(other.m_result) {

        }

        Foo &operator=(const Foo &other) {
            delete m_pInt;
            m_pInt = new int(*other.m_pInt);
            m_result = other.m_result;
            return *this;
        }

    public:
        ~Foo() {
            delete m_pInt;
            m_result++;
        }

        int *m_pInt;
        int &m_result;
    };

    int result = 0;

    //checks that explicit removeComponent calls the dtor
    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;
    //storage.registerComponentKind<Foo>();
    EntityT e = storage.addEntity();
    e.addComponent<Foo>(result);
    //must explicitly call the dtor
    e.removeComponent<Foo>();

    ASSERT_EQ(result, 1);
}

TEST(StorageTest, ComponentsDestructorsInvocation4) {

    struct Foo {
        Foo(int &result) : m_pInt(new int(29)), m_result(result) {

        }

        Foo(const Foo &other) : m_pInt(new int(*other.m_pInt)), m_result(other.m_result) {

        }

        Foo &operator=(const Foo &other) {
            delete m_pInt;
            m_pInt = new int(*other.m_pInt);
            m_result = other.m_result;
            return *this;
        }

        ~Foo() {
            delete m_pInt;
            m_result++;
        }

        int *m_pInt;
        int &m_result;
    };

    int result = 0;

    {
        using StorageT = Giraffe::Storage<Foo>;
        using EntityT = Giraffe::Entity<StorageT>;

        StorageT storage;
        //storage.registerComponentKind<Foo>();
        EntityT e = storage.addEntity();
        e.addComponent<Foo>(result);
        e.removeComponent<Foo>();
        e.addComponent<Foo>(result);
    }
    //there must be two dtors calls: 1) during explicit removeComponent 2) another one at the end of scope
    ASSERT_EQ(result, 2);
}

TEST(StorageTest, ComponentsDestructorsInvocation5) {

    struct Foo {
        Foo(int &result) : m_pInt(new int(29)), m_result(result) {

        }

        Foo(const Foo &other) : m_pInt(new int(*other.m_pInt)), m_result(other.m_result) {

        }

        Foo &operator=(const Foo &other) {
            delete m_pInt;
            m_pInt = new int(*other.m_pInt);
            m_result = other.m_result;
            return *this;
        }

        ~Foo() {
            delete m_pInt;
            m_result++;
        }

        int *m_pInt;
        int &m_result;
    };

    int result = 0;

    using StorageT = Giraffe::Storage<Foo>;
    using EntityT = Giraffe::Entity<StorageT>;

    StorageT storage;
    //storage.registerComponentKind<Foo>();

    for (int i = 0; i < 5; ++i) {
        EntityT eX = storage.addEntity();
        eX.addComponent<Foo>(result);
        storage.removeEntity(eX);
    }

    ASSERT_EQ(result, 5);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}