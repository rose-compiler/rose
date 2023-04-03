#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfLineList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLinePtrList const&
SgAsmDwarfLineList::get_line_list() const {
    return p_line_list;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLinePtrList&
SgAsmDwarfLineList::get_line_list() {
    return p_line_list;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLineList::set_line_list(SgAsmDwarfLinePtrList const& x) {
    this->p_line_list = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLineList::~SgAsmDwarfLineList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLineList::SgAsmDwarfLineList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLineList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
