#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilUint8Heap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
std::vector<uint8_t> const&
SgAsmCilUint8Heap::get_Stream() const {
    return p_Stream;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
std::vector<uint8_t>&
SgAsmCilUint8Heap::get_Stream() {
    return p_Stream;
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCilUint8Heap::~SgAsmCilUint8Heap() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCilUint8Heap::SgAsmCilUint8Heap() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=Offset           class=SgAsmCilDataStream
//    property=Size             class=SgAsmCilDataStream
//    property=Name             class=SgAsmCilDataStream
//    property=NamePadding      class=SgAsmCilDataStream
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmCilUint8Heap::SgAsmCilUint8Heap(uint32_t const& Offset,
                                     uint32_t const& Size,
                                     std::string const& Name,
                                     uint32_t const& NamePadding)
    : SgAsmCilDataStream(Offset, Size, Name, NamePadding) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilUint8Heap::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
