#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbolList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolPtrList const&
SgAsmElfSymbolList::get_symbols() const {
    return p_symbols;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolPtrList&
SgAsmElfSymbolList::get_symbols() {
    return p_symbols;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbolList::set_symbols(SgAsmElfSymbolPtrList const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolList::~SgAsmElfSymbolList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolList::SgAsmElfSymbolList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbolList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
