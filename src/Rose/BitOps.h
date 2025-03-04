#ifndef ROSE_BitOps_H
#define ROSE_BitOps_H
#include <RoseFirst.h>

#include <Sawyer/Assert.h>
#include <Sawyer/Optional.h>

namespace Rose {

/** Bit operations on unsigned integers.
 *
 * This namespace provides functions that operate at the bit level on unsigned integer types and avoid C/C++ undefined
 * behavior. */
namespace BitOps {

/** Number of bits in a type or value.
 *
 *  @snippet{trimleft} bitOpUnitTests.C nBits */
template<typename Unsigned>
inline size_t nBits(Unsigned = Unsigned(0)) {
    return 8*sizeof(Unsigned);
}

/** Generate a value with all bits set or cleared.
 *
 *  @snippet{trimleft} bitOpUnitTests.C all */
template<typename Unsigned>
inline Unsigned all(bool b = true) {
    return Unsigned(0) - Unsigned(b ? 1 : 0);
}

template<typename Unsigned> inline Unsigned lowMask(size_t n);

/** Set or clear the low-order @p w bits.
 *
 *  Sets or clears the low order bits of the input value without affecting any of the other bits.
 *
 *  @snippet{trimleft} bitOpUnitTests.C allLsb */
template<typename Unsigned>
inline Unsigned allLsb(Unsigned src, size_t w, bool b = true) {
    ASSERT_require(w <= nBits(src));
    if (b) {
        return src | lowMask<Unsigned>(w);
    } else {
        return src & ~lowMask<Unsigned>(w);
    }
}

/** Generate a value with low order bits set.
 *
 *  Returns a value whose @p n low-order bits are set and all the other higher order bits are cleared. If @p n is greater than
 *  or equal to the size of the unsigned type then the returned value has all bits set. If @p n is zero then no bits are set in
 *  the return value.
 *
 *  @snippet{trimleft} bitOpUnitTests.C lowMask */
template<typename Unsigned>
inline Unsigned lowMask(size_t n) {
    return n >= nBits<Unsigned>() ? all<Unsigned>(true) : (Unsigned(1) << n) - Unsigned(1);
}

/** Generate a value with high order bits set.
 *
 *  Returns a value whose @p n high-order bits are set and the other low order bits are cleared.  If @p n is greater than or
 *  equal to the size of the unsigned type then the returned value has all bits set.
 *
 *  @snippet{trimleft} bitOpUnitTests.C highMask */
template<typename Unsigned>
inline Unsigned highMask(size_t n) {
    return n >= nBits<Unsigned>() ? all<Unsigned>(true) : lowMask<Unsigned>(n) << (nBits<Unsigned>() - n);
}

/** Combine two values based on a bit mask.
 *
 *  The return value has bits from @p a and @p b depending on the mask @p cond.  If the mask bit @c i is set, then the return value
 *  bit @c i comes from @p a, otherwise it comes from @p b.
 *
 *  @snippet{trimleft} bitOpUnitTests.C select */
template<typename Unsigned>
inline Unsigned select(Unsigned cond, Unsigned a, Unsigned b) {
    return (a & cond) | (b & ~cond);
}

/** Left shift a value.
 *
 *  The value @p src has its bits shifted @p n places toward higher order. The @n highest order bits are discarded and the @p n
 *  new low-order bits are either set or cleared depending on the value of @p b. If @p n is greater than or equal to the number
 *  of bits in @p src then the return value has either all bits set or all bits cleared depending on @p b.
 *
 *  @snippet{trimleft} bitOpUnitTests.C shiftLeft */
template<typename Unsigned>
inline Unsigned shiftLeft(Unsigned src, size_t n, bool b = false) {
    if (n >= nBits(src)) {
        return all<Unsigned>(b);
    } else {
        return Unsigned(src << n) | (all<Unsigned>(b) & lowMask<Unsigned>(n));
    }
}

/** Left shift part of a value without affecting the rest.
 *
 *  The value @p src has its low-order @p w bits shifted @p n places toward higher order.  The @p n highest bits are discarded and
 *  the @p n new lower order bits are either set or cleared depending on the value of @p b. If @p n is greater than or equal to
 *  @p w then all affected bits are set or cleared, depending on @p b.  The bits not in the affected range are not affected and
 *  are returned.
 *
 *  @snippet{trimleft} bitOpUnitTests.C shiftLeftLsb */
template<typename Unsigned>
inline Unsigned shiftLeftLsb(Unsigned src, size_t w, size_t n, bool b = false) {
    ASSERT_require(w <= nBits(src));
    if (n >= w) {
        return allLsb(src, w, b);
    } else {
        return select(lowMask<Unsigned>(w), shiftLeft(src, n, b), src);
    }
}

/** Right shift a value.
 *
 *  Shift all bits of the value right (to lower indices) by @p n. The @p n low-order bits are discarded and the new @p n
 *  high-order bits are set or cleared depending on @p b.  If @p n is greater than or equal to the size of @p src then the return
 *  value has either all bits set or all bits cleared depending on @p b.
 *
 *  @snippet{trimleft} bitOpUnitTests.C shiftRight */
template<typename Unsigned>
inline Unsigned shiftRight(Unsigned src, size_t n, bool b = false) {
    if (n >= nBits(src)) {
        return all<Unsigned>(b);
    } else {
        return Unsigned(src >> n) | (all<Unsigned>(b) & highMask<Unsigned>(n));
    }
}

/** Right shift part of a value without affecting the rest.
 *
 *  The value @p src has its low-order @p w bits shifted right toward lower order. The @p n lowest bits are discarded and the
 *  @p n new higher order bits are either set or cleared depending on @p b. If @p n is greater than or equal to @p w then all
 *  affected bits are set or cleared, depending on @p b.  The bits not in the affected range are not affected and are returned.
 *
 *  @snippet{trimleft} bitOpUnitTests.C shiftRightLsb */
template<typename Unsigned>
inline Unsigned shiftRightLsb(Unsigned src, size_t w, size_t n, bool b = false) {
    ASSERT_require(w <= nBits(src));
    if (n >= w) {
        return allLsb(src, w, b);
    } else {
        const Unsigned affectedBits = lowMask<Unsigned>(w);
        const Unsigned toShift = select(affectedBits, src, all<Unsigned>(b));
        return select(affectedBits, shiftRight(toShift, n, b), src);
    }
}

/** Generate a single-bit mask.
 *
 *  Returns a value that has all bit cleared except the bit at position @p i. If @p i is outside the valid range of bit
 *  positions for the unsigned type, then zero is returned.
 *
 *  @snippet{trimleft} bitOpUnitTests.C position */
template<typename Unsigned>
inline Unsigned position(size_t i) {
    return i < nBits<Unsigned>() ? shiftLeft(Unsigned(1), i) : Unsigned(0);
}

/** Generate a single-bit mask without affecting the high-order bits.
 *
 *  The low order @p w bits of src are cleared except bit @p i is set, and other bits are not affected.
 *
 *  @snippet{trimleft} bitOpUnitTests.C positionLsb */
template<typename Unsigned>
inline Unsigned positionLsb(Unsigned src, size_t w, size_t i) {
    ASSERT_require(w <= nBits(src));
    return select(lowMask<Unsigned>(w), position<Unsigned>(i), src);
}

/** Generate a mask.
 *
 *  Returns a value where bits @p least through @p greatest (inclusive) are set and all other bits are cleared. The specified
 *  indexes must be valid for the type of @p x. In other words, @p greatest must be less than the number of bits in @p x and @p
 *  greatest must be greater than or equal to @p least.
 *
 *  @snippet{trimleft} bitOpUnitTests.C mask */
template<typename Unsigned>
inline Unsigned mask(size_t least, size_t greatest) {
    ASSERT_require(greatest < nBits<Unsigned>());
    ASSERT_require(greatest >= least);
    return shiftLeft(lowMask<Unsigned>(greatest - least + 1), least);
}

/** Generate a mask without affecting other bits.
 *
 *  Generates a mask limited to the low order @p w bits without affecting the other bits of src.
 *
 *  @snippet{trimleft} bitOpUnitTests.C maskLsb */
template<typename Unsigned>
inline Unsigned maskLsb(Unsigned src, size_t w, size_t least, size_t greatest) {
    ASSERT_require(w <= nBits(src));
    return select(lowMask<Unsigned>(w), mask<Unsigned>(least, greatest), src);
}

/** Extract a single bit.
 *
 *  The bit at position @p i of the value @p src is returned. If @p i is out of range for @p src then zero is returned.
 *
 *  @snippet{trimleft} bitOpUnitTests.C bit */
template<typename Unsigned>
inline bool bit(Unsigned src, size_t i) {
    return i < nBits(src) ? (src & position<Unsigned>(i)) != 0 : false;
}

/** Extract a single bit.
 *
 *  The bit at position @p i of value @p src is returned. If @p i is out of range for the specified value width then zero is
 *  returned.
 *
 *  @snippet{trimleft} bitOpUnitTests.C bitLsb */
template<typename Unsigned>
inline bool bitLsb(Unsigned src, size_t w, size_t i) {
    return i < w ? (src & position<Unsigned>(i)) != 0 : false;
}

/** Most significant bit.
 *
 *  Returns the most significant bit. This is the sign bit for two's complement values.
 *
 *  @snippet{trimleft} bitOpUnitTests.C msb */
template<typename Unsigned>
inline bool msb(Unsigned src) {
    return bit(src, nBits(src) - 1);
}

/** Most significant bit within lsb region.
 *
 *  Returns the value of the most significant bit within the region of @p w low-order bits. If @p w is zero then returns false.
 *
 *  @snippet{trimleft} bitOpUnitTests.C msbLsb */
template<typename Unsigned>
inline bool msbLsb(Unsigned src, size_t w) {
    ASSERT_require(w <= nBits(src));
    return w > 0 ? bit(src, w-1) : false;
}

/** Right shift replicating MSB.
 *
 *  Shift all bits of the value right (to lower indices) by @p n. The @p n low-order bits are discarded and the new @p n
 *  high-order bits are set or cleared depending on the original most significant bit.  If @p n is greater than or equal to the
 *  size of @p src then the return value has either all bits set or all bits cleared depending on its original most significant
 *  bit.
 *
 *  @snippet{trimleft} bitOpUnitTests.C shiftRightSigned */
template<typename Unsigned>
inline Unsigned shiftRightSigned(Unsigned src, size_t n) {
    return shiftRight(src, n, msb(src));
}

/** Right shift low bits without affecting other bits.
 *
 *  Right shift the low-order @p w bits by @p n without affecting higher order bits. The width, @p w, must not be larger than the @p
 *  src width. If @n is greater than or equal to @p w then the @p w low order bits are set or cleared depending on bit
 *  `w-1`. Otherwise, the @p n low order bits are discarded and the @p n new bits introduced at index `w-1` are either zero or one
 *  depending on bit `w-1`.
 *
 *  @snippet{trimleft} bitOpUnitTests.C shiftRightSignedLsb */
template<typename Unsigned>
inline Unsigned shiftRightSignedLsb(Unsigned src, size_t w, size_t n) {
    return shiftRightLsb(src, w, n, msbLsb(src, w));
}

/** Extract part of a value.
 *
 *  Extracts the bits in the range @p least through @p greatest (inclusive) and shifts them right by @p least bits. The @p least and
 *  @p greatest indices must be valid for @p src as defined by @ref mask.
 *
 *  @snippet{trimleft} bitOpUnitTests.C bits */
template<typename Unsigned>
inline Unsigned bits(Unsigned src, size_t least, size_t greatest) {
    return shiftRight(src & mask<Unsigned>(least, greatest), least);
}

/** Extract part of a value limited by width.
 *
 *  Extracts the bits in the range @p least through @p greatest (inclusive) and shifts them right by @p least bits. Any bits of @p
 *  src at index @p w or greater are treated as zeros.
 *
 *  @snippet{trimleft} bitOpUnitTests.C bitsLsb */
template<typename Unsigned>
inline Unsigned bitsLsb(Unsigned src, size_t w, size_t least, size_t greatest) {
    return shiftRight(src & mask<Unsigned>(least, greatest) & lowMask<Unsigned>(w), least);
}


/** Extend or truncate a value.
 *
 *  When the destination type is smaller than the source type, the most significant bits of the source value are discarded,
 *  otherwise the most significant bits of the destination type are set to @p b.
 *
 *  @snippet{trimleft} bitOpUnitTests.C convert */
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
 *  value are all set to the most significant bit of the source value.
 *
 *  @snippet{trimleft} bitOpUnitTests.C convertSigned */
template<typename UnsignedTarget, typename UnsignedSource>
inline UnsignedTarget convertSigned(UnsignedSource x) {
    return convert<UnsignedTarget>(x, msb(x));
}

/** Sign extend part of a value to the full width of the src type.
 *
 *  The low order @p n bits are treated as a signed integer and sign extended to fill the entire width of the return value.
 *
 *  @snippet{trimleft} bitOpUnitTests.C signExtend */
template<typename Unsigned>
inline Unsigned signExtend(Unsigned src, size_t n) {
    if (n < nBits(src)) {
        if (msbLsb(src, n)) {
            src |= mask<Unsigned>(n, nBits(src)-1);
        } else {
            src &= ~mask<Unsigned>(n, nBits(src)-1);
        }
    }
    return src;
}

/** Sign extend part of value without affecting other bits.
 *
 *  Sign extends the low-order @p n bits of the input value to occupy the lower order @p m bits of the output, where @p m
 *  is greater than or equal to @p n and less than or equal to the number of bits in the @p src value.
 *
 *  @snippet{trimleft} bitOpUnitTests.C signExtendLsb */
template<typename Unsigned>
inline Unsigned signExtendLsb(Unsigned src, size_t n, size_t m) {
    ASSERT_require(n > 0);
    ASSERT_require(m >= n);
    ASSERT_require(m <= nBits(src));
    if (m == n) {
        return src;
    } else {
        Unsigned newBitsMask = mask<Unsigned>(n, m-1);
        if (bit(src, n-1)) {
            return src | newBitsMask;
        } else {
            return src & ~newBitsMask;
        }
    }
}

/** Rotate bits left.
 *
 *  Rotates the bits of @p src left (toward higher indices) by @p n bits. This is similar to @ref shiftLeft except the high order
 *  bits that would normally be discarded are reintroduced in the low order positions. If @p n is zero then this is a
 *  no-op. The rotation amount is calculated modulo the width of @p src
 *
 *  @snippet{trimleft} bitOpUnitTests.C rotateLeft */
template<typename Unsigned>
inline Unsigned rotateLeft(Unsigned src, size_t n) {
    n %= nBits(src);
    return shiftLeft(src, n) | shiftRight(src, nBits(src)-n);
}

/** Rotate low-order bits left without affecting others.
 *
 *  Rotates the low-order @p w bits of @p src left by @p n bits without affecting the other bits, and returns the result. The
 *  rotation amount is modulo @p w.  If @p w is zero then the original value is returned.
 *
 *  @snippet{trimleft} bitOpUnitTests.C rotateLeftLsb */
template<typename Unsigned>
inline Unsigned rotateLeftLsb(Unsigned src, size_t w, size_t n) {
    ASSERT_require(w <= nBits(src));
    n = w ? n % w : 0;
    return select(lowMask<Unsigned>(w),
                  shiftLeftLsb(src, w, n) | shiftRightLsb(src, w, w-n),
                  src);
}

/** Rotate bits right.
 *
 *  Rotates the bits of @p src right (toward lower indices) by @p n bits. This is similar to @ref shiftRight except the low order
 *  bits that would normally be discarded are reintroduced in the high order positions. If @p n is zero then this is a no-op.
 *  The rotation amount is calculated modulo the width of @p src.
 *
 *  @snippet{trimleft} bitOpUnitTests.C rotateRight */
template<typename Unsigned>
inline Unsigned rotateRight(Unsigned src, size_t n) {
    n %= nBits(src);
    return shiftRight(src, n) | shiftLeft(src, nBits(src)-n);
}

/** Rotate low-order bits right without affecting others.
 *
 *  Rotates the low-order @p w bits of @p src right by @p n bits without affecting the higher-order bits, and returns the
 *  result.  The rotation amount is modulo @p w. If @p w is zero then the original value is returned.
 *
 *  @snippet{trimleft} bitOpUnitTests.C rotateRightLsb */
template<typename Unsigned>
inline Unsigned rotateRightLsb(Unsigned src, size_t w, size_t n) {
    ASSERT_require(w <= nBits(src));
    n = w ? n % w : 0;
    return select(lowMask<Unsigned>(w),
                  shiftRightLsb(src, w, n) | shiftLeftLsb(src, w, w-n),
                  src);
}

/** Replicate low-order bits to fill return value.
 *
 *  The @p n low-order bits of @p src are repeated as a group as many times as necessary to fill the entire return value. For
 *  instance, if @p src contains 0xabcdef and @p n is 8 and the return type is a 32-bit unsigned integer, then the return value
 *  will be 0xefefefef.  If the width of the return value is not an integer multiple of @p n, then the high order bits of the
 *  return value will contain only some of the lowest order bits of the @p src. The value of @p n cannot be zero.
 *
 *  @snippet{trimleft} bitOpUnitTests.C replicate */
template<typename Unsigned>
inline Unsigned replicate(Unsigned src, size_t n) {
    ASSERT_require(n != 0);
    if (n >= nBits(src)) {
        return src;
    } else {
        size_t ngroups = (nBits(src) + n - 1) / n;
        Unsigned retval = 0;
        for (size_t i = 0; i < ngroups; ++i)
            retval |= shiftLeft(src & lowMask<Unsigned>(n), i*n);
        return retval;
    }
}

/** Replicate low-order bits to fill region without affecting other bits.
 *
 *  This is identical to @ref replicate except that instead of filling the entire return value with the replicated bits, at
 *  most @p w low-order bits of the return value are filled with replicated bits and the remaining high order bits are copied
 *  from @p src.
 *
 *  @snippet{trimleft} bitOpUnitTests.C replicateLsb */
template<typename Unsigned>
inline Unsigned replicateLsb(Unsigned src, size_t w, size_t n) {
    ASSERT_require(w <= nBits(src));
    return select(lowMask<Unsigned>(w), replicate(src, n), src);
}

/** Index of the highest set bit.
 *
 *  If no bits are set then this returns nothing. Otherwise it returns the zero-origin index of the highest order set bit.
 *
 *  @snippet{trimleft} bitOpUnitTests.C highestSetBit */
template<typename Unsigned>
inline Sawyer::Optional<size_t> highestSetBit(Unsigned src) {
    if (src) {
        for (size_t i = nBits(src); i > 0; --i) {
            if (bit(src, i-1))
                return i-1;
        }
    }
    return Sawyer::Nothing();
}

template<typename Unsigned>
inline size_t nSet(Unsigned src) {
    size_t retval = 0;
    while (src != 0) {
        if ((src & 1) != 0)
            ++retval;
        src >>= 1;
    }
    return retval;
}

/** Reverse the bytes.
 *
 *  @snippet{trimleft} bitOpUnitTests.C reverseBytes */
template<class T>
typename std::enable_if<std::is_integral<T>::value, T>::type
inline reverseBytes(const T &x) {
    // In C++23 and later, use std::byteswap
    using Unsigned = typename std::make_unsigned<T>::type;
    Unsigned u = x;
    const size_t n = sizeof(Unsigned);
    for (size_t i = 0; i < n/2; ++i) {
        const size_t loOffset = i*8;
        const size_t hiOffset = nBits<Unsigned>() - i*8 - 8;
        const auto loMask = mask<Unsigned>(loOffset, loOffset+7);
        const auto hiMask = mask<Unsigned>(hiOffset, hiOffset+7);
        const Unsigned lo = (u >> loOffset) & 0xff;
        const Unsigned hi = (u >> hiOffset) & 0xff;

        u &= ~(loMask | hiMask);
        u |= lo << hiOffset;
        u |= hi << loOffset;
    }
    return u;
}

/** True if host is big endian.
 *
 *  @snippet{trimleft} bitOpUnitTests.C isBigEndian */
inline bool
isBigEndian() {
    static const unsigned i = 1;
    return *(unsigned char*)&i == 0;
}

/** True if host is little endian.
 *
 *  @snippet{trimleft} bitOpUnitTests.C isLittleEndian */
inline bool
isLittleEndian() {
    static const unsigned i = 1;
    return *(unsigned char*)&i == 1;
}

/** Convert integral value from host order to big endian.
 *
 *  @snippet{trimleft} bitOpUnitTests.C toBigEndian */
template<class T>
inline typename std::enable_if<std::is_integral<T>::value, T>::type
toBigEndian(const T x) {
    using U = typename std::make_unsigned<T>::type;
    return isBigEndian() ? x : reverseBytes((U)x);
}

/** Convert integral value from host order to little endian.
 *
 *  @snippet{trimleft} bitOpUnitTests.C toLittleEndian */
template<class T>
inline typename std::enable_if<std::is_integral<T>::value, T>::type
toLittleEndian(const T x) {
    using U = typename std::make_unsigned<T>::type;
    return isLittleEndian() ? x : reverseBytes((U)x);
}

/** Convert integral value from big endian to host order.
 *
 *  @snippet{trimleft} bitOpUnitTests.C fromBigEndian */
template<class T>
inline typename std::enable_if<std::is_integral<T>::value, T>::type
fromBigEndian(const T x) {
    using U = typename std::make_unsigned<T>::type;
    return isBigEndian() ? x : reverseBytes((U)x);
}

/** Convert integral value from little endian to host order.
 *
 *  @snippet{trimleft} bitOpUnitTests.C fromLittleEndian */
template<class T>
inline typename std::enable_if<std::is_integral<T>::value, T>::type
fromLittleEndian(const T x) {
    using U = typename std::make_unsigned<T>::type;
    return isLittleEndian() ? x : reverseBytes((U)x);
}

} // namespace
} // namespace
#endif

