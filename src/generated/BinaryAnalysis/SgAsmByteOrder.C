//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmByteOrder            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmByteOrder_IMPL
#include <SgAsmByteOrder.h>

Rose::BinaryAnalysis::ByteOrder::Endianness const&
SgAsmByteOrder::byteOrder() const {
    return byteOrder_;
}

void
SgAsmByteOrder::byteOrder(Rose::BinaryAnalysis::ByteOrder::Endianness const& x) {
    this->byteOrder_ = x;
    set_isModified(true);
}

SgAsmByteOrder::~SgAsmByteOrder() {
    destructorHelper();
}

SgAsmByteOrder::SgAsmByteOrder() {}

// The association between constructor arguments and their classes:
//    property=byteOrder        class=SgAsmByteOrder
SgAsmByteOrder::SgAsmByteOrder(Rose::BinaryAnalysis::ByteOrder::Endianness const& byteOrder)
    : byteOrder_(byteOrder) {}

void
SgAsmByteOrder::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
