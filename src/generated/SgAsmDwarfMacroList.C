#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfMacroList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfMacroPtrList const&
SgAsmDwarfMacroList::get_macro_list() const {
    return p_macro_list;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfMacroList::set_macro_list(SgAsmDwarfMacroPtrList const& x) {
    this->p_macro_list = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfMacroList::~SgAsmDwarfMacroList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfMacroList::SgAsmDwarfMacroList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfMacroList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
