//
// Created by varnie on 2/23/16.
//

#ifndef ECS_GIRAFFE_H
#define ECS_GIRAFFE_H

#include <cstddef> //std::size_t
#include <limits> //std::numeric_limits
#include <vector> //std::vector
#include <functional> //std::function
#include <algorithm> //std::all_of
#include <memory> //std::unique_ptr, std::make_unique
#include <deque>

#include <utility>
#include <exception> //std::runtime_error, assert
#include <type_traits> //for iterators
#include <string> //std::to_string
#include <cassert>

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

    template<typename T>
    struct Component {
    protected:
        //intended to be deleted through the base class pointer, therefore the dtor is not public and virtual
        ~Component() {}
    };

    class Storage;

    struct Entity {
        Entity(std::size_t index, Storage &storage)
                : m_index(index), m_storage(&storage) { }

        template<typename C, typename ... Args>
        void addComponent(Args &&... args);

        template<typename C>
        void removeComponent();

        template<typename C>
        C *getComponent() const;

        template<typename C>
        bool hasComponent() const;

        std::size_t m_index;
        Storage *m_storage;
    };

    //
    class ComponentsPool {
    public:
        virtual ~ComponentsPool() { }

        virtual void removeComponent(std::size_t index) = 0;
    };

    //
    template<class C>
    class DerivedComponentsPool : public ComponentsPool {
        struct Chunk {
            char mem[sizeof(C)];
        };
    public:
        DerivedComponentsPool() : ComponentsPool(), m_memoryBlock(), m_deletedComponentsIndexes() { }

        ~DerivedComponentsPool() {
            if (!m_memoryBlock.empty()) {
                if (!m_deletedComponentsIndexes.empty()) {
                    size_t index = m_memoryBlock.size() - 1;
                    for (auto rit = m_memoryBlock.rbegin(), rend = m_memoryBlock.rend(); rit != rend; ++rit, --index) {
                        if (std::find(m_deletedComponentsIndexes.begin(), m_deletedComponentsIndexes.end(), index) ==
                            m_deletedComponentsIndexes.end()) {
                            C *component = reinterpret_cast<C *>(&*rit);
                            component->~C();
                        }
                    }
                } else {
                    for (auto rit = m_memoryBlock.rbegin(), rend = m_memoryBlock.rend(); rit != rend; ++rit) {
                        C *component = reinterpret_cast<C *>(&*rit);
                        component->~C();
                    }
                }
            }
        }

        template<typename ... Args>
        std::size_t addComponent(Args &&... args) {

            if (!m_deletedComponentsIndexes.empty()) {
                //reuse old block
                size_t positionIndex = m_deletedComponentsIndexes.back();
                m_deletedComponentsIndexes.pop_back();
                void *block = static_cast<void *> (&m_memoryBlock[positionIndex]);
                new(block) C(std::forward<Args>(args) ...);

                return positionIndex;
            } else {
                //not found, add new block
                m_memoryBlock.emplace_back();

                void *block = static_cast<void *> (&m_memoryBlock.back());
                new(block) C(std::forward<Args>(args) ...);

                return m_memoryBlock.size() - 1;
            }
        }

        void removeComponent(std::size_t index) override {
            assert(index < m_memoryBlock.size());
            C *component = reinterpret_cast<C *>(&m_memoryBlock[index]);
            component->~C();
            m_deletedComponentsIndexes.push_back(index);
        }

        C *getComponent(std::size_t index) {
            assert(index < m_memoryBlock.size());
            return reinterpret_cast<C *>(&m_memoryBlock[index]);
        }

        std::size_t getSize() const {
            return m_memoryBlock.size() - m_deletedComponentsIndexes.size();
        }

        static std::size_t getKindIndex() {
            return index;
        }

        static std::size_t index;
    private:
        std::vector<Chunk> m_memoryBlock;
        std::deque<std::size_t> m_deletedComponentsIndexes;
    };

    template<class C>
    std::size_t DerivedComponentsPool<C>::index = COMPONENT_DOES_NOT_EXIST;
    //

    class Storage {
    public:

        //iterator
        template<class Iterator, class Predicate>
        class FilterIterator {
        public:
            using value_type = typename std::iterator_traits<Iterator>::value_type;
            using reference = typename std::iterator_traits<Iterator>::reference;
            using pointer = typename std::iterator_traits<Iterator>::pointer;

            FilterIterator(Iterator current, Iterator end, Predicate pred) :
                    m_current(current), m_end(end), m_pred(pred) {

                while (m_current != m_end && !m_pred(*m_current)) {
                    ++m_current;
                }
            }

            reference operator*() const { return *m_current; }

            pointer operator->() const { return &*m_current; }

            FilterIterator &operator++() {
                advance();
                return *this;
            }

            FilterIterator operator++(int) {
                FilterIterator obj = *this;
                advance();
                return obj;
            }

            bool operator==(const FilterIterator &rhs) const { return m_current == rhs.m_current; }

            bool operator!=(const FilterIterator &rhs) const { return !(operator==(rhs)); }

        private:
            void advance() {
                do {
                    ++m_current;
                } while (m_current != m_end && !m_pred(*m_current));
            }

            Iterator m_current;
            Iterator m_end;
            Predicate m_pred;
        };
        //iterator

        //predicate
        template<typename T>
        class PredicateAll {
        public:
            PredicateAll(const Storage &storage, const std::initializer_list<std::size_t> &conditions)
                    : m_storage(storage), m_conditions(conditions) { }

            bool operator()(const T &e) const {

                std::size_t entityIndex = e.m_index;
                const auto &entityComponentsMask = m_storage.m_entitiesComponentsMask[entityIndex];

                return std::all_of(m_conditions.cbegin(), m_conditions.cend(),
                                   [&entityComponentsMask](const std::size_t cond) {
                                       return entityComponentsMask[cond] != COMPONENT_DOES_NOT_EXIST;
                                   });
            }

        private:
            const Storage &m_storage;
            std::vector<std::size_t> m_conditions;
        };

        template<typename T>
        class PredicateTwo {
        public:
            PredicateTwo(const Storage &storage, const std::size_t cond1, const std::size_t cond2)
                    : m_storage(storage), m_cond1(cond1), m_cond2(cond2) { }

            bool operator()(const T &e) const {

                std::size_t entityIndex = e.m_index;
                const auto &entityComponentsMask = m_storage.m_entitiesComponentsMask[entityIndex];

                return entityComponentsMask[m_cond1] != COMPONENT_DOES_NOT_EXIST &&
                       entityComponentsMask[m_cond2] != COMPONENT_DOES_NOT_EXIST;
            }

        private:
            const Storage &m_storage;
            std::size_t m_cond1, m_cond2;
        };

        template<typename T>
        class PredicateOne {
        public:
            PredicateOne(const Storage &storage, const std::size_t condition)
                    : m_storage(storage), m_condition(condition) { }

            bool operator()(const T &e) const {

                std::size_t entityIndex = e.m_index;
                const auto &entityComponentsMask = m_storage.m_entitiesComponentsMask[entityIndex];

                return entityComponentsMask[m_condition] != COMPONENT_DOES_NOT_EXIST;
            }

        private:
            const Storage &m_storage;
            std::size_t m_condition;
        };
        //predicate

        using Entities_Container_t = std::deque<Entity>;
        using Entities_Iterator_t = Entities_Container_t::const_iterator;

        template<template<class Item> class Predicate>
        using Iterator = FilterIterator<Entities_Iterator_t, Predicate<Entity> >;

        Storage() : m_entitiesComponentsMask(), m_entities(), m_componentsKindsCount(0), m_deletedEntities(), m_pools() { }

        Entity addEntity() {

            if (!m_deletedEntities.empty()) {
                Entity entity = m_deletedEntities.back();
                m_deletedEntities.pop_back();
                m_entities.push_back(entity);
            } else {
                std::size_t curEntityIndex = m_entities.size();
                m_entities.push_back(Entity(curEntityIndex, *this));
                m_entitiesComponentsMask.push_back(
                        std::vector<std::size_t>(m_componentsKindsCount, COMPONENT_DOES_NOT_EXIST));
            }

            return m_entities.back();
        }

        void removeEntity(const Entity &entity) {

            std::size_t entityIndex = entity.m_index;
            if (entityIndex < m_entities.size()) {
                std::swap(m_entities[entityIndex], m_entities.back());
                m_entities.pop_back();

                m_deletedEntities.push_back(entity);

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
        }

        std::size_t getEntitiesCount() const {

            return m_entities.size();
        }

        template<typename C>
        void registerComponentKind() {

            //
            static_assert(std::is_base_of<Component<C>, C>::value, "CRTP failure");
            //

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

            //
            static_assert(std::is_base_of<Component<C>, C>::value, "CRTP failure");
            //

            std::size_t componentKindIndex = DerivedComponentsPool<C>::getKindIndex();
            if (DerivedComponentsPool<C>::index == COMPONENT_DOES_NOT_EXIST) {
                return 0;
            }

            DerivedComponentsPool<C> *poolC = static_cast< DerivedComponentsPool<C> * >(m_pools[componentKindIndex].get());
            return poolC->getSize();
        }

        //TODO: check that recreation of component on an entity which was deleted, works
        //ie: oldEntity.addComponent<Foo>(); storage.removeEntity(oldEntity); newEntity = storage.addEntity(); newEntity.addComponent<Foo>() puts a newly allocated component instead of an older one
        template<typename C, typename ... Args>
        void addComponent(const Entity &entity, Args &&... args) {

            //
            static_assert(std::is_base_of<Component<C>, C>::value, "CRTP failure");
            //

            std::size_t componentKindIndex = DerivedComponentsPool<C>::getKindIndex();
            if (DerivedComponentsPool<C>::index == COMPONENT_DOES_NOT_EXIST) {
                //an attempt to add non registered component

                registerComponentKind<C>();
                componentKindIndex = DerivedComponentsPool<C>::getKindIndex();

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

            //
            static_assert(std::is_base_of<Component<C>, C>::value, "CRTP failure");
            //

            std::size_t componentKindIndex = DerivedComponentsPool<C>::getKindIndex();
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

            //
            static_assert(std::is_base_of<Component<C>, C>::value, "CRTP failure");
            //

            std::size_t componentKindIndex = DerivedComponentsPool<C>::getKindIndex();
            std::size_t entityIndex = entity.m_index;

            return componentKindIndex != COMPONENT_DOES_NOT_EXIST
                   && m_entitiesComponentsMask[entityIndex][componentKindIndex] != COMPONENT_DOES_NOT_EXIST;
        }

        template<typename C>
        C *getComponent(const Entity &entity) const {

            std::size_t componentKindIndex = DerivedComponentsPool<C>::getKindIndex();

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

    public:
        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        Iterator<PredicateAll> begin() {

            PredicateAll<Entity> predicate(*this, {DerivedComponentsPool<A1>::getKindIndex(),
                                                   DerivedComponentsPool<A2>::getKindIndex(),
                                                   DerivedComponentsPool<A3>::getKindIndex(),
                                                   DerivedComponentsPool<Ax>::getKindIndex() ...});
            return Iterator<PredicateAll>(m_entities.begin(), m_entities.end(), predicate);
        }

        //2 parameters
        template<class A1, class A2>
        Iterator<PredicateTwo> begin() {

            PredicateTwo<Entity> predicate(*this, DerivedComponentsPool<A1>::getKindIndex(),
                                           DerivedComponentsPool<A2>::getKindIndex());
            return Iterator<PredicateTwo>(m_entities.begin(), m_entities.end(), predicate);
        }

        //1 parameter
        template<class A>
        Iterator<PredicateOne> begin() {

            PredicateOne<Entity> predicate(*this, DerivedComponentsPool<A>::getKindIndex());
            return Iterator<PredicateOne>(m_entities.begin(), m_entities.end(), predicate);
        }

        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        Iterator<PredicateAll> end() {

            PredicateAll<Entity> predicate(*this, {DerivedComponentsPool<A1>::getKindIndex(),
                                                   DerivedComponentsPool<A2>::getKindIndex(),
                                                   DerivedComponentsPool<A3>::getKindIndex(),
                                                   DerivedComponentsPool<Ax>::getKindIndex() ...});
            return Iterator<PredicateAll>(m_entities.end(), m_entities.end(), predicate);
        }

        //2 parameters
        template<class A1, class A2>
        Iterator<PredicateTwo> end() {

            PredicateTwo<Entity> predicate(*this, DerivedComponentsPool<A1>::getKindIndex(),
                                           DerivedComponentsPool<A2>::getKindIndex());
            return Iterator<PredicateTwo>(m_entities.end(), m_entities.end(), predicate);
        }

        //1 parameter
        template<class A>
        Iterator<PredicateOne> end() {

            PredicateOne<Entity> predicate(*this, DerivedComponentsPool<A>::getKindIndex());
            return Iterator<PredicateOne>(m_entities.end(), m_entities.end(), predicate);
        }

        //3 or more parameters
        template<class A1, class A2, class A3, class ...Ax>
        void process(std::function<void(const Entity &entity)> func) {

            PredicateAll<Entity> predicate(*this, {DerivedComponentsPool<A1>::getKindIndex(),
                                                   DerivedComponentsPool<A2>::getKindIndex(),
                                                   DerivedComponentsPool<A3>::getKindIndex(),
                                                   DerivedComponentsPool<Ax>::getKindIndex() ...});

            for (Iterator<PredicateAll> iterBegin = Iterator<PredicateAll>(m_entities.begin(), m_entities.end(), predicate),
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

            PredicateTwo<Entity> predicate(*this, DerivedComponentsPool<A1>::getKindIndex(),
                                           DerivedComponentsPool<A2>::getKindIndex());

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

            PredicateOne<Entity> predicate(*this, DerivedComponentsPool<A>::getKindIndex());

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

            PredicateAll<Entity> predicate(*this, {DerivedComponentsPool<A1>::getKindIndex(),
                                                   DerivedComponentsPool<A2>::getKindIndex(),
                                                   DerivedComponentsPool<A3>::getKindIndex(),
                                                   DerivedComponentsPool<Ax>::getKindIndex() ...});

            return Result<Iterator<PredicateAll>>(
                    Iterator<PredicateAll>(m_entities.begin(), m_entities.end(), predicate),
                    Iterator<PredicateAll>(m_entities.end(), m_entities.end(), predicate)
            );
        }

        //2 parameters
        template<class A1, class A2>
        Result<Iterator<PredicateTwo>> range() {

            PredicateTwo<Entity> predicate(*this, DerivedComponentsPool<A1>::getKindIndex(),
                                           DerivedComponentsPool<A2>::getKindIndex());

            return Result<Iterator<PredicateTwo>>(
                    Iterator<PredicateTwo>(m_entities.begin(), m_entities.end(), predicate),
                    Iterator<PredicateTwo>(m_entities.end(), m_entities.end(), predicate)
            );
        }

        //1 parameter
        template<class A>
        Result<Iterator<PredicateOne>> range() {

            PredicateOne<Entity> predicate(*this, DerivedComponentsPool<A>::getKindIndex());

            return Result<Iterator<PredicateOne>>(
                    Iterator<PredicateOne>(m_entities.begin(), m_entities.end(), predicate),
                    Iterator<PredicateOne>(m_entities.end(), m_entities.end(), predicate)
            );
        }

        //methods for retrieving *all* entities
        Entities_Container_t::iterator begin() {

            return m_entities.begin();
        }

        Entities_Container_t::iterator end() {

            return m_entities.end();
        }

        void process(std::function<void(const Entity &entity)> func) {

            for (auto iterBegin = m_entities.begin(), iterEnd = m_entities.end();
                 iterBegin != iterEnd;
                 ++iterBegin) {
                Giraffe::Entity &entity = *iterBegin;
                func(entity);
            }
        }

        Result<Entities_Container_t::iterator> range() {

            return Result<Entities_Container_t::iterator>(
                    Entities_Container_t::iterator(m_entities.begin()),
                    Entities_Container_t::iterator(m_entities.end())
            );
        }

    private:

        std::vector<std::vector<std::size_t> > m_entitiesComponentsMask; //entity id -> components mask
        Entities_Container_t m_entities;
        std::size_t m_componentsKindsCount;
        std::deque<Entity> m_deletedEntities;

        std::vector<std::unique_ptr<ComponentsPool>> m_pools;
    };

    class System {
    public:
        System(Storage &storage) : _storage(storage) { }

        virtual ~System() { }

        virtual void update(float f) = 0;

    protected:
        Storage &_storage;
    };

    template<typename C, typename ... Args>
    void Entity::addComponent(Args &&... args) {
        m_storage->addComponent<C>(*this, std::forward<Args>(args) ...);
    }

    template<typename C>
    void Entity::removeComponent() {
        m_storage->removeComponent<C>(*this);
    }

    template<typename C>
    C *Entity::getComponent() const {
        return m_storage->getComponent<C>(*this);
    }

    template<typename C>
    bool Entity::hasComponent() const {
        return m_storage->hasComponent<C>(*this);
    }
}

#endif //ECS_GIRAFFE_H
