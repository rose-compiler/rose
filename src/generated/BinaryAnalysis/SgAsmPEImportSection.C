//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmPEImportSection_IMPL
#include <sage3basic.h>

SgAsmPEImportDirectoryList* const&
SgAsmPEImportSection::get_importDirectories() const {
    return p_importDirectories;
}

void
SgAsmPEImportSection::set_importDirectories(SgAsmPEImportDirectoryList* const& x) {
    changeChildPointer(this->p_importDirectories, const_cast<SgAsmPEImportDirectoryList*&>(x));
    set_isModified(true);
}

SgAsmPEImportSection::~SgAsmPEImportSection() {
    destructorHelper();
}

SgAsmPEImportSection::SgAsmPEImportSection()
    : p_importDirectories(createAndParent<SgAsmPEImportDirectoryList>(this)) {}

void
SgAsmPEImportSection::initializeProperties() {
    p_importDirectories = createAndParent<SgAsmPEImportDirectoryList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
