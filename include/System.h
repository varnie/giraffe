//
// Created by varnie on 3/21/16.
//

#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

namespace Giraffe {

    class Storage;

    class System {
    public:
        System(Storage &storage) : _storage(storage) { }
        virtual ~System() { }
        virtual void update(float f) = 0;
    protected:
        Storage &_storage;
    };

}

#endif //ECS_SYSTEM_H
