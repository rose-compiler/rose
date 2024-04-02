//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfUpcSharedType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfUpcSharedType_IMPL
#include <SgAsmDwarfUpcSharedType.h>

SgAsmDwarfUpcSharedType::~SgAsmDwarfUpcSharedType() {
    destructorHelper();
}

SgAsmDwarfUpcSharedType::SgAsmDwarfUpcSharedType() {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfUpcSharedType::SgAsmDwarfUpcSharedType(int const& nesting_level,
                                                 uint64_t const& offset,
                                                 uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfUpcSharedType::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
