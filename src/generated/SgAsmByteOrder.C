#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmByteOrder            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
ByteOrder::Endianness const&
SgAsmByteOrder::byteOrder() const {
    return byteOrder_;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmByteOrder::byteOrder(ByteOrder::Endianness const& x) {
    this->byteOrder_ = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmByteOrder::~SgAsmByteOrder() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmByteOrder::SgAsmByteOrder() {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=byteOrder        class=SgAsmByteOrder
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmByteOrder::SgAsmByteOrder(ByteOrder::Endianness const& byteOrder)
    : byteOrder_(byteOrder) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmByteOrder::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
