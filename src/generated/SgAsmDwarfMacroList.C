//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfMacroList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmDwarfMacroPtrList const&
SgAsmDwarfMacroList::get_macro_list() const {
    return p_macro_list;
}

void
SgAsmDwarfMacroList::set_macro_list(SgAsmDwarfMacroPtrList const& x) {
    this->p_macro_list = x;
    set_isModified(true);
}

SgAsmDwarfMacroList::~SgAsmDwarfMacroList() {
    destructorHelper();
}

SgAsmDwarfMacroList::SgAsmDwarfMacroList() {}

void
SgAsmDwarfMacroList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
