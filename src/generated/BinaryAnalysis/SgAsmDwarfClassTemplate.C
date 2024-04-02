//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfClassTemplate            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfClassTemplate_IMPL
#include <SgAsmDwarfClassTemplate.h>

SgAsmDwarfClassTemplate::~SgAsmDwarfClassTemplate() {
    destructorHelper();
}

SgAsmDwarfClassTemplate::SgAsmDwarfClassTemplate() {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfClassTemplate::SgAsmDwarfClassTemplate(int const& nesting_level,
                                                 uint64_t const& offset,
                                                 uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfClassTemplate::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
