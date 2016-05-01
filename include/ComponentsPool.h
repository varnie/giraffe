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
#include <type_traits> //std::aligned_storage
#include <memory>

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
        std::size_t addComponent(Args &&... args);

        void removeComponent(std::size_t index) override;

        C *getComponent(std::size_t index);

        std::size_t getSize() const;

        static std::size_t getKindIndex() {
            return index;
        }
        static std::size_t index;
    private:

        std::deque<std::size_t> m_deletedComponentsIndexes;
        
        struct Link {
            typename std::aligned_storage<sizeof(C), alignof(C)>::type m_mem[Giraffe::POOL_COMPONENTS_PER_CHUNK];
            std::size_t m_used;
            Link *m_next;

            Link():m_used(0), m_next(nullptr) {}

            template<typename ... Args>
            void addComponent(std::size_t itemIndexInLink, Args &&... args) {
                void *block = static_cast<void *>(&m_mem[itemIndexInLink]);
                new(block) C(std::forward<Args>(args) ...);
            }
        };

        std::vector<std::unique_ptr<Link>> m_links;
        Link *m_curLink;
    };

    template<class C>
    template<typename ... Args>
    std::size_t DerivedComponentsPool<C>::addComponent(Args &&... args) {

        if (!m_deletedComponentsIndexes.empty()) {
            //reuse old item
            size_t positionIndex = m_deletedComponentsIndexes.back();
            m_deletedComponentsIndexes.pop_back();

            std::size_t linkIndex = positionIndex / Giraffe::POOL_COMPONENTS_PER_CHUNK;
            std::size_t itemIndexInLink = positionIndex % Giraffe::POOL_COMPONENTS_PER_CHUNK;

            assert(linkIndex < m_links.size());

            m_links[linkIndex]->addComponent(itemIndexInLink, std::forward<Args>(args) ...);

            return positionIndex;
        } else {
            if (m_curLink->m_used == Giraffe::POOL_COMPONENTS_PER_CHUNK) {
                //the last link is filled; add new link

#if __cplusplus == 201402L // C++14
                m_links.emplace_back(std::make_unique<DerivedComponentsPool<C>::Link>());
#else // C++11
                m_links.emplace_back(make_unique<DerivedComponentsPool<C>::Link>());
#endif

                m_curLink->m_next = m_links.back().get();
                m_curLink = m_curLink->m_next;
            }

            m_curLink->addComponent(m_curLink->m_used, std::forward<Args>(args) ...);
            ++m_curLink->m_used;

            return (m_links.size() - 1)*Giraffe::POOL_COMPONENTS_PER_CHUNK + (m_curLink->m_used - 1);
        }
    }

    template<class C>
    std::size_t DerivedComponentsPool<C>::index = COMPONENT_DOES_NOT_EXIST;

    template<class C>
    DerivedComponentsPool<C>::DerivedComponentsPool() :
        ComponentsPool(), m_deletedComponentsIndexes(),
        m_links(), m_curLink(nullptr) {

#if __cplusplus == 201402L // C++14
        m_links.emplace_back(std::make_unique<DerivedComponentsPool<C>::Link>());
#else // C++11
        m_links.emplace_back(make_unique<DerivedComponentsPool<C>::Link>());
#endif

        m_curLink = m_links.back().get();
    }

    template<class C>
    DerivedComponentsPool<C>::~DerivedComponentsPool() {

        if (!m_deletedComponentsIndexes.empty()) {
            std::size_t index = 0;
            for (const auto &link : m_links) {
                std::size_t indexStart = index * Giraffe::POOL_COMPONENTS_PER_CHUNK;
                for (std::size_t i = 0; i < link->m_used; ++i) {
                    std::size_t itemIndexInLink = indexStart + i;
                    if (std::find(m_deletedComponentsIndexes.begin(), m_deletedComponentsIndexes.end(),
                                  itemIndexInLink) == m_deletedComponentsIndexes.end()) {
                        void *block = static_cast<void *>(&link->m_mem[i]);
                        C *component = reinterpret_cast<C *>(block);
                        component->~C();
                    }
                }

                ++index;
            }
        } else {
            for (const auto &link : m_links) {
                for (std::size_t i = 0; i < link->m_used; ++i) {
                    void *block = static_cast<void *>(&link->m_mem[i]);
                    C *component = reinterpret_cast<C *>(block);
                    component->~C();
                }
            }
        }
    }

    template<class C>
    C *DerivedComponentsPool<C>::getComponent(std::size_t index) {

        std::size_t linkIndex = index / Giraffe::POOL_COMPONENTS_PER_CHUNK;
        std::size_t itemIndexInLink = index % Giraffe::POOL_COMPONENTS_PER_CHUNK;

        assert(linkIndex < m_links.size());
        const auto *pLink = m_links[linkIndex].get();

        return reinterpret_cast<C *>(&pLink->m_mem[itemIndexInLink]);
    }

    template<class C>
    void DerivedComponentsPool<C>::removeComponent(std::size_t index) {

        C *component = getComponent(index);
        component->~C();
        m_deletedComponentsIndexes.push_back(index);
    }

    template<class C>
    std::size_t DerivedComponentsPool<C>::getSize() const {

        std::size_t result = 0;
        const auto *link = m_links[0].get();
        while (link != nullptr) {
            result += link->m_used;
            link = link->m_next;
        }

        result -= m_deletedComponentsIndexes.size();

        return result;
    }
}

#endif //ECS_COMPONENTSPOOL_H
