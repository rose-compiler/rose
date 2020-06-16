#ifndef ROSE_BitOps_H
#define ROSE_BitOps_H

#include <Sawyer/Assert.h>

namespace Rose {

/** Bit operations on unsigned integers.
 *
 * This namespace provides functions that operate at the bit level on unsigned integer types and avoid C/C++ undefined
 * behavior. */
namespace BitOps {

/** Number of bits in a type or value. */
template<typename Unsigned>
inline size_t nBits(Unsigned x = Unsigned(0)) {
    return 8*sizeof(Unsigned);
}

/** Generate a value with all bits set or cleared. */
template<typename Unsigned>
inline Unsigned all(bool b = true) {
    return Unsigned(0) - Unsigned(b ? 1 : 0);
}

/** Generate a value with low order bits set.
 *
 *  Returns a value whose @p n low-order bits are set and all the other higher order bits are cleared. If @p n is greater than
 *  or equal to the size of the unsigned type then the returned value has all bits set. */
template<typename Unsigned>
inline Unsigned lowMask(size_t n) {
    return n >= nBits<Unsigned>() ? all<Unsigned>(true) : (Unsigned(1) << n) - Unsigned(1);
}

/** Generate a value with high order bits set.
 *
 *  Returns a value whose @p n high-order bits are set and the other low order bits are cleared.  If @p n is greater than or
 *  equal to the size of the unsigned type then the returned value has all bits set. */
template<typename Unsigned>
inline Unsigned highMask(size_t n) {
    return n >= nBits<Unsigned>() ? all<Unsigned>(true) : lowMask<Unsigned>(n) << (nBits<Unsigned>() - n);
}

/** Left shift a value.
 *
 *  The value @p x has its bits shifted @p n places toward higher order. The @n highest order bits are discarded and the @p n
 *  new low-order bits are either set or cleared depending on the value of @p b. If @p n is greater than or equal to the number
 *  of bits in @p x then the return value has either all bits set or all bits cleared depending on @p b. */
template<typename Unsigned>
inline Unsigned shiftLeft(Unsigned x, size_t n, bool b = false) {
    if (n >= nBits(x)) {
        return all<Unsigned>(b);
    } else {
        return Unsigned(x << n) | (all<Unsigned>(b) & lowMask<Unsigned>(n));
    }
}

/** Right shift a value.
 *
 *  Shift all bits of the value right (to lower indices) by @p n. The @p n low-order bits are discarded and the new @p n
 *  high-order bits are set or cleared depending on @p b.  If @p n is greater than or equal to the size of @p x then the return
 *  value has either all bits set or all bits cleared depending on @p b. */
template<typename Unsigned>
inline Unsigned shiftRight(Unsigned x, size_t n, bool b = false) {
    if (n >= nBits(x)) {
        return all<Unsigned>(b);
    } else {
        return Unsigned(x >> n) | (all<Unsigned>(b) & highMask<Unsigned>(n));
    }
}

/** Generate a single-bit mask.
 *
 *  Returns a value that has all bit cleared except the bit at position @p i. If @p i is outside the valid range of bit
 *  positions for the unsigned type, then zero is returned. */
template<typename Unsigned>
inline Unsigned position(size_t i) {
    return i < nBits<Unsigned>() ? shiftLeft(Unsigned(1), i) : Unsigned(0);
}

/** Generate a mask.
 *
 *  Returns a value where bits @p least through @p greatest (inclusive) are set and all other bits are cleared. The specified
 *  indexes must be valid for the type of @p x. In other words, @p greatest must be less than the number of bits in @p x and @p
 *  greatest must be greater than or equal to @p least. */
template<typename Unsigned>
inline Unsigned mask(size_t least, size_t greatest) {
    ASSERT_require(greatest < nBits<Unsigned>());
    ASSERT_require(greatest >= least);
    return shiftLeft(lowMask<Unsigned>(greatest - least + 1), least);
}

/** Extract a single bit.
 *
 *  The bit at position @p i of the value @p x is returned. If @p i is out of range for @p x then zero is returned. */
template<typename Unsigned>
inline bool bit(Unsigned x, size_t i) {
    return i < nBits(x) ? (x & position<Unsigned>(i)) != 0 : false;
}

/** Most significant bit.
 *
 *  Returns the most significant bit. This is the sign bit for two's complement values. */
template<typename Unsigned>
inline bool msb(Unsigned x) {
    return bit(x, nBits(x) - 1);
}

/** Right shift replicating MSB.
 *
 *  Shift all bits of the value right (to lower indices) by @p n. The @p n low-order bits are discarded and the new @p n
 *  high-order bits are set or cleared depending on the original most significant bit.  If @p n is greater than or equal to the
 *  size of @p x then the return value has either all bits set or all bits cleared depending on its original most significant
 *  bit. */
template<typename Unsigned>
inline Unsigned shiftRightSigned(Unsigned x, size_t n) {
    return shiftRight(x, n, msb(x));
}

/** Extract part of a value.
 *
 *  Extracts the bits in the range @p least through @greatest (inclusive) and shifts them right by @p least bits. The @p least
 *  and @p greatest indices must be value for @p x as defined by @ref mask. */
template<typename Unsigned>
inline Unsigned bits(Unsigned x, size_t least, size_t greatest) {
    return shiftRight(x & mask<Unsigned>(least, greatest), least);
}

/** Extend or truncate a value.
 *
 *  When the destination type is smaller than the source type, the most significant bits of the source value are discarded,
 *  otherwise the most significant bits of the destination type are set to @p b. */
template<typename UnsignedTarget, typename UnsignedSource>
inline UnsignedTarget convert(UnsignedSource x, bool b = false) {
    if (nBits(x) < nBits<UnsignedTarget>()) {
        // extending
        return UnsignedTarget(x) | (all<UnsignedTarget>(b) & ~lowMask<UnsignedTarget>(nBits(x)));
    } else {
        // truncating
        return UnsignedTarget(x & lowMask<UnsignedSource>(nBits<UnsignedTarget>()));
    }
}

/** Sign extend or truncate a value.
 *
 *  This is identical to @ref convert except when the target value is wider than the source value the new bits of the return
 *  value are all set to the most significant bit of the source value. */
template<typename UnsignedTarget, typename UnsignedSource>
inline UnsignedTarget convertSigned(UnsignedSource x) {
    return convert<UnsignedTarget>(x, msb(x));
}

/** Rotate bits left.
 *
 *  Rotates the bits of @p x left (toward higher indices) by @p n bits. This is similar to @ref shiftLeft except the high order
 *  bits that would normally be discarded are reintroduced in the low order positions. If @p n is zero then this is a
 *  no-op. The rotation amount is calculated modulo the width of @p x. */
template<typename Unsigned>
inline Unsigned rotateLeft(Unsigned x, size_t n) {
    n %= nBits(x);
    return shiftLeft(x, n) | (shiftRight(x, nBits(x)-n) & lowMask<Unsigned>(n));
}

/** Rotate bits right.
 *
 *  Rotates the bits of @p x right (toward lower indices) by @p n bits. This is similar to @ref shiftRight except the low order
 *  bits that would normally be discarded are reintroduced in the high order positions. If @p n is zero then this is a no-op.
 *  The rotation amount is calculated modulo the width of @p x. */
template<typename Unsigned>
inline Unsigned rotateRight(Unsigned x, size_t n) {
    n %= nBits(x);
    return shiftRight(x, n) | (shiftLeft(x, nBits(x)-n) & highMask<Unsigned>(n));
}

} // namespace
} // namespace
#endif

