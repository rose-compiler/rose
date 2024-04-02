//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfCondition            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfCondition_IMPL
#include <SgAsmDwarfCondition.h>

SgAsmDwarfCondition::~SgAsmDwarfCondition() {
    destructorHelper();
}

SgAsmDwarfCondition::SgAsmDwarfCondition() {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfCondition::SgAsmDwarfCondition(int const& nesting_level,
                                         uint64_t const& offset,
                                         uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfCondition::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
