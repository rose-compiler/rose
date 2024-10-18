#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using namespace Rose;

SgAsmVoidType*
SgAsmVoidType::instance() {
    return new SgAsmVoidType;
}

std::string
SgAsmVoidType::toString() const {
    return "void";
}

size_t
SgAsmVoidType::get_nBits() const {
    return 0;
}

#endif
