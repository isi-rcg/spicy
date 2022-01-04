#ifndef BOOST_SMART_PTR_DETAIL_ATOMIC_COUNT_SYNC_HPP_INCLUDED
#define BOOST_SMART_PTR_DETAIL_ATOMIC_COUNT_SYNC_HPP_INCLUDED

//
//  boost/detail/atomic_count_sync.hpp
//
//  atomic_count for g++ 4.1+
//
//  http://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Atomic-Builtins.html
//
//  Copyright 2007 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#if defined( __ia64__ ) && defined( __INTEL_COMPILER )
# include <ia64intrin.h>
#endif

namespace boost
{

namespace detail
{

class atomic_count
{
public:

    explicit atomic_count( long v ) : value_( v ) {}

    long operator++()
    {
#ifdef __ARM_ARCH_7A__
       int v1, tmp;
       asm volatile ("1:                 \n\t"
                     "ldrex   %0, %1     \n\t"
                     "add     %0 ,%0, #1 \n\t"
                     "strex   %2, %0, %1 \n\t"
                     "cmp     %2, #0     \n\t"
                     "bne     1b         \n\t"
                     : "=&r" (v1), "+Q"(value_), "=&r"(tmp)
                    );
#else
        return __sync_add_and_fetch( &value_, 1 );
#endif
    }

    long operator--()
    {
#ifdef __ARM_ARCH_7A__
       int v1, tmp;
       asm volatile ("1:                 \n\t"
                     "ldrex   %0, %1     \n\t"
                     "sub     %0 ,%0, #1 \n\t"
                     "strex   %2, %0, %1 \n\t"
                     "cmp     %2, #0     \n\t"
                     "bne     1b         \n\t"
                     : "=&r" (v1), "+Q"(value_), "=&r"(tmp)
                    );
       return value_;
#else
        return __sync_add_and_fetch( &value_, -1 );
#endif
    }

    operator long() const
    {
#if __ARM_ARCH_7A__
        return value_;
#else
        return __sync_fetch_and_add( &value_, 0 );
#endif
    }

private:

    atomic_count(atomic_count const &);
    atomic_count & operator=(atomic_count const &);

    mutable long value_;
};

} // namespace detail

} // namespace boost

#endif // #ifndef BOOST_SMART_PTR_DETAIL_ATOMIC_COUNT_SYNC_HPP_INCLUDED
