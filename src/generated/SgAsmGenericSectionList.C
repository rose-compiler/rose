#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSectionList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionPtrList const&
SgAsmGenericSectionList::get_sections() const {
    return p_sections;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionPtrList&
SgAsmGenericSectionList::get_sections() {
    return p_sections;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSectionList::set_sections(SgAsmGenericSectionPtrList const& x) {
    this->p_sections = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionList::~SgAsmGenericSectionList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSectionList::SgAsmGenericSectionList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericSectionList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
