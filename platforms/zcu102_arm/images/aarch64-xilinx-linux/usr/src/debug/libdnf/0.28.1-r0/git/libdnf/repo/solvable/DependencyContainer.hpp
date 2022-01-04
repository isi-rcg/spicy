/*
 * Copyright (C) 2018 Red Hat, Inc.
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef LIBDNF_DEPENDENCYCONTAINER_HPP
#define LIBDNF_DEPENDENCYCONTAINER_HPP


#include <solv/queue.h>
#include <memory>
#include "libdnf/dnf-sack.h"

namespace libdnf {

struct Dependency;

struct DependencyContainer
{
public:
    DependencyContainer(const DependencyContainer &src);
    explicit DependencyContainer(DnfSack *sack);
    DependencyContainer(DnfSack *sack, Queue queue);
    ~DependencyContainer();

    DependencyContainer &operator=(DependencyContainer &&src) noexcept;
    bool operator==(const DependencyContainer &r) const;
    bool operator!=(const DependencyContainer &r) const;

    void add(Dependency *dependency);
    void add(Id id);

    /**
    * @brief Adds a reldep from Char*. Only globs in name are proccessed. The proccess is slow
    * therefore if reldepStr is not a glob please use addReldep() instead.
    *
    * @param reldepStr p_reldepStr: Char*
    * @return bool - false if parsing or reldep creation fails
    */
    bool addReldepWithGlob(const char *reldepStr);

    /**
    * @brief Adds a reldep from Char*. It does not support globs.
    *
    * @param reldepStr p_reldepStr: Char*
    * @return bool false if parsing or reldep creation fails
    */
    bool addReldep(const char *reldepStr);
    void extend(DependencyContainer *container);

    std::unique_ptr<Dependency> get(int index) const noexcept;
    Dependency *getPtr(int index) const noexcept;
    Id getId(int index) const noexcept;
    int count() const noexcept;

    const Queue &getQueue() const noexcept;

private:
    DnfSack *sack;
    Queue queue;
};

inline Id DependencyContainer::getId(int index) const noexcept
{
    return queue.elements[index];
}

}

#endif //LIBDNF_DEPENDENCYCONTAINER_HPP
