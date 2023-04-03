#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilUint8Heap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
std::vector<uint8_t> const&
SgAsmCilUint8Heap::get_Stream() const {
    return p_Stream;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
std::vector<uint8_t>&
SgAsmCilUint8Heap::get_Stream() {
    return p_Stream;
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmCilUint8Heap::~SgAsmCilUint8Heap() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmCilUint8Heap::SgAsmCilUint8Heap() {}

#line 305 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=Offset           class=SgAsmCilDataStream
//    property=Size             class=SgAsmCilDataStream
//    property=Name             class=SgAsmCilDataStream
//    property=NamePadding      class=SgAsmCilDataStream
#line 313 "src/Rosebud/RosettaGenerator.C"
SgAsmCilUint8Heap::SgAsmCilUint8Heap(uint32_t const& Offset,
                                     uint32_t const& Size,
                                     std::string const& Name,
                                     uint32_t const& NamePadding)
    : SgAsmCilDataStream(Offset, Size, Name, NamePadding) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCilUint8Heap::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
