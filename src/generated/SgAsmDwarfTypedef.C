//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfTypedef                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmDwarfTypedef::~SgAsmDwarfTypedef() {
    destructorHelper();
}

SgAsmDwarfTypedef::SgAsmDwarfTypedef() {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfTypedef::SgAsmDwarfTypedef(int const& nesting_level,
                                     uint64_t const& offset,
                                     uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfTypedef::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
