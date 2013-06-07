#ifndef ROSE_Combinatorics_H
#define ROSE_Combinatorics_H

#include <algorithm>
#include <cassert>
#include <stdint.h>
#include <vector>

namespace Combinatorics {

/** Returns the factorial of @p n. */
template<typename T>
static T
factorial(T n)
{
    T retval = 1;
    while (n>1) {
        T next = retval * n--;
        assert(next>retval); // overflow
        retval = next;
    }
    return retval;
}

/** Permute a vector according to the specified permutation number. The permutation number should be between zero (inclusive)
 *  and the factorial of the values size (exclusive).  A permutation number of zero is a no-op; higher permutation numbers
 *  shuffle the values in repeatable ways.  Using swap rather that erase/insert is much faster than the standard Lehmer codes,
 *  but doesn't return permutations in lexicographic order.  This function can perform approx 9.6 million permutations per
 *  second on a vector of 12 64-bit integers on Robb's machine (computing all 12! permutations in about 50 seconds). */
template<typename T>
static void
permute(std::vector<T> &values/*in,out*/, uint64_t pn, size_t sz=(size_t)(-1))
{
    if ((size_t)(-1)==sz)
        sz = values.size();
    assert(sz<=values.size());
    assert(pn<factorial(sz));
    for (size_t i=0; i<sz; ++i) {
        uint64_t radix = sz - i;
        uint64_t idx = pn % radix;
        std::swap(values[i+idx], values[i]);
        pn /= radix;
    }
}

} // namespace
#endif
