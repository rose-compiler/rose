//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFileList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmGenericFileList_IMPL
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
    changeChildPointer(this->p_files, const_cast<SgAsmGenericFilePtrList&>(x));
    set_isModified(true);
}

SgAsmGenericFileList::~SgAsmGenericFileList() {
    destructorHelper();
}

SgAsmGenericFileList::SgAsmGenericFileList() {}

void
SgAsmGenericFileList::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
