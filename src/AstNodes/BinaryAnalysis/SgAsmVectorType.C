#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

using namespace Rose;

SgAsmVectorType::SgAsmVectorType(size_t nElmts, SgAsmType *elmtType) {
    initializeProperties();
    p_nElmts = nElmts;
    p_elmtType = elmtType;
    check();
}

// see super class
void
SgAsmVectorType::check() const {
    SgAsmType::check();
    ASSERT_always_require(p_nElmts>0);
    ASSERT_always_not_null(p_elmtType);
    p_elmtType->check();
}

// see super class
std::string
SgAsmVectorType::toString() const {
    std::ostringstream retval;
    retval <<"vector(nElmts=" <<p_nElmts <<", " <<p_elmtType->toString() <<")";
    return retval.str();
}

// see super class
size_t
SgAsmVectorType::get_nBits() const {
    // each element is aligned on a byte boundary
    ASSERT_require(p_nElmts>0);
    return 8 * (p_nElmts-1) * p_elmtType->get_nBytes() + p_elmtType->get_nBits();
}

size_t
SgAsmVectorType::get_nElmts() const {
    return p_nElmts;
}

SgAsmType *
SgAsmVectorType::get_elmtType() const {
    return p_elmtType;
}

#endif
