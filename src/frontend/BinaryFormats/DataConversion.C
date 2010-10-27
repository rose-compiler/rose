/* Functions for data conversion. Functions such as byte swapping, etc. */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

SgAsmExecutableFileFormat::ByteOrder
SgAsmExecutableFileFormat::host_order()
{
    static const int i = 1;
    return *(unsigned char*)&i ? ORDER_LSB : ORDER_MSB;
}

/* Swap (reverse) bytes taking care of sign extension */
uint8_t
SgAsmExecutableFileFormat::swap_bytes(uint8_t n)
{
    return n;
}

uint16_t
SgAsmExecutableFileFormat::swap_bytes(uint16_t n)
{
    return ((n>>8) & 0xff) | ((n<<8) & 0xff00);
}

uint32_t
SgAsmExecutableFileFormat::swap_bytes(uint32_t n)
{
    return ((n>>24) & 0xff) | ((n>>8) & 0xff00) | ((n<<8) & 0xff0000) | ((n<<24) & 0xff000000u);
}

uint64_t
SgAsmExecutableFileFormat::swap_bytes(uint64_t n)
{
    return (((n>>56) & (0xffull<<0 )) | ((n>>40) & (0xffull<<8 )) | ((n>>24) & (0xffull<<16)) | ((n>>8 ) & (0xffull<<24)) |
            ((n<<8 ) & (0xffull<<32)) | ((n<<24) & (0xffull<<40)) | ((n<<40) & (0xffull<<48)) | ((n<<56) & (0xffull<<56)));
}

int8_t
SgAsmExecutableFileFormat::swap_bytes(int8_t n)
{
    return swap_bytes((uint8_t)n);
}

int16_t
SgAsmExecutableFileFormat::swap_bytes(int16_t n)
{
    return swap_bytes((uint16_t)n);
}

int32_t
SgAsmExecutableFileFormat::swap_bytes(int32_t n)
{
    return swap_bytes((uint32_t)n);
}

int64_t
SgAsmExecutableFileFormat::swap_bytes(int64_t n)
{
    return swap_bytes((uint64_t)n);
}

/* Little-endian byte order conversions */
uint8_t
SgAsmExecutableFileFormat::le_to_host(uint8_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

uint16_t
SgAsmExecutableFileFormat::le_to_host(uint16_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

uint32_t
SgAsmExecutableFileFormat::le_to_host(uint32_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

uint64_t
SgAsmExecutableFileFormat::le_to_host(uint64_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

int8_t
SgAsmExecutableFileFormat::le_to_host(int8_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

int16_t
SgAsmExecutableFileFormat::le_to_host(int16_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

int32_t
SgAsmExecutableFileFormat::le_to_host(int32_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

int64_t
SgAsmExecutableFileFormat::le_to_host(int64_t n)
{
    return ORDER_LSB==host_order() ? n : swap_bytes(n);
}

void
SgAsmExecutableFileFormat::host_to_le(unsigned h, uint8_t *n)
{
    assert(0==(h & ~0xff));
    uint8_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(unsigned h, uint16_t *n)
{
    assert(0==(h & ~0xffff));
    uint16_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(unsigned h, uint32_t *n)
{
    assert(0==(h & ~0xfffffffful));
    uint32_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(rose_addr_t h, uint64_t *n)
{
    assert(0==(h & ~0xffffffffffffffffull));
    uint64_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(rose_rva_t h, uint32_t *n)
{
    host_to_le(h.get_rva(), n);
}

void
SgAsmExecutableFileFormat::host_to_le(rose_rva_t h, uint64_t *n)
{
    host_to_le(h.get_rva(), n);
}

void
SgAsmExecutableFileFormat::host_to_le(int h, int8_t *n)
{
    assert((unsigned)h<=0x8f || ((unsigned)h|0xff)==(unsigned)-1);
    int8_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(int h, int16_t *n)
{
    assert((unsigned)h<=0x8fff || ((unsigned)h|0xffff)==(unsigned)-1);
    int16_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(int h, int32_t *n)
{
    assert((unsigned)h<=0x8fffffffu || ((unsigned)h|0xffffffffu)==(unsigned)-1);
    int32_t hh = h;
    *n = ORDER_LSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_le(int64_t h, int64_t *n)
{
    *n = ORDER_LSB==host_order() ? h : swap_bytes(h);
}

/* Big-endian byte order conversions */
uint8_t
SgAsmExecutableFileFormat::be_to_host(uint8_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

uint16_t
SgAsmExecutableFileFormat::be_to_host(uint16_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

uint32_t
SgAsmExecutableFileFormat::be_to_host(uint32_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

uint64_t
SgAsmExecutableFileFormat::be_to_host(uint64_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

int8_t
SgAsmExecutableFileFormat::be_to_host(int8_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

int16_t
SgAsmExecutableFileFormat::be_to_host(int16_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

int32_t
SgAsmExecutableFileFormat::be_to_host(int32_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

int64_t
SgAsmExecutableFileFormat::be_to_host(int64_t n)
{
    return ORDER_MSB==host_order() ? n : swap_bytes(n);
}

void
SgAsmExecutableFileFormat::host_to_be(unsigned h, uint8_t *n)
{
    assert(0==(h & ~0xff));
    uint8_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(unsigned h, uint16_t *n)
{
    assert(0==(h & ~0xffff));
    uint16_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(unsigned h, uint32_t *n)
{
    assert(0==(h & ~0xfffffffful));
    uint32_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(rose_addr_t h, uint64_t *n)
{
    assert(0==(h & ~0xffffffffffffffffull));
    uint64_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(rose_rva_t h, uint32_t *n)
{
    host_to_be(h.get_rva(), n);
}

void
SgAsmExecutableFileFormat::host_to_be(rose_rva_t h, uint64_t *n)
{
    host_to_be(h.get_rva(), n);
}

void
SgAsmExecutableFileFormat::host_to_be(int h, int8_t *n)
{
    assert((unsigned)h<0x8f || ((unsigned)h|0xff)==(unsigned)-1);
    int8_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(int h, int16_t *n)
{
    assert((unsigned)h<0x8fff || ((unsigned)h|0xffff)==(unsigned)-1);
    int16_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(int h, int32_t *n)
{
    assert((unsigned)h<0x8ffffffful || ((unsigned)h|0xfffffffful)==(unsigned)-1);
    int32_t hh = h;
    *n = ORDER_MSB==host_order() ? hh : swap_bytes(hh);
}

void
SgAsmExecutableFileFormat::host_to_be(int64_t h, int64_t *n)
{
    *n = ORDER_MSB==host_order() ? h : swap_bytes(h);
}

/* Caller-specified byte order conversions */
uint8_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, uint8_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

uint16_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, uint16_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

uint32_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, uint32_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

uint64_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, uint64_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

int8_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, int8_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

int16_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, int16_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

int32_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, int32_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

int64_t
SgAsmExecutableFileFormat::disk_to_host(SgAsmExecutableFileFormat::ByteOrder sex, int64_t n)
{
    return ORDER_LSB==sex ? le_to_host(n) : be_to_host(n);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, unsigned h, uint8_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, unsigned h, uint16_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, unsigned h, uint32_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, rose_addr_t h, uint64_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, rose_rva_t h, uint64_t *np)
{
    host_to_disk(sex, h.get_rva(), np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, int h, int8_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, int h, int16_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, int h, int32_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}

void
SgAsmExecutableFileFormat::host_to_disk(SgAsmExecutableFileFormat::ByteOrder sex, int64_t h, int64_t *np)
{
    ORDER_LSB==sex ? host_to_le(h, np) : host_to_be(h, np);
}
