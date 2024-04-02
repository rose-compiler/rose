//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbolList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymbolList_IMPL
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
    changeChildPointer(this->p_symbols, const_cast<SgAsmElfSymbolPtrList&>(x));
    set_isModified(true);
}

SgAsmElfSymbolList::~SgAsmElfSymbolList() {
    destructorHelper();
}

SgAsmElfSymbolList::SgAsmElfSymbolList() {}

void
SgAsmElfSymbolList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
