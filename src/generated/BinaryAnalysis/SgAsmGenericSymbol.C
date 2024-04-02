//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSymbol            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmGenericSymbol_IMPL
#include <SgAsmGenericSymbol.h>

SgAsmGenericSymbol::SymbolDefState const&
SgAsmGenericSymbol::get_definitionState() const {
    return p_definitionState;
}

void
SgAsmGenericSymbol::set_definitionState(SgAsmGenericSymbol::SymbolDefState const& x) {
    this->p_definitionState = x;
    set_isModified(true);
}

SgAsmGenericSymbol::SymbolBinding const&
SgAsmGenericSymbol::get_binding() const {
    return p_binding;
}

void
SgAsmGenericSymbol::set_binding(SgAsmGenericSymbol::SymbolBinding const& x) {
    this->p_binding = x;
    set_isModified(true);
}

SgAsmGenericSymbol::SymbolType const&
SgAsmGenericSymbol::get_type() const {
    return p_type;
}

void
SgAsmGenericSymbol::set_type(SgAsmGenericSymbol::SymbolType const& x) {
    this->p_type = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmGenericSymbol::get_value() const {
    return p_value;
}

void
SgAsmGenericSymbol::set_value(rose_addr_t const& x) {
    this->p_value = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmGenericSymbol::get_size() const {
    return p_size;
}

void
SgAsmGenericSymbol::set_size(rose_addr_t const& x) {
    this->p_size = x;
    set_isModified(true);
}

SgAsmGenericSection* const&
SgAsmGenericSymbol::get_bound() const {
    return p_bound;
}

void
SgAsmGenericSymbol::set_bound(SgAsmGenericSection* const& x) {
    this->p_bound = x;
    set_isModified(true);
}

SgAsmGenericSymbol::~SgAsmGenericSymbol() {
    destructorHelper();
}

SgAsmGenericSymbol::SgAsmGenericSymbol()
    : p_definitionState(SgAsmGenericSymbol::SYM_UNDEFINED)
    , p_binding(SgAsmGenericSymbol::SYM_NO_BINDING)
    , p_type(SgAsmGenericSymbol::SYM_NO_TYPE)
    , p_value(0)
    , p_size(0)
    , p_bound(nullptr)
    , p_name(nullptr) {}

void
SgAsmGenericSymbol::initializeProperties() {
    p_definitionState = SgAsmGenericSymbol::SYM_UNDEFINED;
    p_binding = SgAsmGenericSymbol::SYM_NO_BINDING;
    p_type = SgAsmGenericSymbol::SYM_NO_TYPE;
    p_value = 0;
    p_size = 0;
    p_bound = nullptr;
    p_name = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
