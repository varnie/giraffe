//
// Created by varnie on 3/22/16.
//

#ifndef GIRAFFE_FILTERITERATOR_H
#define GIRAFFE_FILTERITERATOR_H

#include <utility>
#include <iterator>
#include <type_traits>

namespace Giraffe {

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

}

#endif //GIRAFFE_FILTERITERATOR_H
