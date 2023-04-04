#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymbolSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
bool const&
SgAsmElfSymbolSection::get_is_dynamic() const {
    return p_is_dynamic;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbolSection::set_is_dynamic(bool const& x) {
    this->p_is_dynamic = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolList* const&
SgAsmElfSymbolSection::get_symbols() const {
    return p_symbols;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbolSection::set_symbols(SgAsmElfSymbolList* const& x) {
    this->p_symbols = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolSection::~SgAsmElfSymbolSection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymbolSection::SgAsmElfSymbolSection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_is_dynamic(false)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_symbols(createAndParent<SgAsmElfSymbolList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymbolSection::initializeProperties() {
    p_is_dynamic = false;
    p_symbols = createAndParent<SgAsmElfSymbolList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
