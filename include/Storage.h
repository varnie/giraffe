//
// Created by varnie on 3/21/16.
//

#ifndef ECS_STORAGE_H
#define ECS_STORAGE_H

#include <cstddef> //std::size_t
#include <vector>
#include <functional> //std::function
#include <algorithm> //std::all_of
#include <memory> //std::unique_ptr, std::make_unique
#include <deque>
#include <tuple>
#include <type_traits>

#include <utility>
#include <exception> //std::runtime_error
#include <string> //std::to_string
#include <cassert>

#include "./ComponentsPool.h"
#include "./FilterIterator.h"
#include "./Predicates.h"

namespace {
    template<typename T, typename ...Ts>
    struct contains;

    template<typename T>
    struct contains<T> {
        static constexpr bool value = false;
    };

    template<typename T1, typename T2, typename ...Ts>
    struct contains<T1, T2, Ts...> {
        static constexpr bool value = std::is_same<T1, T2>::value ? true : contains<T1, Ts...>::value;
    };
}

namespace Giraffe {

    template <class Storage>
    struct Entity {
        Entity(std::size_t index, std::size_t version, Storage &storage)
            : m_index(index), m_version(version), m_storage(storage) { }

        template<typename C, typename ... Args>
        void addComponent(Args &&... args);

        template<typename C>
        void removeComponent();

        template<typename C>
        C *getComponent() const;

        template<typename C>
        bool hasComponent() const;

        bool isValid() const {
            return m_storage.isValid(*this);
        }

        std::size_t m_index;
        std::size_t m_version;
        Storage &m_storage;
    };

    template <class... Components>
    class Storage {
    public:

        friend class PredicateOne<Entity<Storage>, Storage>;

        friend class PredicateTwo<Entity<Storage>, Storage>;

        friend class PredicateAll<Entity<Storage>, Storage>;

        using EntitiesContainerT = typename std::vector<Entity<Storage>>;
        using EntitiesIteratorT = typename EntitiesContainerT::const_iterator;

        template<template<class Item, class Storage> class Predicate>
        using Iterator = FilterIterator<EntitiesIteratorT, Predicate<Entity<Storage>, Storage> >;

        Storage()
                : m_entitiesComponentsMask(), m_entities(), m_deletedEntities(), m_entitiesVersions(),
                  m_componentsKindsCount(0) {

            using expander = int[];
            (void) expander{ 0, (registerComponentKind<Components>(), 0)... };
        }

        bool isValid(const Entity<Storage> &entity) const {

            return entity.m_index < m_entitiesVersions.size() && m_entitiesVersions[entity.m_index] == entity.m_version;
        }

        Entity<Storage> addEntity() {

            if (!m_deletedEntities.empty()) {
                Entity<Storage> entity = m_deletedEntities.back();
                m_deletedEntities.pop_back();
                ++entity.m_version;
                m_entities.push_back(entity);
            } else {
                std::size_t curEntityIndex = m_entities.size();
                m_entities.push_back(Entity<Storage>(curEntityIndex, 0, *this));
                m_entitiesVersions.push_back(0);
                m_entitiesComponentsMask.push_back(
                        std::vector<std::size_t>(m_componentsKindsCount, COMPONENT_DOES_NOT_EXIST));
            }

            return m_entities.back();
        }

        void removeEntity(const Entity<Storage> &entity) {

            assert(entity.isValid() && "invalid entity");

            std::size_t entityIndex = entity.m_index;
            m_deletedEntities.push_back(entity);
            ++m_entitiesVersions[entityIndex];

            //remove all entity's components from the pools
            using expander = int[];
            (void) expander{ 0, (removeComponent<Components>(entity), 0)... };

            //reset the mask
            auto &entityComponentsMask = m_entitiesComponentsMask[entityIndex];
            std::size_t componentsCount = entityComponentsMask.size();
            entityComponentsMask.assign(componentsCount, COMPONENT_DOES_NOT_EXIST);
        }

        std::size_t getEntitiesCount() const {

            return m_entities.size();
        }

        template<typename C>
        std::size_t getPoolSize() const {

            std::size_t componentKindIndex = ComponentsPool<C>::index;
            if (componentKindIndex == COMPONENT_DOES_NOT_EXIST) {
                return 0;
            }

            return std::get<ComponentsPool<C>>(m_pools).getSize();
        }

        template<typename C, typename ... Args>
        void addComponent(const Entity<Storage> &entity, Args &&... args) {

            static_assert(contains<C, Components...>::value, "type is not registered");
            assert(entity.isValid() && "invalid entity");

            std::size_t componentKindIndex = ComponentsPool<C>::index;

            if (m_entitiesComponentsMask[entity.m_index][componentKindIndex] != COMPONENT_DOES_NOT_EXIST) {
                //entity already has a component
                return;
            }

            ComponentsPool<C> &poolC = std::get<ComponentsPool<C>>(m_pools);

            std::pair<std::size_t, C *> result = poolC.addComponent(std::forward<Args>(args) ...);
            std::size_t componentIndex = result.first;
            C *component = result.second;
            m_entitiesComponentsMask[entity.m_index][componentKindIndex] = componentIndex;

            //TODO: refactor out
            auto &line = std::get<typename std::vector<C *>>(m_vals);
            if (componentIndex < line.size()) {
                line[componentIndex] = component;
            } else {
                line.push_back(component);
            }
        }

        template<typename C>
        void removeComponent(const Entity<Storage> &entity) {

            static_assert(contains<C, Components...>::value, "type is not registered");
            assert(entity.isValid() && "invalid entity");

            std::size_t componentKindIndex = ComponentsPool<C>::index;
            if (componentKindIndex == COMPONENT_DOES_NOT_EXIST) {
                return;
            }

            std::size_t entityIndex = entity.m_index;
            auto &entityComponentsMask = m_entitiesComponentsMask[entityIndex];
            std::size_t curComponentIndex = entityComponentsMask[componentKindIndex];

            if (curComponentIndex != COMPONENT_DOES_NOT_EXIST) {
                entityComponentsMask[componentKindIndex] = COMPONENT_DOES_NOT_EXIST;

                ComponentsPool<C> &poolC = std::get<ComponentsPool<C>>(m_pools);
                poolC.removeComponent(curComponentIndex);

                //TODO: refactor out
                auto &line = std::get<typename std::vector<C *>>(m_vals);
                line[curComponentIndex] = nullptr;
            }
        }

        template<typename C>
        bool hasComponent(const Entity<Storage> &entity) const {

            static_assert(contains<C, Components...>::value, "type is not registered");
            assert(entity.isValid() && "invalid entity");

            std::size_t componentKindIndex = ComponentsPool<C>::index;
            std::size_t entityIndex = entity.m_index;

            return componentKindIndex != COMPONENT_DOES_NOT_EXIST
                   && m_entitiesComponentsMask[entityIndex][componentKindIndex] != COMPONENT_DOES_NOT_EXIST;
        }

        template<typename C>
        C *getComponent(const Entity<Storage> &entity) const {

            static_assert(contains<C, Components...>::value, "type is not registered");
            assert(entity.isValid() && "invalid entity");

            std::size_t componentKindIndex = ComponentsPool<C>::index;

            if (componentKindIndex != COMPONENT_DOES_NOT_EXIST) {

                std::size_t entityIndex = entity.m_index;
                std::size_t componentIndex = m_entitiesComponentsMask[entityIndex][componentKindIndex];

                if (componentIndex != COMPONENT_DOES_NOT_EXIST) {

                    const auto &line = std::get<typename std::vector<C *>>(m_vals);
                    return line[componentIndex];
                }

                throw std::runtime_error("component not registered");
            }

            throw std::runtime_error("component kind not registered");
        }

        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        Iterator<PredicateAll> begin() {

            PredicateAll<Entity<Storage>, Storage> predicate(*this, {ComponentsPool<A1>::index,
                                                            ComponentsPool<A2>::index,
                                                            ComponentsPool<A3>::index,
                                                            ComponentsPool<Ax>::index ...});
            return Iterator<PredicateAll>(m_entities.begin(), m_entities.end(), predicate);
        }

        //2 parameters
        template<class A1, class A2>
        Iterator<PredicateTwo> begin() {

            PredicateTwo<Entity<Storage>, Storage> predicate(*this, ComponentsPool<A1>::index,
                                                    ComponentsPool<A2>::index);
            return Iterator<PredicateTwo>(m_entities.begin(), m_entities.end(), predicate);
        }

        //1 parameter
        template<class A>
        Iterator<PredicateOne> begin() {

            PredicateOne<Entity<Storage>, Storage> predicate(*this, ComponentsPool<A>::index);
            return Iterator<PredicateOne>(m_entities.begin(), m_entities.end(), predicate);
        }

        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        Iterator<PredicateAll> end() {

            PredicateAll<Entity<Storage>, Storage> predicate(*this, {ComponentsPool<A1>::index,
                                                            ComponentsPool<A2>::index,
                                                            ComponentsPool<A3>::index,
                                                            ComponentsPool<Ax>::index ...});
            return Iterator<PredicateAll>(m_entities.end(), m_entities.end(), predicate);
        }

        //2 parameters
        template<class A1, class A2>
        Iterator<PredicateTwo> end() {

            PredicateTwo<Entity<Storage>, Storage> predicate(*this, ComponentsPool<A1>::index,
                                                    ComponentsPool<A2>::index);
            return Iterator<PredicateTwo>(m_entities.end(), m_entities.end(), predicate);
        }

        //1 parameter
        template<class A>
        Iterator<PredicateOne> end() {

            PredicateOne<Entity<Storage>, Storage> predicate(*this, ComponentsPool<A>::index);
            return Iterator<PredicateOne>(m_entities.end(), m_entities.end(), predicate);
        }

        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        void process(std::function<void(const Entity<Storage> &entity)> func) {

            PredicateAll<Entity<Storage>, Storage> predicate(*this, {ComponentsPool<A1>::index,
                                                            ComponentsPool<A2>::index,
                                                            ComponentsPool<A3>::index,
                                                            ComponentsPool<Ax>::index ...});

            for (Iterator<PredicateAll> iterBegin = Iterator<PredicateAll>(m_entities.begin(), m_entities.end(),
                                                                           predicate),
                         iterEnd = Iterator<PredicateAll>(m_entities.end(), m_entities.end(), predicate);
                 iterBegin != iterEnd;
                 ++iterBegin) {
                const Entity<Storage> &entity = *iterBegin;
                func(entity);
            }
        }

        //2 parameters
        template<class A1, class A2>
        void process(std::function<void(const Entity<Storage> &entity)> func) {

            PredicateTwo<Entity<Storage>, Storage> predicate(*this, ComponentsPool<A1>::index,
                                                    ComponentsPool<A2>::index);

            for (Iterator<PredicateTwo> iterBegin = Iterator<PredicateTwo>(m_entities.begin(), m_entities.end(),
                                                                           predicate),
                         iterEnd = Iterator<PredicateTwo>(m_entities.end(), m_entities.end(), predicate);
                 iterBegin != iterEnd;
                 ++iterBegin) {
                const Entity<Storage> &entity = *iterBegin;
                func(entity);
            }
        }

        //1 parameter
        template<class A>
        void process(std::function<void(const Entity<Storage> &entity)> func) {

            PredicateOne<Entity<Storage>, Storage> predicate(*this, ComponentsPool<A>::index);

            for (Iterator<PredicateOne> iterBegin = Iterator<PredicateOne>(m_entities.begin(), m_entities.end(),
                                                                           predicate),
                         iterEnd = Iterator<PredicateOne>(m_entities.end(), m_entities.end(), predicate);
                 iterBegin != iterEnd;
                 ++iterBegin) {
                const Entity<Storage> &entity = *iterBegin;
                func(entity);
            }
        }

        template<typename Iterator>
        struct Result {
            Iterator _begin;
            Iterator _end;

            Result(Iterator begin, Iterator end) : _begin(begin), _end(end) { }

            Iterator begin() const { return _begin; }

            Iterator end() const { return _end; }
        };

        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        Result<Iterator<PredicateAll>> range() {

            PredicateAll<Entity<Storage>, Storage> predicate(*this, {ComponentsPool<A1>::index,
                                                            ComponentsPool<A2>::index,
                                                            ComponentsPool<A3>::index,
                                                            ComponentsPool<Ax>::index ...});

            return Result<Iterator<PredicateAll>>(
                    Iterator<PredicateAll>(m_entities.begin(), m_entities.end(), predicate),
                    Iterator<PredicateAll>(m_entities.end(), m_entities.end(), predicate)
            );
        }

        //2 parameters
        template<class A1, class A2>
        Result<Iterator<PredicateTwo>> range() {

            PredicateTwo<Entity<Storage>, Storage> predicate(*this, ComponentsPool<A1>::index,
                                                    ComponentsPool<A2>::index);

            return Result<Iterator<PredicateTwo>>(
                    Iterator<PredicateTwo>(m_entities.begin(), m_entities.end(), predicate),
                    Iterator<PredicateTwo>(m_entities.end(), m_entities.end(), predicate)
            );
        }

        //1 parameter
        template<class A>
        Result<Iterator<PredicateOne>> range() {

            PredicateOne<Entity<Storage>, Storage> predicate(*this, ComponentsPool<A>::index);

            return Result<Iterator<PredicateOne>>(
                    Iterator<PredicateOne>(m_entities.begin(), m_entities.end(), predicate),
                    Iterator<PredicateOne>(m_entities.end(), m_entities.end(), predicate)
            );
        }

        void process(std::function<void(const Entity<Storage> &entity)> func) {

            for (auto iterBegin = m_entities.begin(), iterEnd = m_entities.end();
                 iterBegin != iterEnd;
                 ++iterBegin) {
                const Giraffe::Entity<Storage> &entity = *iterBegin;
                if (entity.isValid()) {
                    func(entity);
                }
            }
        }

        Result<Iterator<PredicateDummy>> range() {

            return Result<Iterator<PredicateDummy>>(
                    Iterator<PredicateDummy>(m_entities.begin(), m_entities.end(), PredicateDummy<Entity<Storage>, Storage>()),
                    Iterator<PredicateDummy>(m_entities.end(), m_entities.end(), PredicateDummy<Entity<Storage>, Storage>())
            );
        }

    private:

        template<typename C>
        void registerComponentKind() {

            //has the component kind/family been registered yet?
            if (ComponentsPool<C>::index == COMPONENT_DOES_NOT_EXIST) {

                if (m_componentsKindsCount == MAX_COMPONENTS_COUNT) {
                    throw std::runtime_error(
                            "Maximum number of components exceeded: " + std::to_string(MAX_COMPONENTS_COUNT));
                }

                ComponentsPool<C>::index = m_componentsKindsCount++;
            }
        }

        std::vector<std::vector<std::size_t> > m_entitiesComponentsMask; //entity id -> components mask
        EntitiesContainerT m_entities;
        std::deque<Entity<Storage>> m_deletedEntities;
        std::vector<std::size_t> m_entitiesVersions;
        std::size_t m_componentsKindsCount;
        std::tuple<std::vector<Components *>...> m_vals;
        std::tuple<ComponentsPool<Components>...> m_pools;
    };

    template <class Storage>
    template<typename C, typename ... Args>
    void Entity<Storage>::addComponent(Args &&... args) {
        m_storage.addComponent<C>(*this, std::forward<Args>(args) ...);
    }

    template <class Storage>
    template<typename C>
    void Entity<Storage>::removeComponent() {
        m_storage.removeComponent<C>(*this);
    }

    template <class Storage>
    template<typename C>
    C *Entity<Storage>::getComponent() const {
        return m_storage.getComponent<C>(*this);
    }

    template <class Storage>
    template<typename C>
    bool Entity<Storage>::hasComponent() const {
        return m_storage.hasComponent<C>(*this);
    }
}

#endif //ECS_STORAGE_H
