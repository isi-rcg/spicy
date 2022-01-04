/*
 *      fm-version.h
 *
 *      Copyright 2012-2013 Andriy Grytsenko (LStranger) <andrej@rep.kiev.ua>
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2.1 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __FM_VERSION_H__
#define __FM_VERSION_H__

#define FM_VERSION_MAJOR 1
#define FM_VERSION_MINOR 3
#define FM_VERSION_MICRO 1

#define FM_CHECK_VERSION(_a,_b,_c) \
    (FM_VERSION_MAJOR > _a || \
    (FM_VERSION_MAJOR == _a && FM_VERSION_MINOR > _b) || \
    (FM_VERSION_MAJOR == _a && FM_VERSION_MINOR == _b && FM_VERSION_MICRO >= _c))

#endif /* __FM_VERSION_H__ */
