#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfNamespace            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfConstructList* const&
SgAsmDwarfNamespace::get_body() const {
    return p_body;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfNamespace::set_body(SgAsmDwarfConstructList* const& x) {
    this->p_body = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfNamespace::~SgAsmDwarfNamespace() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfNamespace::SgAsmDwarfNamespace()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_body(NULL) {}

#line 349 "src/Rosebud/RosettaGenerator.C"
// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
#line 357 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfNamespace::SgAsmDwarfNamespace(int const& nesting_level,
                                         uint64_t const& offset,
                                         uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_body(NULL) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfNamespace::initializeProperties() {
    p_body = NULL;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
