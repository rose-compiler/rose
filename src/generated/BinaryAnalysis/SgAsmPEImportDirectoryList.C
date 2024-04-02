//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportDirectoryList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPEImportDirectoryList_IMPL
#include <SgAsmPEImportDirectoryList.h>

SgAsmPEImportDirectoryPtrList const&
SgAsmPEImportDirectoryList::get_vector() const {
    return p_vector;
}

SgAsmPEImportDirectoryPtrList&
SgAsmPEImportDirectoryList::get_vector() {
    return p_vector;
}

void
SgAsmPEImportDirectoryList::set_vector(SgAsmPEImportDirectoryPtrList const& x) {
    changeChildPointer(this->p_vector, const_cast<SgAsmPEImportDirectoryPtrList&>(x));
    set_isModified(true);
}

SgAsmPEImportDirectoryList::~SgAsmPEImportDirectoryList() {
    destructorHelper();
}

SgAsmPEImportDirectoryList::SgAsmPEImportDirectoryList() {}

void
SgAsmPEImportDirectoryList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
