#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmByteOrder            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
ByteOrder::Endianness const&
SgAsmByteOrder::byteOrder() const {
    return byteOrder_;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmByteOrder::byteOrder(ByteOrder::Endianness const& x) {
    this->byteOrder_ = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmByteOrder::~SgAsmByteOrder() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmByteOrder::SgAsmByteOrder() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=byteOrder        class=SgAsmByteOrder
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmByteOrder::SgAsmByteOrder(ByteOrder::Endianness const& byteOrder)
    : byteOrder_(byteOrder) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmByteOrder::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
