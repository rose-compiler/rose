//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCoffSymbolList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmCoffSymbolPtrList const&
SgAsmCoffSymbolList::get_symbols() const {
    return p_symbols;
}

SgAsmCoffSymbolPtrList&
SgAsmCoffSymbolList::get_symbols() {
    return p_symbols;
}

void
SgAsmCoffSymbolList::set_symbols(SgAsmCoffSymbolPtrList const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

SgAsmCoffSymbolList::~SgAsmCoffSymbolList() {
    destructorHelper();
}

SgAsmCoffSymbolList::SgAsmCoffSymbolList() {}

void
SgAsmCoffSymbolList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
