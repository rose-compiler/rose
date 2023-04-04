#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSymbol            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSymbol::SymbolDefState const&
SgAsmGenericSymbol::get_def_state() const {
    return p_def_state;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSymbol::set_def_state(SgAsmGenericSymbol::SymbolDefState const& x) {
    this->p_def_state = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSymbol::SymbolBinding const&
SgAsmGenericSymbol::get_binding() const {
    return p_binding;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSymbol::set_binding(SgAsmGenericSymbol::SymbolBinding const& x) {
    this->p_binding = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSymbol::SymbolType const&
SgAsmGenericSymbol::get_type() const {
    return p_type;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSymbol::set_type(SgAsmGenericSymbol::SymbolType const& x) {
    this->p_type = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmGenericSymbol::get_value() const {
    return p_value;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSymbol::set_value(rose_addr_t const& x) {
    this->p_value = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmGenericSymbol::get_size() const {
    return p_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSymbol::set_size(rose_addr_t const& x) {
    this->p_size = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSection* const&
SgAsmGenericSymbol::get_bound() const {
    return p_bound;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSymbol::set_bound(SgAsmGenericSection* const& x) {
    this->p_bound = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSymbol::~SgAsmGenericSymbol() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSymbol::SgAsmGenericSymbol()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_def_state(SgAsmGenericSymbol::SYM_UNDEFINED)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_binding(SgAsmGenericSymbol::SYM_NO_BINDING)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_type(SgAsmGenericSymbol::SYM_NO_TYPE)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_value(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_size(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_bound(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_name(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSymbol::initializeProperties() {
    p_def_state = SgAsmGenericSymbol::SYM_UNDEFINED;
    p_binding = SgAsmGenericSymbol::SYM_NO_BINDING;
    p_type = SgAsmGenericSymbol::SYM_NO_TYPE;
    p_value = 0;
    p_size = 0;
    p_bound = nullptr;
    p_name = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
