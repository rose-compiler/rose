#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCoffSymbolList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolPtrList const&
SgAsmCoffSymbolList::get_symbols() const {
    return p_symbols;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolPtrList&
SgAsmCoffSymbolList::get_symbols() {
    return p_symbols;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbolList::set_symbols(SgAsmCoffSymbolPtrList const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolList::~SgAsmCoffSymbolList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmCoffSymbolList::SgAsmCoffSymbolList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmCoffSymbolList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
