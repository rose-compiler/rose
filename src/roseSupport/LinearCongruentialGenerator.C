#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <stdlib.h>

#include "LinearCongruentialGenerator.h"

uint64_t
LinearCongruentialGenerator::next()
{
    // These are the values used by MMIX written by Donald Knuth. All 64 bits are returned.
    value_ = 6364136223846793005ull * value_ + 1442695040888963407ull;
    return value_;
}

uint64_t
LinearCongruentialGenerator::max()
{
    // all 64 bits are returned by next()
    return UINT64_MAX;
}
