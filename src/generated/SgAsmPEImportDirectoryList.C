//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportDirectoryList                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

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
    this->p_vector = x;
    set_isModified(true);
}

SgAsmPEImportDirectoryList::~SgAsmPEImportDirectoryList() {
    destructorHelper();
}

SgAsmPEImportDirectoryList::SgAsmPEImportDirectoryList() {}

void
SgAsmPEImportDirectoryList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
