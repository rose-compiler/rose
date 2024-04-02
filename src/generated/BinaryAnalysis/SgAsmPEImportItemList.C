//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportItemList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPEImportItemList_IMPL
#include <sage3basic.h>

SgAsmPEImportItemPtrList const&
SgAsmPEImportItemList::get_vector() const {
    return p_vector;
}

SgAsmPEImportItemPtrList&
SgAsmPEImportItemList::get_vector() {
    return p_vector;
}

void
SgAsmPEImportItemList::set_vector(SgAsmPEImportItemPtrList const& x) {
    changeChildPointer(this->p_vector, const_cast<SgAsmPEImportItemPtrList&>(x));
    set_isModified(true);
}

SgAsmPEImportItemList::~SgAsmPEImportItemList() {
    destructorHelper();
}

SgAsmPEImportItemList::SgAsmPEImportItemList() {}

void
SgAsmPEImportItemList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
