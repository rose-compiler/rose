#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfMacroList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfMacroPtrList const&
SgAsmDwarfMacroList::get_macro_list() const {
    return p_macro_list;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfMacroList::set_macro_list(SgAsmDwarfMacroPtrList const& x) {
    this->p_macro_list = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfMacroList::~SgAsmDwarfMacroList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfMacroList::SgAsmDwarfMacroList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfMacroList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
