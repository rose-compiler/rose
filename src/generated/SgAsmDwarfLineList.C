#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDwarfLineList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLinePtrList const&
SgAsmDwarfLineList::get_line_list() const {
    return p_line_list;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLinePtrList&
SgAsmDwarfLineList::get_line_list() {
    return p_line_list;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLineList::set_line_list(SgAsmDwarfLinePtrList const& x) {
    this->p_line_list = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLineList::~SgAsmDwarfLineList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmDwarfLineList::SgAsmDwarfLineList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmDwarfLineList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
