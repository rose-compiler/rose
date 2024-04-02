//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilUint8Heap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilUint8Heap_IMPL
#include <sage3basic.h>

std::vector<uint8_t> const&
SgAsmCilUint8Heap::get_Stream() const {
    return p_Stream;
}

std::vector<uint8_t>&
SgAsmCilUint8Heap::get_Stream() {
    return p_Stream;
}

SgAsmCilUint8Heap::~SgAsmCilUint8Heap() {
    destructorHelper();
}

SgAsmCilUint8Heap::SgAsmCilUint8Heap() {}

// The association between constructor arguments and their classes:
//    property=Offset           class=SgAsmCilDataStream
//    property=Size             class=SgAsmCilDataStream
//    property=Name             class=SgAsmCilDataStream
//    property=NamePadding      class=SgAsmCilDataStream
SgAsmCilUint8Heap::SgAsmCilUint8Heap(uint32_t const& Offset,
                                     uint32_t const& Size,
                                     std::string const& Name,
                                     uint32_t const& NamePadding)
    : SgAsmCilDataStream(Offset, Size, Name, NamePadding) {}

void
SgAsmCilUint8Heap::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
