//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfInlinedSubroutine            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfInlinedSubroutine_IMPL
#include <sage3basic.h>

SgAsmDwarfConstructList* const&
SgAsmDwarfInlinedSubroutine::get_body() const {
    return p_body;
}

void
SgAsmDwarfInlinedSubroutine::set_body(SgAsmDwarfConstructList* const& x) {
    changeChildPointer(this->p_body, const_cast<SgAsmDwarfConstructList*&>(x));
    set_isModified(true);
}

SgAsmDwarfInlinedSubroutine::~SgAsmDwarfInlinedSubroutine() {
    destructorHelper();
}

SgAsmDwarfInlinedSubroutine::SgAsmDwarfInlinedSubroutine()
    : p_body(nullptr) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfInlinedSubroutine::SgAsmDwarfInlinedSubroutine(int const& nesting_level,
                                                         uint64_t const& offset,
                                                         uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_body(nullptr) {}

void
SgAsmDwarfInlinedSubroutine::initializeProperties() {
    p_body = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
