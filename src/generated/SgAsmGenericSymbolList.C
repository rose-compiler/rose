#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSymbolList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSymbolPtrList const&
SgAsmGenericSymbolList::get_symbols() const {
    return p_symbols;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSymbolPtrList&
SgAsmGenericSymbolList::get_symbols() {
    return p_symbols;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSymbolList::set_symbols(SgAsmGenericSymbolPtrList const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSymbolList::~SgAsmGenericSymbolList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSymbolList::SgAsmGenericSymbolList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSymbolList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
