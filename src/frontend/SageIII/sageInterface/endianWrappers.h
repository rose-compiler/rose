#ifndef ROSE_ENDIANWRAPPERS_H
#define ROSE_ENDIANWRAPPERS_H

#include <stdint.h>

// We don't just implement these ourselves because some processors have special
// instructions for endian swapping

#ifdef HAVE_BYTESWAP_H
#include <byteswap.h>

static inline uint16_t roseBswap16(uint16_t x) {
  return bswap_16(x);
}

static inline uint32_t roseBswap32(uint32_t x) {
  return bswap_32(x);
}

static inline uint64_t roseBswap64(uint64_t x) {
  return bswap_64(x);
}
#else

#ifdef HAVE_MACHINE_ENDIAN_H
#include <machine/endian.h>
static inline uint16_t roseBswap16(uint16_t x) {
  return _OSSwapInt16(x);
}

static inline uint32_t roseBswap32(uint32_t x) {
  return _OSSwapInt32(x);
}

static inline uint64_t roseBswap64(uint64_t x) {
  return _OSSwapInt64(x);
}

#else
#ifdef _MSC_VER
// DQ (11/28/2009): Since it seems we can compile everything, let this be something we 
// fix if we have probelems linking. Remove the warning for now to focus on a clean compile.
// #pragma message ("WARNING EndianWrappers.h: MS: Could not find endian swapping code" )
#include <sys/types.h>

#else
#error "Could not find endian swapping code"
#endif
#endif
#endif

#ifndef _MSC_VER
#ifdef WORDS_BIGENDIAN
static inline uint16_t getLittleEndian16(uint16_t x) {return roseBswap16(x);}
static inline uint32_t getLittleEndian32(uint32_t x) {return roseBswap32(x);}
static inline uint64_t getLittleEndian64(uint64_t x) {return roseBswap64(x);}
static inline uint16_t getBigEndian16(uint16_t x) {return x;}
static inline uint32_t getBigEndian32(uint32_t x) {return x;}
static inline uint64_t getBigEndian64(uint64_t x) {return x;}
#else
static inline uint16_t getLittleEndian16(uint16_t x) {return x;}
static inline uint32_t getLittleEndian32(uint32_t x) {return x;}
static inline uint64_t getLittleEndian64(uint64_t x) {return x;}
static inline uint16_t getBigEndian16(uint16_t x) {return roseBswap16(x);}
static inline uint32_t getBigEndian32(uint32_t x) {return roseBswap32(x);}
static inline uint64_t getBigEndian64(uint64_t x) {return roseBswap64(x);}
#endif

static inline uint16_t getSwitchedEndian16(bool isBigEndian, uint16_t x) {
  return isBigEndian ? getBigEndian16(x) : getLittleEndian16(x);
}

static inline uint32_t getSwitchedEndian32(bool isBigEndian, uint32_t x) {
  return isBigEndian ? getBigEndian32(x) : getLittleEndian32(x);
}

static inline uint64_t getSwitchedEndian64(bool isBigEndian, uint64_t x) {
  return isBigEndian ? getBigEndian64(x) : getLittleEndian64(x);
}

#endif
#endif // ROSE_ENDIANWRAPPERS_H
