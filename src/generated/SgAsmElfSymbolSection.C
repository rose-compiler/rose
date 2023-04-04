//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbolSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

bool const&
SgAsmElfSymbolSection::get_is_dynamic() const {
    return p_is_dynamic;
}

void
SgAsmElfSymbolSection::set_is_dynamic(bool const& x) {
    this->p_is_dynamic = x;
    set_isModified(true);
}

SgAsmElfSymbolList* const&
SgAsmElfSymbolSection::get_symbols() const {
    return p_symbols;
}

void
SgAsmElfSymbolSection::set_symbols(SgAsmElfSymbolList* const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

SgAsmElfSymbolSection::~SgAsmElfSymbolSection() {
    destructorHelper();
}

SgAsmElfSymbolSection::SgAsmElfSymbolSection()
    : p_is_dynamic(false)
    , p_symbols(createAndParent<SgAsmElfSymbolList>(this)) {}

void
SgAsmElfSymbolSection::initializeProperties() {
    p_is_dynamic = false;
    p_symbols = createAndParent<SgAsmElfSymbolList>(this);
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
