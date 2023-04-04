//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSectionList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmGenericSectionPtrList const&
SgAsmGenericSectionList::get_sections() const {
    return p_sections;
}

SgAsmGenericSectionPtrList&
SgAsmGenericSectionList::get_sections() {
    return p_sections;
}

void
SgAsmGenericSectionList::set_sections(SgAsmGenericSectionPtrList const& x) {
    this->p_sections = x;
    set_isModified(true);
}

SgAsmGenericSectionList::~SgAsmGenericSectionList() {
    destructorHelper();
}

SgAsmGenericSectionList::SgAsmGenericSectionList() {}

void
SgAsmGenericSectionList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
