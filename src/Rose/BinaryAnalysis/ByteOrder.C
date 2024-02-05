#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"
#include "ByteOrder.h"
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <Rose/BitOps.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ByteOrder {

Endianness
hostOrder() {
    return BitOps::isLittleEndian() ? ORDER_LSB : ORDER_MSB;
}

void
convert(void *bytes_, size_t nbytes, Endianness from, Endianness to) {
    if (ORDER_UNSPECIFIED == from || ORDER_UNSPECIFIED == to)
        return;

    if (from != to) {
        assert(ORDER_LSB == from || ORDER_MSB == from);
        assert(ORDER_LSB == to   || ORDER_MSB == to);
        uint8_t *bytes = (uint8_t*)bytes_;
        size_t nswaps = nbytes / 2;
        for (size_t i = 0; i < nswaps; ++i)
            std::swap(bytes[i], bytes[nbytes - (i + 1)]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The rest of this file is deprecated [Robb Matzke 2023-06-09]
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// [Robb Matzke 2023-06-09]: deprecated
Endianness host_order() {
    return hostOrder();
}

// [Robb Matzke 2023-06-09]: deprecated
uint8_t
swap_bytes(uint8_t n) {
    return n;
}

// [Robb Matzke 2023-06-09]: deprecated
uint16_t
swap_bytes(uint16_t n) {
    return swapBytes(n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint32_t
swap_bytes(uint32_t n) {
    return swapBytes(n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint64_t
swap_bytes(uint64_t n) {
    return swapBytes(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int8_t
swap_bytes(int8_t n) {
    return swapBytes(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int16_t
swap_bytes(int16_t n) {
    return swapBytes(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int32_t
swap_bytes(int32_t n) {
    return swapBytes(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int64_t
swap_bytes(int64_t n) {
    return swapBytes(n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint8_t
le_to_host(uint8_t n) {
    return leToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint16_t
le_to_host(uint16_t n) {
    return leToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint32_t
le_to_host(uint32_t n) {
    return leToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint64_t
le_to_host(uint64_t n) {
    return leToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int8_t
le_to_host(int8_t n) {
    return leToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int16_t
le_to_host(int16_t n) {
    return leToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int32_t
le_to_host(int32_t n) {
    return leToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int64_t
le_to_host(int64_t n) {
    return leToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(unsigned h, uint8_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(unsigned h, uint16_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(unsigned h, uint32_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(rose_addr_t h, uint64_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(int h, int8_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(int h, int16_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(int h, int32_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(int64_t h, int64_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint8_t
be_to_host(uint8_t n) {
    return beToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint16_t
be_to_host(uint16_t n) {
    return beToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint32_t
be_to_host(uint32_t n) {
    return beToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint64_t
be_to_host(uint64_t n) {
    return beToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int8_t
be_to_host(int8_t n) {
    return beToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int16_t
be_to_host(int16_t n) {
    return beToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int32_t
be_to_host(int32_t n) {
    return beToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
int64_t
be_to_host(int64_t n) {
    return beToHost(n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(unsigned h, uint8_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(unsigned h, uint16_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(unsigned h, uint32_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(rose_addr_t h, uint64_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(int h, int8_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(int h, int16_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(int h, int32_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(int64_t h, int64_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(RelativeVirtualAddress h, uint32_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_le(RelativeVirtualAddress h, uint64_t *n) {
    hostToLe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(RelativeVirtualAddress h, uint32_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_be(RelativeVirtualAddress h, uint64_t *n) {
    hostToBe(h, n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint8_t
disk_to_host(Endianness sex, uint8_t n) {
    return diskToHost(sex, n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint16_t
disk_to_host(Endianness sex, uint16_t n) {
    return diskToHost(sex, n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint32_t
disk_to_host(Endianness sex, uint32_t n) {
    return diskToHost(sex, n);
}

// [Robb Matzke 2023-06-09]: deprecated
uint64_t
disk_to_host(Endianness sex, uint64_t n) {
    return diskToHost(sex, n);
}

// [Robb Matzke 2023-06-09]: deprecated
int8_t
disk_to_host(Endianness sex, int8_t n) {
    return diskToHost(sex, n);
}

// [Robb Matzke 2023-06-09]: deprecated
int16_t
disk_to_host(Endianness sex, int16_t n) {
    return diskToHost(sex, n);
}

// [Robb Matzke 2023-06-09]: deprecated
int32_t
disk_to_host(Endianness sex, int32_t n) {
    return diskToHost(sex, n);
}

// [Robb Matzke 2023-06-09]: deprecated
int64_t
disk_to_host(Endianness sex, int64_t n) {
    return diskToHost(sex, n);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_disk(Endianness sex, unsigned h, uint8_t *np) {
    hostToDisk(sex, h, np);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_disk(Endianness sex, unsigned h, uint16_t *np) {
    hostToDisk(sex, h, np);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_disk(Endianness sex, unsigned h, uint32_t *np) {
    hostToDisk(sex, h, np);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_disk(Endianness sex, rose_addr_t h, uint64_t *np) {
    hostToDisk(sex, h, np);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_disk(Endianness sex, RelativeVirtualAddress h, uint64_t *np) {
    hostToDisk(sex, h.rva(), np);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_disk(Endianness sex, int h, int8_t *np) {
    hostToDisk(sex, h, np);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_disk(Endianness sex, int h, int16_t *np) {
    hostToDisk(sex, h, np);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_disk(Endianness sex, int h, int32_t *np) {
    hostToDisk(sex, h, np);
}

// [Robb Matzke 2023-06-09]: deprecated
void
host_to_disk(Endianness sex, int64_t h, int64_t *np) {
    hostToDisk(sex, h, np);
}

} // namespace
} // namespace
} // namespace

#endif
