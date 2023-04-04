//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbolList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmElfSymbolPtrList const&
SgAsmElfSymbolList::get_symbols() const {
    return p_symbols;
}

SgAsmElfSymbolPtrList&
SgAsmElfSymbolList::get_symbols() {
    return p_symbols;
}

void
SgAsmElfSymbolList::set_symbols(SgAsmElfSymbolPtrList const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

SgAsmElfSymbolList::~SgAsmElfSymbolList() {
    destructorHelper();
}

SgAsmElfSymbolList::SgAsmElfSymbolList() {}

void
SgAsmElfSymbolList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
