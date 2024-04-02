//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilUint32Heap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilUint32Heap_IMPL
#include <SgAsmCilUint32Heap.h>

std::vector<uint32_t> const&
SgAsmCilUint32Heap::get_Stream() const {
    return p_Stream;
}

std::vector<uint32_t>&
SgAsmCilUint32Heap::get_Stream() {
    return p_Stream;
}

SgAsmCilUint32Heap::~SgAsmCilUint32Heap() {
    destructorHelper();
}

SgAsmCilUint32Heap::SgAsmCilUint32Heap() {}

// The association between constructor arguments and their classes:
//    property=Offset           class=SgAsmCilDataStream
//    property=Size             class=SgAsmCilDataStream
//    property=Name             class=SgAsmCilDataStream
//    property=NamePadding      class=SgAsmCilDataStream
SgAsmCilUint32Heap::SgAsmCilUint32Heap(uint32_t const& Offset,
                                       uint32_t const& Size,
                                       std::string const& Name,
                                       uint32_t const& NamePadding)
    : SgAsmCilDataStream(Offset, Size, Name, NamePadding) {}

void
SgAsmCilUint32Heap::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
