//
// Created by varnie on 3/21/16.
//

#include "../include/Storage.h"

Giraffe::Entity::Entity(std::size_t index, std::size_t version, Giraffe::Storage &storage)
    : m_index(index), m_version(version), m_storage(storage) { }

bool Giraffe::Entity::isValid() const {
    return m_storage.isValid(*this);
}