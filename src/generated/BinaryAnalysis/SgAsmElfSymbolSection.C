//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbolSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymbolSection_IMPL
#include <SgAsmElfSymbolSection.h>

bool const&
SgAsmElfSymbolSection::get_isDynamic() const {
    return p_isDynamic;
}

void
SgAsmElfSymbolSection::set_isDynamic(bool const& x) {
    this->p_isDynamic = x;
    set_isModified(true);
}

SgAsmElfSymbolList* const&
SgAsmElfSymbolSection::get_symbols() const {
    return p_symbols;
}

void
SgAsmElfSymbolSection::set_symbols(SgAsmElfSymbolList* const& x) {
    changeChildPointer(this->p_symbols, const_cast<SgAsmElfSymbolList*&>(x));
    set_isModified(true);
}

SgAsmElfSymbolSection::~SgAsmElfSymbolSection() {
    destructorHelper();
}

SgAsmElfSymbolSection::SgAsmElfSymbolSection()
    : p_isDynamic(false)
    , p_symbols(createAndParent<SgAsmElfSymbolList>(this)) {}

void
SgAsmElfSymbolSection::initializeProperties() {
    p_isDynamic = false;
    p_symbols = createAndParent<SgAsmElfSymbolList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
