//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfNamespace            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmDwarfConstructList* const&
SgAsmDwarfNamespace::get_body() const {
    return p_body;
}

void
SgAsmDwarfNamespace::set_body(SgAsmDwarfConstructList* const& x) {
    this->p_body = x;
    set_isModified(true);
}

SgAsmDwarfNamespace::~SgAsmDwarfNamespace() {
    destructorHelper();
}

SgAsmDwarfNamespace::SgAsmDwarfNamespace()
    : p_body(NULL) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfNamespace::SgAsmDwarfNamespace(int const& nesting_level,
                                         uint64_t const& offset,
                                         uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset) {}

void
SgAsmDwarfNamespace::initializeProperties() {
    p_body = NULL;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
