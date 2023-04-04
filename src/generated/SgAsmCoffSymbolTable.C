#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCoffSymbolTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSection* const&
SgAsmCoffSymbolTable::get_strtab() const {
    return p_strtab;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbolTable::set_strtab(SgAsmGenericSection* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolList* const&
SgAsmCoffSymbolTable::get_symbols() const {
    return p_symbols;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbolTable::set_symbols(SgAsmCoffSymbolList* const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolTable::~SgAsmCoffSymbolTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolTable::SgAsmCoffSymbolTable()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_strtab(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_symbols(createAndParent<SgAsmCoffSymbolList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbolTable::initializeProperties() {
    p_strtab = nullptr;
    p_symbols = createAndParent<SgAsmCoffSymbolList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
