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

#ifndef LIBDNF_DEPENDENCY_SPLITTER_HPP
#define LIBDNF_DEPENDENCY_SPLITTER_HPP

#include <string>

#include "../dnf-types.h"

namespace libdnf {

struct DependencySplitter
{
public:
    /**
    * @brief Parse realdep char* into thee elements (name, evr, and comparison type), and transforms
    * into int (HY_EQ, HY_LT, HY_GT, and their combinations).
    *
    * @param reldepStr p_reldepStr: Char * that represent reldep
    * @return bool - true if parsing was succesful
    */
    bool parse(const char * reldepStr);
    const std::string & getName() const noexcept;
    const char * getNameCStr() const noexcept;
    const std::string & getEVR() const noexcept;
    const char * getEVRCStr() const noexcept;
    int getCmpType() const noexcept;

private:
    std::string name;
    std::string evr;
    int cmpType{0};
};

inline const std::string & DependencySplitter::getName() const noexcept
{
    return name;
}

inline const std::string & DependencySplitter::getEVR() const noexcept
{
    return evr;
}

inline int DependencySplitter::getCmpType() const noexcept
{
    return cmpType;
}

inline const char * DependencySplitter::getNameCStr() const noexcept
{
    return name.empty() ? NULL : name.c_str();
}

inline const char * DependencySplitter::getEVRCStr() const noexcept
{
    return evr.empty() ? NULL : evr.c_str();
}

}

#endif // LIBDNF_DEPENDENCY_SPLITTER_HPP
