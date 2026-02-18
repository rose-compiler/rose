//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfVariable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfVariable_IMPL
#include <SgAsmDwarfVariable.h>

std::string const&
SgAsmDwarfVariable::get_linkage_name() const {
    return p_linkage_name;
}

void
SgAsmDwarfVariable::set_linkage_name(std::string const& x) {
    this->p_linkage_name = x;
    set_isModified(true);
}

bool const&
SgAsmDwarfVariable::get_is_const_expr() const {
    return p_is_const_expr;
}

void
SgAsmDwarfVariable::set_is_const_expr(bool const& x) {
    this->p_is_const_expr = x;
    set_isModified(true);
}

SgAsmDwarfVariable::~SgAsmDwarfVariable() {
    destructorHelper();
}

SgAsmDwarfVariable::SgAsmDwarfVariable()
    : p_is_const_expr(false) {}

// The association between constructor arguments and their classes:
//    property=nesting_level    class=SgAsmDwarfConstruct
//    property=offset           class=SgAsmDwarfConstruct
//    property=overall_offset   class=SgAsmDwarfConstruct
SgAsmDwarfVariable::SgAsmDwarfVariable(int const& nesting_level,
                                       uint64_t const& offset,
                                       uint64_t const& overall_offset)
    : SgAsmDwarfConstruct(nesting_level, offset, overall_offset)
    , p_is_const_expr(false) {}

void
SgAsmDwarfVariable::initializeProperties() {
    p_is_const_expr = false;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
