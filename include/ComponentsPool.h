//
// Created by varnie on 3/20/16.
//

#ifndef ECS_COMPONENTSPOOL_H
#define ECS_COMPONENTSPOOL_H

#include <cstddef> //std::size_t
#include <vector>
#include <deque>
#include <algorithm>
#include <cassert>

#include "./Config.h"

namespace Giraffe {

    class ComponentsPool {
    public:
        virtual ~ComponentsPool() {}
        virtual void removeComponent(std::size_t index) = 0;
    };

    template <class C>
    class DerivedComponentsPool : public ComponentsPool {
    public:
        DerivedComponentsPool();
        ~DerivedComponentsPool();
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
        void removeComponent(std::size_t index) override;
        C *getComponent(std::size_t index);
        std::size_t getSize() const;
        static std::size_t getKindIndex() {
            return index;
        }
        static std::size_t index;
    private:
        struct Chunk {
            char mem[sizeof(C)];
        };

        std::vector<Chunk> m_memoryBlock;
        std::deque<std::size_t> m_deletedComponentsIndexes;
    };

    template<class C>
    std::size_t DerivedComponentsPool<C>::index = COMPONENT_DOES_NOT_EXIST;

    template<class C>
    DerivedComponentsPool<C>::DerivedComponentsPool() :
            ComponentsPool(), m_memoryBlock(), m_deletedComponentsIndexes() { }

    template<class C>
    DerivedComponentsPool<C>::~DerivedComponentsPool() {
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

    template<class C>
    C *DerivedComponentsPool<C>::getComponent(std::size_t index) {
        assert(index < m_memoryBlock.size());
        return reinterpret_cast<C *>(&m_memoryBlock[index]);
    }

    template<class C>
    void DerivedComponentsPool<C>::removeComponent(std::size_t index) {
        assert(index < m_memoryBlock.size());
        C *component = reinterpret_cast<C *>(&m_memoryBlock[index]);
        component->~C();
        m_deletedComponentsIndexes.push_back(index);
    }

    template<class C>
    std::size_t DerivedComponentsPool<C>::getSize() const {
        return m_memoryBlock.size() - m_deletedComponentsIndexes.size();
    }
}

#endif //ECS_COMPONENTSPOOL_H
