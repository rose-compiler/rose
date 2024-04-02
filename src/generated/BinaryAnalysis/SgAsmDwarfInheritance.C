//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfInheritance            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfInheritance_IMPL
#include <sage3basic.h>

SgAsmDwarfInheritance::~SgAsmDwarfInheritance() {
    destructorHelper();
}

SgAsmDwarfInheritance::SgAsmDwarfInheritance() {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfInheritance::SgAsmDwarfInheritance(int const& nesting_level,
                                             uint64_t const& offset,
                                             uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfInheritance::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
