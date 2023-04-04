//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfSharedType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmDwarfSharedType::~SgAsmDwarfSharedType() {
    destructorHelper();
}

SgAsmDwarfSharedType::SgAsmDwarfSharedType() {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfSharedType::SgAsmDwarfSharedType(int const& nesting_level,
                                           uint64_t const& offset,
                                           uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfSharedType::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
