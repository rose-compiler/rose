#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCoffSymbolTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSection* const&
SgAsmCoffSymbolTable::get_strtab() const {
    return p_strtab;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbolTable::set_strtab(SgAsmGenericSection* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolList* const&
SgAsmCoffSymbolTable::get_symbols() const {
    return p_symbols;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbolTable::set_symbols(SgAsmCoffSymbolList* const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolTable::~SgAsmCoffSymbolTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolTable::SgAsmCoffSymbolTable()
    : p_strtab(nullptr)
    , p_symbols(createAndParent<SgAsmCoffSymbolList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbolTable::initializeProperties() {
    p_strtab = nullptr;
    p_symbols = createAndParent<SgAsmCoffSymbolList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
