//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfUnspecifiedParameters            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfUnspecifiedParameters_IMPL
#include <SgAsmDwarfUnspecifiedParameters.h>

SgAsmDwarfUnspecifiedParameters::~SgAsmDwarfUnspecifiedParameters() {
    destructorHelper();
}

SgAsmDwarfUnspecifiedParameters::SgAsmDwarfUnspecifiedParameters() {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfUnspecifiedParameters::SgAsmDwarfUnspecifiedParameters(int const& nesting_level,
                                                                 uint64_t const& offset,
                                                                 uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfUnspecifiedParameters::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
