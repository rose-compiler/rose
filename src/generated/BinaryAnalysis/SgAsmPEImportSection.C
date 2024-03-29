//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmPEImportDirectoryList* const&
SgAsmPEImportSection::get_import_directories() const {
    return p_import_directories;
}

void
SgAsmPEImportSection::set_import_directories(SgAsmPEImportDirectoryList* const& x) {
    changeChildPointer(this->p_import_directories, const_cast<SgAsmPEImportDirectoryList*&>(x));
    set_isModified(true);
}

SgAsmPEImportSection::~SgAsmPEImportSection() {
    destructorHelper();
}

SgAsmPEImportSection::SgAsmPEImportSection()
    : p_import_directories(createAndParent<SgAsmPEImportDirectoryList>(this)) {}

void
SgAsmPEImportSection::initializeProperties() {
    p_import_directories = createAndParent<SgAsmPEImportDirectoryList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
