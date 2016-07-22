//
// Created by varnie on 3/21/16.
//

#ifndef ECS_SYSTEM_H
#define ECS_SYSTEM_H

namespace Giraffe {

    template <class... Types>
    class Storage;

    template <class Storage>
    class System {
    public:
        System(Storage &storage) : m_storage(storage) { }
        virtual ~System() { }
        virtual void update(float f) = 0;
    protected:
        Storage &m_storage;
    };

}

#endif //ECS_SYSTEM_H
