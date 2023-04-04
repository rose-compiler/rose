//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFileList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmGenericFilePtrList const&
SgAsmGenericFileList::get_files() const {
    return p_files;
}

SgAsmGenericFilePtrList&
SgAsmGenericFileList::get_files() {
    return p_files;
}

void
SgAsmGenericFileList::set_files(SgAsmGenericFilePtrList const& x) {
    this->p_files = x;
    set_isModified(true);
}

SgAsmGenericFileList::~SgAsmGenericFileList() {
    destructorHelper();
}

SgAsmGenericFileList::SgAsmGenericFileList() {}

void
SgAsmGenericFileList::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
