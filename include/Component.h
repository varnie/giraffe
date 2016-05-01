//
// Created by varnie on 3/20/16.
//

#ifndef ECS_COMPONENTS_H
#define ECS_COMPONENTS_H

namespace Giraffe {

    template<typename T>
    struct Component {
        virtual ~Component() {}
    };

}

#endif //ECS_COMPONENTS_H
