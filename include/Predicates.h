//
// Created by varnie on 3/22/16.
//

#ifndef GIRAFFE_PREDICATES_H
#define GIRAFFE_PREDICATES_H

#include <cstddef> //std::size_t
#include <initializer_list>
#include <algorithm>
#include <vector>
#include "./Config.h"

namespace Giraffe {

    template<typename T, typename Storage>
    class PredicateAll {
    public:
        PredicateAll(const Storage &storage, const std::initializer_list<std::size_t> &conditions)
                : m_storage(storage), m_conditions(conditions) { }

        bool operator()(const T &e) const {

            const auto &entityComponentsMask = m_storage.m_entitiesComponentsMask[e.m_index];
            return e.isValid() && std::all_of(m_conditions.cbegin(), m_conditions.cend(),
                                              [&entityComponentsMask](const std::size_t cond) {
                                                  return entityComponentsMask[cond] != COMPONENT_DOES_NOT_EXIST;
                                              });
        }

    private:
        const Storage &m_storage;
        std::vector<std::size_t> m_conditions;
    };

    template<typename T, typename Storage>
    class PredicateTwo {
    public:
        PredicateTwo(const Storage &storage, const std::size_t cond1, const std::size_t cond2)
                : m_storage(storage), m_cond1(cond1), m_cond2(cond2) { }

        bool operator()(const T &e) const {

            const auto &entityComponentsMask = m_storage.m_entitiesComponentsMask[e.m_index];
            return e.isValid() && entityComponentsMask[m_cond1] != COMPONENT_DOES_NOT_EXIST &&
                   entityComponentsMask[m_cond2] != COMPONENT_DOES_NOT_EXIST;
        }

    private:
        const Storage &m_storage;
        std::size_t m_cond1, m_cond2;
    };

    template<typename T, typename Storage>
    class PredicateOne {
    public:
        PredicateOne(const Storage &storage, const std::size_t condition)
                : m_storage(storage), m_condition(condition) { }

        bool operator()(const T &e) const {

            const auto &entityComponentsMask = m_storage.m_entitiesComponentsMask[e.m_index];
            return e.isValid() && entityComponentsMask[m_condition] != COMPONENT_DOES_NOT_EXIST;
        }

    private:
        const Storage &m_storage;
        std::size_t m_condition;
    };

    template<typename T, typename Storage>
    class PredicateDummy {
    public:
        bool operator()(const T &e) const {
            return e.isValid();
        }
    };

}

#endif //GIRAFFE_PREDICATES_H
