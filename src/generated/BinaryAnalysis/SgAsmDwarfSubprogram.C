//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfSubprogram            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfSubprogram_IMPL
#include <sage3basic.h>

SgAsmDwarfConstructList* const&
SgAsmDwarfSubprogram::get_body() const {
    return p_body;
}

void
SgAsmDwarfSubprogram::set_body(SgAsmDwarfConstructList* const& x) {
    changeChildPointer(this->p_body, const_cast<SgAsmDwarfConstructList*&>(x));
    set_isModified(true);
}

SgAsmDwarfSubprogram::~SgAsmDwarfSubprogram() {
    destructorHelper();
}

SgAsmDwarfSubprogram::SgAsmDwarfSubprogram()
    : p_body(NULL) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfSubprogram::SgAsmDwarfSubprogram(int const& nesting_level,
                                           uint64_t const& offset,
                                           uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_body(NULL) {}

void
SgAsmDwarfSubprogram::initializeProperties() {
    p_body = NULL;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
