//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSectionList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmGenericSectionList_IMPL
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
    changeChildPointer(this->p_sections, const_cast<SgAsmGenericSectionPtrList&>(x));
    set_isModified(true);
}

SgAsmGenericSectionList::~SgAsmGenericSectionList() {
    destructorHelper();
}

SgAsmGenericSectionList::SgAsmGenericSectionList() {}

void
SgAsmGenericSectionList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
