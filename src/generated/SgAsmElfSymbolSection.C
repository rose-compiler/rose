#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbolSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmElfSymbolSection::get_is_dynamic() const {
    return p_is_dynamic;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbolSection::set_is_dynamic(bool const& x) {
    this->p_is_dynamic = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolList* const&
SgAsmElfSymbolSection::get_symbols() const {
    return p_symbols;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbolSection::set_symbols(SgAsmElfSymbolList* const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolSection::~SgAsmElfSymbolSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolSection::SgAsmElfSymbolSection()
    : p_is_dynamic(false)
    , p_symbols(createAndParent<SgAsmElfSymbolList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbolSection::initializeProperties() {
    p_is_dynamic = false;
    p_symbols = createAndParent<SgAsmElfSymbolList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
