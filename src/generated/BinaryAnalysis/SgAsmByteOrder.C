//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmByteOrder            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

ByteOrder::Endianness const&
SgAsmByteOrder::byteOrder() const {
    return byteOrder_;
}

void
SgAsmByteOrder::byteOrder(ByteOrder::Endianness const& x) {
    this->byteOrder_ = x;
    set_isModified(true);
}

SgAsmByteOrder::~SgAsmByteOrder() {
    destructorHelper();
}

SgAsmByteOrder::SgAsmByteOrder() {}

// The association between constructor arguments and their classes:
//    property=byteOrder        class=SgAsmByteOrder
SgAsmByteOrder::SgAsmByteOrder(ByteOrder::Endianness const& byteOrder)
    : byteOrder_(byteOrder) {}

void
SgAsmByteOrder::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
