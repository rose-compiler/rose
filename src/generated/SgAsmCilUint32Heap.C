#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilUint32Heap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
std::vector<uint32_t> const&
SgAsmCilUint32Heap::get_Stream() const {
    return p_Stream;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
std::vector<uint32_t>&
SgAsmCilUint32Heap::get_Stream() {
    return p_Stream;
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCilUint32Heap::~SgAsmCilUint32Heap() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCilUint32Heap::SgAsmCilUint32Heap() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=Offset           class=SgAsmCilDataStream
//    property=Size             class=SgAsmCilDataStream
//    property=Name             class=SgAsmCilDataStream
//    property=NamePadding      class=SgAsmCilDataStream
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmCilUint32Heap::SgAsmCilUint32Heap(uint32_t const& Offset,
                                       uint32_t const& Size,
                                       std::string const& Name,
                                       uint32_t const& NamePadding)
    : SgAsmCilDataStream(Offset, Size, Name, NamePadding) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilUint32Heap::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
