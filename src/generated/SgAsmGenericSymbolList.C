//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSymbolList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmGenericSymbolPtrList const&
SgAsmGenericSymbolList::get_symbols() const {
    return p_symbols;
}

SgAsmGenericSymbolPtrList&
SgAsmGenericSymbolList::get_symbols() {
    return p_symbols;
}

void
SgAsmGenericSymbolList::set_symbols(SgAsmGenericSymbolPtrList const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

SgAsmGenericSymbolList::~SgAsmGenericSymbolList() {
    destructorHelper();
}

SgAsmGenericSymbolList::SgAsmGenericSymbolList() {}

void
SgAsmGenericSymbolList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
