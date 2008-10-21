#ifndef ROSE_SEMANTICSMODULE_H
#define ROSE_SEMANTICSMODULE_H

#include "rose.h"
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

static inline int numBytesInAsmType(SgAsmType* ty) {
  switch (ty->variantT()) {
    case V_SgAsmTypeByte: return 1;
    case V_SgAsmTypeWord: return 2;
    case V_SgAsmTypeDoubleWord: return 4;
    case V_SgAsmTypeQuadWord: return 8;
    default: {std::cerr << "Unhandled type " << ty->class_name() << " in numBytesInAsmType" << std::endl; abort();}
  }
}

static inline uint64_t shl1(unsigned int amount) { // 2**amount, safe for when amount >= 64
  if (amount >= 64) return 0;
  return 1ULL << amount;
}

template <unsigned int Amount, bool AtLeast64>
struct SHL1Helper {};

template <unsigned int Amount>
struct SHL1Helper<Amount, false> {
  static const uint64_t value = (1ULL << Amount);
};

template <unsigned int Amount>
struct SHL1Helper<Amount, true> {
  static const uint64_t value = 0;
};

template <unsigned int Amount>
struct SHL1 {
  static const uint64_t value = SHL1Helper<Amount, (Amount >= 64)>::value;
};


#endif // ROSE_SEMANTICSMODULE_H
