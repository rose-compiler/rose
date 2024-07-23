#ifndef ROSE_BinaryAnalysis_ByteOrder_H
#define ROSE_BinaryAnalysis_ByteOrder_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <Rose/BitOps.h>
#include <ROSE_DEPRECATED.h>

#include <boost/numeric/conversion/cast.hpp>

namespace Rose {
namespace BinaryAnalysis {

/** Definitions dealing with byte order. */
namespace ByteOrder {

/** Byte order. */
enum Endianness {
    // Caution: the symbols LITTLE_ENDIAN and BIG_ENDIAN are already defined on some systems, so we use other names in ROSE.
    ORDER_UNSPECIFIED=0,                                /**< Endianness is unspecified and unknown. */
    ORDER_LSB,                                          /**< Least significant byte first, i.e., little-endian. */
    ORDER_MSB,                                          /**< Most significant byte first, i.e., big-endian. */
    BE = ORDER_MSB,                                     /**< Big-endian. */
    EL = ORDER_LSB,                                     /**< Little-endian. */
};

/** Byte order of host machine. */
Endianness hostOrder();

/** Convert data from one byte order to another in place. */
void convert(void *bytes, size_t nbytes, Endianness from, Endianness to);

/** Reverse bytes. */
template<class T>
typename std::enable_if<std::is_integral<T>::value, T>::type
swapBytes(const T &x) {
    return BitOps::reverseBytes(x);
}

/** Convert a little-endian integer to host order. */
template<class T>
typename std::enable_if<std::is_integral<T>::value, T>::type
leToHost(const T &x) {
    return BitOps::fromLittleEndian(x);
}

/** Convert a big-endian integer to host order. */
template<class T>
typename std::enable_if<std::is_integral<T>::value, T>::type
beToHost(const T &x) {
    return BitOps::fromBigEndian(x);
}

/** Convert specified order to host order. */
template<class T>
typename std::enable_if<std::is_integral<T>::value, T>::type
diskToHost(Endianness sex, const T &x) {
    return ORDER_LSB == sex ? BitOps::fromLittleEndian(x) : BitOps::fromBigEndian(x);
}

/** Convert host order to little-endian.
 *
 *  The source value is first converted to the type of the destination argument and an exception is raised if the
 *  source value is not representable as the destination type. Then the converted source value is byte swapped
 *  if necessary as it is stored in the destination argument.
 *
 * @{ */
template<class Source, class Destination>
void
hostToLe(const Source &src, Destination *dst /*out*/,
         typename std::enable_if<std::is_integral<Source>::value && std::is_integral<Destination>::value>::type* = nullptr) {
    ASSERT_not_null(dst);
    *dst = BitOps::toLittleEndian(boost::numeric_cast<Destination>(src));
}

template<class T>
void
hostToLe(const RelativeVirtualAddress &in, T *out) {
    hostToLe(in.rva(), out);
}
/** @} */

/** Convert host order to big-endian.
 *
 *  The source value is first converted to the type of the destination argument and an exception is raised if the
 *  source value is not representable as the destination type. Then the converted source value is byte swapped
 *  if necessary as it is stored in the destination argument.
 *
 * @{ */
template<class Source, class Destination>
void
hostToBe(const Source &src, Destination *dst,
         typename std::enable_if<std::is_integral<Source>::value && std::is_integral<Destination>::value>::type* = nullptr) {
    ASSERT_not_null(dst);
    *dst = BitOps::toBigEndian(boost::numeric_cast<Destination>(src));
}

template<class T>
void
hostToBe(const RelativeVirtualAddress &in, T *out) {
    hostToBe(in.rva(), out);
}
/** @} */

/** Convert host value to specified endianness.
 *
 *  The source value is first converted to the type of the destination argument and an exception is raised if the
 *  source value is not representable as the destination type. Then the converted source value is byte swapped
 *  if necessary as it is stored in the destination argument. */
template<class Source, class Destination>
void
hostToDisk(Endianness sex, const Source &src, Destination *dst,
           typename std::enable_if<std::is_integral<Source>::value && std::is_integral<Destination>::value>::type* = nullptr) {
    ASSERT_not_null(dst);
    *dst = ORDER_LSB == sex ?
           BitOps::toLittleEndian(boost::numeric_cast<Destination>(src)) :
           BitOps::toBigEndian(boost::numeric_cast<Destination>(src));
}

// [Robb Matzke 2023-06-09]: deprecated
Endianness host_order() ROSE_DEPRECATED("use hostOrder");
int8_t swap_bytes(int8_t n) ROSE_DEPRECATED("use swapBytes");
uint8_t swap_bytes(uint8_t n) ROSE_DEPRECATED("use swapBytes");
int16_t swap_bytes(int16_t n) ROSE_DEPRECATED("use swapBytes");
uint16_t swap_bytes(uint16_t n) ROSE_DEPRECATED("use swapBytes");
int32_t swap_bytes(int32_t n) ROSE_DEPRECATED("use swapBytes");
uint32_t swap_bytes(uint32_t n) ROSE_DEPRECATED("use swapBytes");
int64_t swap_bytes(int64_t n) ROSE_DEPRECATED("use swapBytes");
uint64_t swap_bytes(uint64_t n) ROSE_DEPRECATED("use swapBytes");
int8_t le_to_host(int8_t n) ROSE_DEPRECATED("use leToHost");
uint8_t le_to_host(uint8_t n) ROSE_DEPRECATED("use leToHost");
int16_t le_to_host(int16_t n) ROSE_DEPRECATED("use leToHost");
uint16_t le_to_host(uint16_t n) ROSE_DEPRECATED("use leToHost");
int32_t le_to_host(int32_t n) ROSE_DEPRECATED("use leToHost");
uint32_t le_to_host(uint32_t n) ROSE_DEPRECATED("use leToHost");
int64_t le_to_host(int64_t n) ROSE_DEPRECATED("use leToHost");
uint64_t le_to_host(uint64_t n) ROSE_DEPRECATED("use leToHost");
void host_to_le(unsigned h, uint8_t *n) ROSE_DEPRECATED("use hostToLe");
void host_to_le(unsigned h, uint16_t *n) ROSE_DEPRECATED("use hostToLe");
void host_to_le(unsigned h, uint32_t *n) ROSE_DEPRECATED("use hostToLe");
void host_to_le(rose_addr_t h, uint64_t *n) ROSE_DEPRECATED("use hostToLe");
void host_to_le(int h, int8_t *n) ROSE_DEPRECATED("use hostToLe");
void host_to_le(int h, int16_t *n) ROSE_DEPRECATED("use hostToLe");
void host_to_le(int h, int32_t *n) ROSE_DEPRECATED("use hostToLe");
void host_to_le(int64_t h, int64_t *n) ROSE_DEPRECATED("use hostToLe");
void host_to_le(RelativeVirtualAddress h, uint32_t *n) ROSE_DEPRECATED("use hostToLe");
void host_to_le(RelativeVirtualAddress h, uint64_t *n) ROSE_DEPRECATED("use hostToLe");
uint8_t be_to_host(uint8_t n) ROSE_DEPRECATED("use beToHost");
uint16_t be_to_host(uint16_t n) ROSE_DEPRECATED("use beToHost");
uint32_t be_to_host(uint32_t n) ROSE_DEPRECATED("use beToHost");
uint64_t be_to_host(uint64_t n) ROSE_DEPRECATED("use beToHost");
int8_t be_to_host(int8_t n) ROSE_DEPRECATED("use beToHost");
int16_t be_to_host(int16_t n) ROSE_DEPRECATED("use beToHost");
int32_t be_to_host(int32_t n) ROSE_DEPRECATED("use beToHost");
int64_t be_to_host(int64_t n) ROSE_DEPRECATED("use beToHost");
void host_to_be(unsigned h, uint8_t *n) ROSE_DEPRECATED("use hostToBe");
void host_to_be(unsigned h, uint16_t *n) ROSE_DEPRECATED("use hostToBe");
void host_to_be(unsigned h, uint32_t *n) ROSE_DEPRECATED("use hostToBe");
void host_to_be(rose_addr_t h, uint64_t *n) ROSE_DEPRECATED("use hostToBe");
void host_to_be(int h, int8_t *n) ROSE_DEPRECATED("use hostToBe");
void host_to_be(int h, int16_t *n) ROSE_DEPRECATED("use hostToBe");
void host_to_be(int h, int32_t *n) ROSE_DEPRECATED("use hostToBe");
void host_to_be(int64_t h, int64_t *n) ROSE_DEPRECATED("use hostToBe");
void host_to_be(RelativeVirtualAddress h, uint32_t *n) ROSE_DEPRECATED("use hostToBe");
void host_to_be(RelativeVirtualAddress h, uint64_t *n) ROSE_DEPRECATED("use hostToBe");
uint8_t disk_to_host(Endianness sex, uint8_t n) ROSE_DEPRECATED("use diskToHost");
uint16_t disk_to_host(Endianness sex, uint16_t n) ROSE_DEPRECATED("use diskToHost");
uint32_t disk_to_host(Endianness sex, uint32_t n) ROSE_DEPRECATED("use diskToHost");
uint64_t disk_to_host(Endianness sex, uint64_t n) ROSE_DEPRECATED("use diskToHost");
int8_t disk_to_host(Endianness sex, int8_t n) ROSE_DEPRECATED("use diskToHost");
int16_t disk_to_host(Endianness sex, int16_t n) ROSE_DEPRECATED("use diskToHost");
int32_t disk_to_host(Endianness sex, int32_t n) ROSE_DEPRECATED("use diskToHost");
int64_t disk_to_host(Endianness sex, int64_t n) ROSE_DEPRECATED("use diskToHost");
void host_to_disk(Endianness sex, unsigned h, uint8_t *np) ROSE_DEPRECATED("use hostToDisk");
void host_to_disk(Endianness sex, unsigned h, uint16_t *np) ROSE_DEPRECATED("use hostToDisk");
void host_to_disk(Endianness sex, unsigned h, uint32_t *np) ROSE_DEPRECATED("use hostToDisk");
void host_to_disk(Endianness sex, rose_addr_t h, uint64_t *np) ROSE_DEPRECATED("use hostToDisk");
void host_to_disk(Endianness sex, RelativeVirtualAddress h, uint64_t *np) ROSE_DEPRECATED("use hostToDisk");
void host_to_disk(Endianness sex, int h, int8_t *np) ROSE_DEPRECATED("use hostToDisk");
void host_to_disk(Endianness sex, int h, int16_t *np) ROSE_DEPRECATED("use hostToDisk");
void host_to_disk(Endianness sex, int h, int32_t *np) ROSE_DEPRECATED("use hostToDisk");
void host_to_disk(Endianness sex, int64_t h, int64_t *np) ROSE_DEPRECATED("use hostToDisk");

} // namespace
} // namespace
} // namespace

#endif
#endif
