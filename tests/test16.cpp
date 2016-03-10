//
// Created by varnie on 2/25/16.
//

#include <gtest/gtest.h>
#include <iostream>
#include "../include/Storage.h"

//TODO:
TEST(StorageTest, ComponentsDestructorsInvocation1) {

    struct Foo : public Engine::Component<Foo> {
        Foo(int &result): Engine::Component<Foo>(), _pInt(new int(29)), _result(result) {

            std::cout << "Foo ctor" << std::endl;
        }
        Foo(const Foo &other) : _pInt(new int(*other._pInt)), _result(other._result) {

            std::cout << "Foo copy-ctor" << std::endl;
        }
        Foo& operator=(const Foo& other) {
            std::cout << "Foo copy assign" << std::endl;
            delete _pInt;
            _pInt = new int(*other._pInt);
            _result = other._result;
            return *this;
        }
    public:
        ~Foo() {
            delete _pInt;
            std::cout << "Foo ~dtor" << std::endl;
            _result++;
        }
        int *_pInt;
        int &_result;
    };

    int result = 0;
    {
        Engine::Storage storage;
        storage.registerComponentKind<Foo>();
        Engine::Entity e = storage.addEntity();
    }

    //checks that no destructors of components were called, because no components were added
    ASSERT_EQ(result, 0);
}

TEST(StorageTest, ComponentsDestructorsInvocation2) {

    struct Foo : public Engine::Component<Foo> {
        Foo(int &result): Engine::Component<Foo>(), _pInt(new int(29)), _result(result) {

            std::cout << "Foo ctor" << std::endl;
        }
        Foo(const Foo &other) : _pInt(new int(*other._pInt)), _result(other._result) {

            std::cout << "Foo copy-ctor" << std::endl;
        }
        Foo& operator=(const Foo& other) {
            std::cout << "Foo copy assign" << std::endl;
            delete _pInt;
            _pInt = new int(*other._pInt);
            _result = other._result;
            return *this;
        }
    public:
        ~Foo() {
            delete _pInt;
            std::cout << "Foo ~dtor" << std::endl;
            _result++;
        }
        int *_pInt;
        int &_result;
    };

    int result = 0;
    {
        Engine::Storage storage;
        storage.registerComponentKind<Foo>();
        Engine::Entity e = storage.addEntity();
        e.addComponent<Foo>(result);
    }
    //checks that 1 destructor of components was called at the end of scope, because only 1 component was added
    ASSERT_EQ(result, 1);
}

TEST(StorageTest, ComponentsDestructorsInvocation3) {

    struct Foo : public Engine::Component<Foo> {
        Foo(int &result): Engine::Component<Foo>(), _pInt(new int(29)), _result(result) {

            std::cout << "Foo ctor" << std::endl;
        }
        Foo(const Foo &other) : _pInt(new int(*other._pInt)), _result(other._result) {

            std::cout << "Foo copy-ctor" << std::endl;
        }
        Foo& operator=(const Foo& other) {
            std::cout << "Foo copy assign" << std::endl;
            delete _pInt;
            _pInt = new int(*other._pInt);
            _result = other._result;
            return *this;
        }
    public:
        ~Foo() {
            delete _pInt;
            std::cout << "Foo ~dtor" << std::endl;
            _result++;
        }
        int *_pInt;
        int &_result;
    };

    int result = 0;

    //checks that explicit removeComponent calls the dtor
    Engine::Storage storage;
    storage.registerComponentKind<Foo>();
    Engine::Entity e = storage.addEntity();
    e.addComponent<Foo>(result);
    //must explicitly call the dtor
    e.removeComponent<Foo>();

    ASSERT_EQ(result, 1);
}

TEST(StorageTest, ComponentsDestructorsInvocation4) {

    struct Foo : public Engine::Component<Foo> {
        Foo(int &result): Engine::Component<Foo>(), _pInt(new int(29)), _result(result) {

            std::cout << "Foo ctor" << std::endl;
        }
        Foo(const Foo &other) : _pInt(new int(*other._pInt)), _result(other._result) {

            std::cout << "Foo copy-ctor" << std::endl;
        }
        Foo& operator=(const Foo& other) {
            std::cout << "Foo copy assign" << std::endl;
            delete _pInt;
            _pInt = new int(*other._pInt);
            _result = other._result;
            return *this;
        }
    public:
        ~Foo() {
            delete _pInt;
            std::cout << "Foo ~dtor" << std::endl;
            _result++;
        }
        int *_pInt;
        int &_result;
    };

    int result = 0;

    {
        Engine::Storage storage;
        storage.registerComponentKind<Foo>();
        Engine::Entity e = storage.addEntity();
        e.addComponent<Foo>(result);
        e.removeComponent<Foo>();
        e.addComponent<Foo>(result);
    }
    //there must be two dtors calls: 1) during explicit removeComponent 2) another one at the end of scope
    ASSERT_EQ(result, 2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}