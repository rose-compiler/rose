#ifndef ROSE_INTEGEROPS_H
#define ROSE_INTEGEROPS_H

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

template <size_t NBits, typename T>
inline bool signBit(T value) {
    return (value & SHL1<T, NBits - 1>::value) != T(0);
}

/** Sign extend value.  If the bit @p FromBits-1 is set set for @p value, then the result will have bits @p FromBits through @p
*  ToBits-1 also set (other bits are unchanged).  If @p ToBits is less than or equal to @p FromBits then nothing happens. */
template <size_t FromBits, size_t ToBits, typename T>
inline T signExtend(T value) {
    return value | (signBit<FromBits>(value) ? (GenMask<T, ToBits>::value ^ GenMask<T, FromBits>::value) : T(0));
}

template <size_t NBits, typename T>
inline T shiftLeft(T value, size_t count) {
    return (value * shl1<T>(count)) & GenMask<T, NBits>::value;
}

template <size_t NBits, typename T>
inline T shiftRightLogical(T value, size_t count) {
    return (count >= NBits) ? T(0) : (value >> count);
}

template <size_t NBits, typename T>
inline T shiftRightArithmetic(T value, size_t count) {
    if (count >= NBits) {
        return signBit<NBits>(value) ? GenMask<T, NBits>::value : T(0);
    } else {
        return (shiftRightLogical<NBits>(value, count) |
                (signBit<NBits>(value) ? (GenMask<T, NBits>::value ^ genMask<T>(NBits - count)) : T(0)));
    }
}

template <size_t NBits, typename T>
inline T rotateLeft(T value, size_t count) {
    count %= NBits;
    return ((value << count) | (value >> (NBits - count))) & GenMask<T, NBits>::value;
}

template <size_t NBits, typename T>
inline T rotateRight(T value, size_t count) {
    count %= NBits;
    return ((value >> count) | (value << (NBits - count))) & GenMask<T, NBits>::value;
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
