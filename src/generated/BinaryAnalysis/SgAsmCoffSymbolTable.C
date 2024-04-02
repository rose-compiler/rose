//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCoffSymbolTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCoffSymbolTable_IMPL
#include <SgAsmCoffSymbolTable.h>

SgAsmGenericSection* const&
SgAsmCoffSymbolTable::get_strtab() const {
    return p_strtab;
}

void
SgAsmCoffSymbolTable::set_strtab(SgAsmGenericSection* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

SgAsmCoffSymbolList* const&
SgAsmCoffSymbolTable::get_symbols() const {
    return p_symbols;
}

void
SgAsmCoffSymbolTable::set_symbols(SgAsmCoffSymbolList* const& x) {
    changeChildPointer(this->p_symbols, const_cast<SgAsmCoffSymbolList*&>(x));
    set_isModified(true);
}

SgAsmCoffSymbolTable::~SgAsmCoffSymbolTable() {
    destructorHelper();
}

SgAsmCoffSymbolTable::SgAsmCoffSymbolTable()
    : p_strtab(nullptr)
    , p_symbols(createAndParent<SgAsmCoffSymbolList>(this)) {}

void
SgAsmCoffSymbolTable::initializeProperties() {
    p_strtab = nullptr;
    p_symbols = createAndParent<SgAsmCoffSymbolList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
