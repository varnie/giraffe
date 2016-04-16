//
// Created by varnie on 3/21/16.
//

#ifndef ECS_CONFIG_H
#define ECS_CONFIG_H

#include <cstddef> //std::size_t
#include <limits> //std::numeric_limits
#include <memory>

#if __cplusplus == 201103L // C++11
//for old compilers
namespace {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args &&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}
#endif

namespace Giraffe {
    static constexpr std::size_t MAX_COMPONENTS_COUNT = 64;
    static constexpr std::size_t COMPONENT_DOES_NOT_EXIST = std::numeric_limits<std::size_t>::max();
    static constexpr std::size_t POOL_COMPONENTS_PER_CHUNK = 100;
}

#endif //ECS_CONFIG_H
