//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfUnspecifiedType            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmDwarfUnspecifiedType::~SgAsmDwarfUnspecifiedType() {
    destructorHelper();
}

SgAsmDwarfUnspecifiedType::SgAsmDwarfUnspecifiedType() {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfUnspecifiedType::SgAsmDwarfUnspecifiedType(int const& nesting_level,
                                                     uint64_t const& offset,
                                                     uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfUnspecifiedType::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
