#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/AST/Traversal.h>

using namespace Rose;

SgAsmPointerType::SgAsmPointerType(Rose::BinaryAnalysis::ByteOrder::Endianness sex, size_t nBits, SgAsmType *subtype)
    : SgAsmScalarType(sex, nBits) {
    ASSERT_not_null(subtype);
    initializeProperties();
    set_subtype(subtype);
    check();
}

SgAsmPointerType*
SgAsmPointerType::instance(Rose::BinaryAnalysis::ByteOrder::Endianness sex, size_t nBits, SgAsmType *subtype) {
    if (Rose::BinaryAnalysis::ByteOrder::ORDER_UNSPECIFIED == sex) {
        Rose::AST::Traversal::forwardPre<SgAsmScalarType>(subtype, [&sex](SgAsmScalarType *t) {
            if (Rose::BinaryAnalysis::ByteOrder::ORDER_UNSPECIFIED == sex)
                sex = t->get_minorOrder();
        });
    }

    return new SgAsmPointerType(sex, nBits, subtype);
}

void
SgAsmPointerType::check() const {
    SgAsmScalarType::check();
    ASSERT_always_not_null(get_subtype());
}

std::string
SgAsmPointerType::toString() const {
    return get_subtype()->toString() + "*";
}

#endif
