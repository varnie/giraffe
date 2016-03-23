//
// Created by varnie on 3/21/16.
//

#ifndef ECS_CONFIG_H
#define ECS_CONFIG_H

#include <cstddef> //std::size_t
#include <limits> //std::numeric_limits

namespace Giraffe {
    static constexpr std::size_t MAX_COMPONENTS_COUNT = 64;
    static constexpr std::size_t COMPONENT_DOES_NOT_EXIST = std::numeric_limits<std::size_t>::max();
}

#endif //ECS_CONFIG_H
