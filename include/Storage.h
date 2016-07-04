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

#include <utility>
#include <exception> //std::runtime_error
#include <string> //std::to_string
#include <cassert>

#include "./ComponentsPool.h"
#include "./FilterIterator.h"
#include "./Predicates.h"

namespace Giraffe {

    class Storage;

    struct Entity {
        Entity(std::size_t index, std::size_t version, Giraffe::Storage &storage);

        template<typename C, typename ... Args>
        void addComponent(Args &&... args);

        template<typename C>
        void removeComponent();

        template<typename C>
        C *getComponent() const;

        template<typename C>
        bool hasComponent() const;

        bool isValid() const;

        std::size_t m_index;
        std::size_t m_version;
        Storage &m_storage;
    };

    class Storage {
    public:

        friend class PredicateOne<Entity, Storage>;

        friend class PredicateTwo<Entity, Storage>;

        friend class PredicateAll<Entity, Storage>;

        using EntitiesContainerT = std::vector<Entity>;
        using EntitiesIteratorT = EntitiesContainerT::const_iterator;

        template<template<class Item, class Storage> class Predicate>
        using Iterator = FilterIterator<EntitiesIteratorT, Predicate<Entity, Storage> >;

        Storage()
                : m_entitiesComponentsMask(), m_entities(), m_deletedEntities(), m_entitiesVersions(), m_pools(),
                  m_componentsKindsCount(0) { }

        bool isValid(const Entity &entity) const {

            return entity.m_index < m_entitiesVersions.size() && m_entitiesVersions[entity.m_index] == entity.m_version;
        }

        Entity addEntity() {

            if (!m_deletedEntities.empty()) {
                Entity entity = m_deletedEntities.back();
                m_deletedEntities.pop_back();
                ++entity.m_version;
                m_entities.push_back(entity);
            } else {
                std::size_t curEntityIndex = m_entities.size();
                m_entities.push_back(Entity(curEntityIndex, 0, *this));
                m_entitiesVersions.push_back(0);
                m_entitiesComponentsMask.push_back(
                        std::vector<std::size_t>(m_componentsKindsCount, COMPONENT_DOES_NOT_EXIST));
            }

            return m_entities.back();
        }

        void removeEntity(const Entity &entity) {

            assert(entity.isValid() && "invalid entity");

            std::size_t entityIndex = entity.m_index;
            m_deletedEntities.push_back(entity);
            ++m_entitiesVersions[entityIndex];

            auto &componentsMask = m_entitiesComponentsMask[entityIndex];

            //remove all entity's components from the pools
            for (std::size_t i = 0, count = componentsMask.size(); i < count; ++i) {
                std::size_t curComponentIndex = componentsMask[i];
                if (curComponentIndex != COMPONENT_DOES_NOT_EXIST) {
                    //tricky
                    m_pools[i]->removeComponent(curComponentIndex);
                }
            }

            //reset the mask
            componentsMask.assign(componentsMask.size(), COMPONENT_DOES_NOT_EXIST);
        }

        std::size_t getEntitiesCount() const {

            return m_entities.size();
        }

        template<typename C>
        void registerComponentKind() {

            //has the component kind/family been registered yet?
            if (DerivedComponentsPool<C>::index == COMPONENT_DOES_NOT_EXIST) {

                if (m_componentsKindsCount == MAX_COMPONENTS_COUNT) {
                    throw std::runtime_error(
                            "Maximum number of components exceeded: " + std::to_string(MAX_COMPONENTS_COUNT));
                }

                DerivedComponentsPool<C>::index = m_componentsKindsCount++;

#if __cplusplus == 201402L // C++14
                m_pools.emplace_back(std::make_unique<DerivedComponentsPool<C> >());
#else // C++11
                m_pools.emplace_back(make_unique<DerivedComponentsPool<C> >());
#endif
            }
        }

        template<typename C>
        std::size_t getPoolSize() const {

            std::size_t componentKindIndex = DerivedComponentsPool<C>::index;
            if (DerivedComponentsPool<C>::index == COMPONENT_DOES_NOT_EXIST) {
                return 0;
            }

            DerivedComponentsPool<C> *poolC = static_cast< DerivedComponentsPool<C> * >(m_pools[componentKindIndex].get());
            return poolC->getSize();
        }

        template<typename C, typename ... Args>
        void addComponent(const Entity &entity, Args &&... args) {

            assert(entity.isValid() && "invalid entity");

            std::size_t componentKindIndex = DerivedComponentsPool<C>::index;
            if (DerivedComponentsPool<C>::index == COMPONENT_DOES_NOT_EXIST) {
                //an attempt to add non registered component

                registerComponentKind<C>();
                componentKindIndex = DerivedComponentsPool<C>::index;

                for (auto &entityComponentMask: m_entitiesComponentsMask) {
                    entityComponentMask.push_back(COMPONENT_DOES_NOT_EXIST);
                }
            } else {
                if (m_entitiesComponentsMask[entity.m_index][componentKindIndex] != COMPONENT_DOES_NOT_EXIST) {
                    //entity already has a component
                    return;
                }
            }

            DerivedComponentsPool<C> *poolC = static_cast< DerivedComponentsPool<C> * >(m_pools[componentKindIndex].get());
            std::size_t componentIndex = poolC->addComponent(std::forward<Args>(args) ...);
            std::size_t entityIndex = entity.m_index;
            m_entitiesComponentsMask[entityIndex][componentKindIndex] = componentIndex;
        }

        template<typename C>
        void removeComponent(const Entity &entity) {

            assert(entity.isValid() && "invalid entity");

            std::size_t componentKindIndex = DerivedComponentsPool<C>::index;
            if (componentKindIndex == COMPONENT_DOES_NOT_EXIST) {
                return;
            }

            std::size_t entityIndex = entity.m_index;
            auto &entityComponentsMask = m_entitiesComponentsMask[entityIndex];
            std::size_t curComponentIndex = entityComponentsMask[componentKindIndex];

            if (curComponentIndex != COMPONENT_DOES_NOT_EXIST) {
                entityComponentsMask[componentKindIndex] = COMPONENT_DOES_NOT_EXIST;

                //no need to cast, i.e.:
                //DerivedComponentsPool<C> *poolC = static_cast< DerivedComponentsPool<C> * >(_pools[componentKindIndex].get());
                //poolC->removeComponent(curComponentIndex);

                m_pools[componentKindIndex]->removeComponent(curComponentIndex);
            }
        }

        template<typename C>
        bool hasComponent(const Entity &entity) const {

            assert(entity.isValid() && "invalid entity");

            std::size_t componentKindIndex = DerivedComponentsPool<C>::index;
            std::size_t entityIndex = entity.m_index;

            return componentKindIndex != COMPONENT_DOES_NOT_EXIST
                   && m_entitiesComponentsMask[entityIndex][componentKindIndex] != COMPONENT_DOES_NOT_EXIST;
        }

        template<typename C>
        C *getComponent(const Entity &entity) const {

            assert(entity.isValid() && "invalid entity");

            std::size_t componentKindIndex = DerivedComponentsPool<C>::index;

            if (componentKindIndex != COMPONENT_DOES_NOT_EXIST) {

                std::size_t entityIndex = entity.m_index;
                std::size_t componentIndex = m_entitiesComponentsMask[entityIndex][componentKindIndex];

                if (componentIndex != COMPONENT_DOES_NOT_EXIST) {
                    DerivedComponentsPool<C> *poolC = static_cast< DerivedComponentsPool<C> * >(m_pools[componentKindIndex].get());
                    return poolC->getComponent(componentIndex);
                }

                throw std::runtime_error("component not registered");
            }

            throw std::runtime_error("component kind not registered");
        }

        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        Iterator<PredicateAll> begin() {

            PredicateAll<Entity, Storage> predicate(*this, {DerivedComponentsPool<A1>::index,
                                                            DerivedComponentsPool<A2>::index,
                                                            DerivedComponentsPool<A3>::index,
                                                            DerivedComponentsPool<Ax>::index ...});
            return Iterator<PredicateAll>(m_entities.begin(), m_entities.end(), predicate);
        }

        //2 parameters
        template<class A1, class A2>
        Iterator<PredicateTwo> begin() {

            PredicateTwo<Entity, Storage> predicate(*this, DerivedComponentsPool<A1>::index,
                                                    DerivedComponentsPool<A2>::index);
            return Iterator<PredicateTwo>(m_entities.begin(), m_entities.end(), predicate);
        }

        //1 parameter
        template<class A>
        Iterator<PredicateOne> begin() {

            PredicateOne<Entity, Storage> predicate(*this, DerivedComponentsPool<A>::index);
            return Iterator<PredicateOne>(m_entities.begin(), m_entities.end(), predicate);
        }

        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        Iterator<PredicateAll> end() {

            PredicateAll<Entity, Storage> predicate(*this, {DerivedComponentsPool<A1>::index,
                                                            DerivedComponentsPool<A2>::index,
                                                            DerivedComponentsPool<A3>::index,
                                                            DerivedComponentsPool<Ax>::index ...});
            return Iterator<PredicateAll>(m_entities.end(), m_entities.end(), predicate);
        }

        //2 parameters
        template<class A1, class A2>
        Iterator<PredicateTwo> end() {

            PredicateTwo<Entity, Storage> predicate(*this, DerivedComponentsPool<A1>::index,
                                                    DerivedComponentsPool<A2>::index);
            return Iterator<PredicateTwo>(m_entities.end(), m_entities.end(), predicate);
        }

        //1 parameter
        template<class A>
        Iterator<PredicateOne> end() {

            PredicateOne<Entity, Storage> predicate(*this, DerivedComponentsPool<A>::index);
            return Iterator<PredicateOne>(m_entities.end(), m_entities.end(), predicate);
        }

        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        void process(std::function<void(const Entity &entity)> func) {

            PredicateAll<Entity, Storage> predicate(*this, {DerivedComponentsPool<A1>::index,
                                                            DerivedComponentsPool<A2>::index,
                                                            DerivedComponentsPool<A3>::index,
                                                            DerivedComponentsPool<Ax>::index ...});

            for (Iterator<PredicateAll> iterBegin = Iterator<PredicateAll>(m_entities.begin(), m_entities.end(),
                                                                           predicate),
                         iterEnd = Iterator<PredicateAll>(m_entities.end(), m_entities.end(), predicate);
                 iterBegin != iterEnd;
                 ++iterBegin) {
                const Giraffe::Entity &entity = *iterBegin;
                func(entity);
            }
        }

        //2 parameters
        template<class A1, class A2>
        void process(std::function<void(const Entity &entity)> func) {

            PredicateTwo<Entity, Storage> predicate(*this, DerivedComponentsPool<A1>::index,
                                                    DerivedComponentsPool<A2>::index);

            for (Iterator<PredicateTwo> iterBegin = Iterator<PredicateTwo>(m_entities.begin(), m_entities.end(),
                                                                           predicate),
                         iterEnd = Iterator<PredicateTwo>(m_entities.end(), m_entities.end(), predicate);
                 iterBegin != iterEnd;
                 ++iterBegin) {
                const Giraffe::Entity &entity = *iterBegin;
                func(entity);
            }
        }

        //1 parameter
        template<class A>
        void process(std::function<void(const Entity &entity)> func) {

            PredicateOne<Entity, Storage> predicate(*this, DerivedComponentsPool<A>::index);

            for (Iterator<PredicateOne> iterBegin = Iterator<PredicateOne>(m_entities.begin(), m_entities.end(),
                                                                           predicate),
                         iterEnd = Iterator<PredicateOne>(m_entities.end(), m_entities.end(), predicate);
                 iterBegin != iterEnd;
                 ++iterBegin) {
                const Giraffe::Entity &entity = *iterBegin;
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

            PredicateAll<Entity, Storage> predicate(*this, {DerivedComponentsPool<A1>::index,
                                                            DerivedComponentsPool<A2>::index,
                                                            DerivedComponentsPool<A3>::index,
                                                            DerivedComponentsPool<Ax>::index ...});

            return Result<Iterator<PredicateAll>>(
                    Iterator<PredicateAll>(m_entities.begin(), m_entities.end(), predicate),
                    Iterator<PredicateAll>(m_entities.end(), m_entities.end(), predicate)
            );
        }

        //2 parameters
        template<class A1, class A2>
        Result<Iterator<PredicateTwo>> range() {

            PredicateTwo<Entity, Storage> predicate(*this, DerivedComponentsPool<A1>::index,
                                                    DerivedComponentsPool<A2>::index);

            return Result<Iterator<PredicateTwo>>(
                    Iterator<PredicateTwo>(m_entities.begin(), m_entities.end(), predicate),
                    Iterator<PredicateTwo>(m_entities.end(), m_entities.end(), predicate)
            );
        }

        //1 parameter
        template<class A>
        Result<Iterator<PredicateOne>> range() {

            PredicateOne<Entity, Storage> predicate(*this, DerivedComponentsPool<A>::index);

            return Result<Iterator<PredicateOne>>(
                    Iterator<PredicateOne>(m_entities.begin(), m_entities.end(), predicate),
                    Iterator<PredicateOne>(m_entities.end(), m_entities.end(), predicate)
            );
        }

        void process(std::function<void(const Entity &entity)> func) {

            for (auto iterBegin = m_entities.begin(), iterEnd = m_entities.end();
                 iterBegin != iterEnd;
                 ++iterBegin) {
                const Giraffe::Entity &entity = *iterBegin;
                if (entity.isValid()) {
                    func(entity);
                }
            }
        }

        Result<Iterator<PredicateDummy>> range() {

            return Result<Iterator<PredicateDummy>>(
                    Iterator<PredicateDummy>(m_entities.begin(), m_entities.end(), PredicateDummy<Entity, Storage>()),
                    Iterator<PredicateDummy>(m_entities.end(), m_entities.end(), PredicateDummy<Entity, Storage>())
            );
        }

    private:

        std::vector<std::vector<std::size_t> > m_entitiesComponentsMask; //entity id -> components mask
        EntitiesContainerT m_entities;
        std::deque<Entity> m_deletedEntities;
        std::vector<std::size_t> m_entitiesVersions;
        std::vector<std::unique_ptr<ComponentsPool>> m_pools;
        std::size_t m_componentsKindsCount;
    };

    template<typename C, typename ... Args>
    void Entity::addComponent(Args &&... args) {
        m_storage.addComponent<C>(*this, std::forward<Args>(args) ...);
    }

    template<typename C>
    void Entity::removeComponent() {
        m_storage.removeComponent<C>(*this);
    }

    template<typename C>
    C *Entity::getComponent() const {
        return m_storage.getComponent<C>(*this);
    }

    template<typename C>
    bool Entity::hasComponent() const {
        return m_storage.hasComponent<C>(*this);
    }
}

#endif //ECS_STORAGE_H
