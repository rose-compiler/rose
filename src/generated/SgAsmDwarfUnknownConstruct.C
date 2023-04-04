#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfUnknownConstruct            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfUnknownConstruct::~SgAsmDwarfUnknownConstruct() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfUnknownConstruct::SgAsmDwarfUnknownConstruct() {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfUnknownConstruct::SgAsmDwarfUnknownConstruct(int const& nesting_level,
                                                       uint64_t const& offset,
                                                       uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfUnknownConstruct::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
