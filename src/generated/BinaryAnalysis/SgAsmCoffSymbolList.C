//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCoffSymbolList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCoffSymbolList_IMPL
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
    changeChildPointer(this->p_symbols, const_cast<SgAsmCoffSymbolPtrList&>(x));
    set_isModified(true);
}

SgAsmCoffSymbolList::~SgAsmCoffSymbolList() {
    destructorHelper();
}

SgAsmCoffSymbolList::SgAsmCoffSymbolList() {}

void
SgAsmCoffSymbolList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
