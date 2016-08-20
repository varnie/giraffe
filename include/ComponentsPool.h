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
#include <utility>      // std::pair

#include "./Config.h"

namespace Giraffe {

    template <class C>
    class ComponentsPool {
    public:
        ComponentsPool();
        
        ~ComponentsPool();
        
        template<typename ... Args>
        std::pair<std::size_t, C*> addComponent(Args &&... args);

        void removeComponent(std::size_t index);

        C *getComponent(std::size_t index);

        std::size_t getSize() const;

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
                new(&m_mem[itemIndexInLink]) C(std::forward<Args>(args) ...);
            }
        };

        std::vector<std::unique_ptr<Link>> m_links;
        Link *m_curLink;
    };

    template<class C>
    template<typename ... Args>
    std::pair<std::size_t, C*> ComponentsPool<C>::addComponent(Args &&... args) {

        if (!m_deletedComponentsIndexes.empty()) {
            //reuse old item
            size_t positionIndex = m_deletedComponentsIndexes.back();
            m_deletedComponentsIndexes.pop_back();

            std::size_t linkIndex = positionIndex / Giraffe::POOL_COMPONENTS_PER_CHUNK;
            std::size_t itemIndexInLink = positionIndex % Giraffe::POOL_COMPONENTS_PER_CHUNK;

            assert(linkIndex < m_links.size());

            m_links[linkIndex]->addComponent(itemIndexInLink, std::forward<Args>(args) ...);

            C *component = reinterpret_cast<C *>(&m_links[linkIndex]->m_mem[itemIndexInLink]);

            return std::make_pair(positionIndex, component);
        } else {
            if (m_curLink->m_used == Giraffe::POOL_COMPONENTS_PER_CHUNK) {
                //the last link is filled; add new link

                m_links.emplace_back(std::make_unique<ComponentsPool<C>::Link>());

                m_curLink->m_next = m_links.back().get();
                m_curLink = m_curLink->m_next;
            }

            m_curLink->addComponent(m_curLink->m_used, std::forward<Args>(args) ...);
            C *component = reinterpret_cast<C *>(&m_curLink->m_mem[m_curLink->m_used]);
            ++m_curLink->m_used;

            std::size_t positionIndex = (m_links.size() - 1)*Giraffe::POOL_COMPONENTS_PER_CHUNK + (m_curLink->m_used - 1);
            return std::make_pair(positionIndex, component);
        }
    }

    template<class C>
    std::size_t ComponentsPool<C>::index = COMPONENT_DOES_NOT_EXIST;

    template<class C>
    ComponentsPool<C>::ComponentsPool() : m_deletedComponentsIndexes(),
        m_links(), m_curLink(nullptr) {

        m_links.emplace_back(std::make_unique<ComponentsPool<C>::Link>());

        m_curLink = m_links.back().get();
    }

    template<class C>
    ComponentsPool<C>::~ComponentsPool() {

        if (!m_deletedComponentsIndexes.empty()) {
            std::size_t index = 0;
            for (const auto &link : m_links) {
                std::size_t indexStart = index * Giraffe::POOL_COMPONENTS_PER_CHUNK;
                for (std::size_t i = 0; i < link->m_used; ++i) {
                    std::size_t itemIndexInLink = indexStart + i;
                    if (std::find(m_deletedComponentsIndexes.begin(), m_deletedComponentsIndexes.end(),
                                  itemIndexInLink) == m_deletedComponentsIndexes.end()) {
                        C *component = reinterpret_cast<C *>(&link->m_mem[i]);
                        component->~C();
                    }
                }

                ++index;
            }
        } else {
            for (const auto &link : m_links) {
                for (std::size_t i = 0; i < link->m_used; ++i) {
                    C *component = reinterpret_cast<C *>(&link->m_mem[i]);
                    component->~C();
                }
            }
        }
    }

    template<class C>
    C *ComponentsPool<C>::getComponent(std::size_t index) {

        std::size_t linkIndex = index / Giraffe::POOL_COMPONENTS_PER_CHUNK;
        std::size_t itemIndexInLink = index % Giraffe::POOL_COMPONENTS_PER_CHUNK;

        assert(linkIndex < m_links.size());

        return reinterpret_cast<C *>(&m_links[linkIndex]->m_mem[itemIndexInLink]);
    }

    template<class C>
    void ComponentsPool<C>::removeComponent(std::size_t index) {

        const C *component = getComponent(index);
        component->~C();
        m_deletedComponentsIndexes.push_back(index);
    }

    template<class C>
    std::size_t ComponentsPool<C>::getSize() const {

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
