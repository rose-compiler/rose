//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilErrorHeap            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilErrorHeap_IMPL
#include <SgAsmCilErrorHeap.h>

SgAsmCilErrorHeap::~SgAsmCilErrorHeap() {
    destructorHelper();
}

SgAsmCilErrorHeap::SgAsmCilErrorHeap() {}

// The association between constructor arguments and their classes:
//    property=Offset           class=SgAsmCilDataStream
//    property=Size             class=SgAsmCilDataStream
//    property=Name             class=SgAsmCilDataStream
//    property=NamePadding      class=SgAsmCilDataStream
SgAsmCilErrorHeap::SgAsmCilErrorHeap(uint32_t const& Offset,
                                     uint32_t const& Size,
                                     std::string const& Name,
                                     uint32_t const& NamePadding)
    : SgAsmCilDataStream(Offset, Size, Name, NamePadding) {}

void
SgAsmCilErrorHeap::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
