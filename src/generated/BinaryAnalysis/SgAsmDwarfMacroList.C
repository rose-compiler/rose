//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfMacroList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDwarfMacroList_IMPL
#include <sage3basic.h>

SgAsmDwarfMacroPtrList const&
SgAsmDwarfMacroList::get_macro_list() const {
    return p_macro_list;
}

void
SgAsmDwarfMacroList::set_macro_list(SgAsmDwarfMacroPtrList const& x) {
    changeChildPointer(this->p_macro_list, const_cast<SgAsmDwarfMacroPtrList&>(x));
    set_isModified(true);
}

SgAsmDwarfMacroList::~SgAsmDwarfMacroList() {
    destructorHelper();
}

SgAsmDwarfMacroList::SgAsmDwarfMacroList() {}

void
SgAsmDwarfMacroList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
