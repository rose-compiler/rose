//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfSubprogram            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfSubprogram_IMPL
#include <SgAsmDwarfSubprogram.h>

SgAsmDwarfConstructList* const&
SgAsmDwarfSubprogram::get_body() const {
    return p_body;
}

void
SgAsmDwarfSubprogram::set_body(SgAsmDwarfConstructList* const& x) {
    changeChildPointer(this->p_body, const_cast<SgAsmDwarfConstructList*&>(x));
    set_isModified(true);
}

std::string const&
SgAsmDwarfSubprogram::get_linkage_name() const {
    return p_linkage_name;
}

void
SgAsmDwarfSubprogram::set_linkage_name(std::string const& x) {
    this->p_linkage_name = x;
    set_isModified(true);
}

bool const&
SgAsmDwarfSubprogram::get_is_main_subprogram() const {
    return p_is_main_subprogram;
}

void
SgAsmDwarfSubprogram::set_is_main_subprogram(bool const& x) {
    this->p_is_main_subprogram = x;
    set_isModified(true);
}

bool const&
SgAsmDwarfSubprogram::get_is_const_expr() const {
    return p_is_const_expr;
}

void
SgAsmDwarfSubprogram::set_is_const_expr(bool const& x) {
    this->p_is_const_expr = x;
    set_isModified(true);
}

SgAsmDwarfSubprogram::~SgAsmDwarfSubprogram() {
    destructorHelper();
}

SgAsmDwarfSubprogram::SgAsmDwarfSubprogram()
    : p_body(NULL)
    , p_is_main_subprogram(false)
    , p_is_const_expr(false) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfSubprogram::SgAsmDwarfSubprogram(int const& nesting_level,
                                           uint64_t const& offset,
                                           uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_body(NULL)
    , p_is_main_subprogram(false)
    , p_is_const_expr(false) {}

void
SgAsmDwarfSubprogram::initializeProperties() {
    p_body = NULL;
    p_is_main_subprogram = false;
    p_is_const_expr = false;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
