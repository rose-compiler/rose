#ifndef ROSE_INTEGEROPS_H
#define ROSE_INTEGEROPS_H

#include <cassert>
#include <limits>
#include <boost/static_assert.hpp>

namespace IntegerOpsPrivate {

    template <typename T>
    struct NumBits {
        BOOST_STATIC_ASSERT (std::numeric_limits<T>::radix == 2);
        BOOST_STATIC_ASSERT (std::numeric_limits<T>::is_integer);
        static const size_t value = std::numeric_limits<T>::digits;
    };

    template <typename T, size_t Count, bool TooBig> struct SHL1Helper;
    template <typename T, size_t Count>
    struct SHL1Helper<T, Count, true> {
        static const T value = 0;
    };
    template <typename T, size_t Count>
    struct SHL1Helper<T, Count, false> {
        static const T value = T(1) << Count;
    };

}

namespace IntegerOps {

/** Bitmask constant with bit @p n set.  Handles the case where @p n is greater than the width of type @p T. */
template <typename T, size_t n>
struct SHL1: public IntegerOpsPrivate::SHL1Helper<T, n, (n >= IntegerOpsPrivate::NumBits<T>::value)> {};

/** Bitmask with bit @p n set.  Handles the case where @p n is greater than the width of type @p T. */
template <typename T>
inline T shl1(size_t n) {
    return (n >= IntegerOpsPrivate::NumBits<T>::value) ? T(0) : (T(1) << n);
}

/** Bit mask constant with bits 0 through @p n-1 set. */
template <typename T, size_t n>
struct GenMask {
    static const T value = SHL1<T, n>::value - T(1);
};

/** Bitmask with bits 0 through @p N-1 set. */
template <typename T>
inline T genMask(size_t n) {
    return shl1<T>(n) - 1;
}

/** Returns true if the sign bit is set, false if clear.
 * @{ */
template <size_t NBits, typename T>
inline bool signBit(T value) {
    return (value & SHL1<T, NBits - 1>::value) != T(0);
}

template <typename T>
inline bool signBit2(T value, size_t width=8*sizeof(T)) {
    assert(width>0 && width<=8*sizeof(T));
    T sign_mask = shl1<T>(width-1);
    return 0 != (value & sign_mask);
}
/** @} */

/** Sign extend value.  If the bit @p FromBits-1 is set set for @p value, then the result will have bits @p FromBits through @p
*  ToBits-1 also set (other bits are unchanged).  If @p ToBits is less than or equal to @p FromBits then nothing happens.
* @{ */
template <size_t FromBits, size_t ToBits, typename T>
inline T signExtend(T value) {
    return value | (signBit<FromBits>(value) ? (GenMask<T, ToBits>::value ^ GenMask<T, FromBits>::value) : T(0));
}

template <typename T>
inline T signExtend2(T value, size_t from_width, size_t to_width) {
    assert(from_width<=8*sizeof(T));
    assert(to_width<=8*sizeof(T));
    return value | (signBit2(value, from_width) ? (genMask<T>(to_width) ^ genMask<T>(from_width)) : T(0));
}
/** @} */

/** Shifts bits of @p value left by @p count bits.
 * @{ */
template <size_t NBits, typename T>
inline T shiftLeft(T value, size_t count) {
    return (value * shl1<T>(count)) & GenMask<T, NBits>::value;
}

template <typename T>
inline T shiftLeft2(T value, size_t count, size_t width=8*sizeof(T)) {
    assert(width>0 && width<=8*sizeof(T));
    return (value * shl1<T>(count)) & genMask<T>(width);
}
/** @} */

/** Shifts bits of @p value right by @p count bits without sign extension.
 * @{ */
template <size_t NBits, typename T>
inline T shiftRightLogical(T value, size_t count) {
    return (count >= NBits) ? T(0) : (value >> count);
}

template <typename T>
inline T shiftRightLogical2(T value, size_t count, size_t width=8*sizeof(T)) {
    assert(width>0 && width<=8*sizeof(T));
    return (count >= width) ? T(0) : (value >> count);
}
/** @} */

/** Shifts bits of @p value right by @p count bits with sign extension.
 * @{ */
template <size_t NBits, typename T>
inline T shiftRightArithmetic(T value, size_t count) {
    if (count >= NBits) {
        return signBit<NBits>(value) ? GenMask<T, NBits>::value : T(0);
    } else {
        return (shiftRightLogical<NBits>(value, count) |
                (signBit<NBits>(value) ? (GenMask<T, NBits>::value ^ genMask<T>(NBits - count)) : T(0)));
    }
}

template <typename T>
inline T shiftRightArithmetic2(T value, size_t count, size_t width=8*sizeof(T)) {
    if (count >= width) {
        return signBit2(value, width) ? genMask<T>(width) : T(0);
    } else {
        return (shiftRightLogical2(value, count, width) |
                (signBit2(value, width) ? (genMask<T>(width) ^ genMask<T>(width-count)) : T(0)));
    }
}
/** @} */

/** Rotate the bits of the value left by count bits.
 * @{ */
template <size_t NBits, typename T>
inline T rotateLeft(T value, size_t count) {
    count %= NBits;
    return ((value << count) | (value >> (NBits - count))) & GenMask<T, NBits>::value;
}

template <typename T>
inline T rotateLeft2(T value, size_t count, size_t width=8*sizeof(T)) {
    assert(width>0 && width<=8*sizeof(T));
    count %= width;
    return ((value << count) | (value >> (width-count))) & genMask<T>(width);
}
/** @} */

/** Rotate bits of the value right by @p count bits.
 * @{ */
template <size_t NBits, typename T>
inline T rotateRight(T value, size_t count) {
    count %= NBits;
    return ((value >> count) | (value << (NBits - count))) & GenMask<T, NBits>::value;
}

template <typename T>
inline T rotateRight2(T value, size_t count, size_t width=8*sizeof(T)) {
    assert(width>0 && width<=8*sizeof(T));
    return ((value >> count) | (value << (width - count))) & genMask<T>(width);
}
/** @} */

/** Returns true if the value is a power of two.  Zero is considered a power of two. */
template <typename T>
inline bool isPowerOfTwo(T value)
{
    if (0 != (value & GenMask<T, 8*sizeof(T)-1>::value))
        value = ~value + 1;
    uint64_t uval = value;
    while (uval) {
        if (uval & 1)
            return 1==uval;
        uval >>= 1u;
    }
    return true; // treat zero as a power of two
}

template <typename T>
inline T log2(T a) {
    T n = T(1);
    T i = 0;
    while (n != 0 && n < a) {
        n <<= 1;
        ++i;
    }
    return i;
}

} // namespace IntegerOps

#endif // ROSE_INTEGEROPS_H
