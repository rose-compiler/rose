#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSectionList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionPtrList const&
SgAsmGenericSectionList::get_sections() const {
    return p_sections;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionPtrList&
SgAsmGenericSectionList::get_sections() {
    return p_sections;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSectionList::set_sections(SgAsmGenericSectionPtrList const& x) {
    this->p_sections = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionList::~SgAsmGenericSectionList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionList::SgAsmGenericSectionList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSectionList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
