//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfNamelist            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfNamelist_IMPL
#include <sage3basic.h>

SgAsmDwarfNamelist::~SgAsmDwarfNamelist() {
    destructorHelper();
}

SgAsmDwarfNamelist::SgAsmDwarfNamelist() {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfNamelist::SgAsmDwarfNamelist(int const& nesting_level,
                                       uint64_t const& offset,
                                       uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfNamelist::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
