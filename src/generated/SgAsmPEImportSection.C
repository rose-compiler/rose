#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportDirectoryList* const&
SgAsmPEImportSection::get_import_directories() const {
    return p_import_directories;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportSection::set_import_directories(SgAsmPEImportDirectoryList* const& x) {
    this->p_import_directories = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportSection::~SgAsmPEImportSection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportSection::SgAsmPEImportSection()
    : p_import_directories(createAndParent<SgAsmPEImportDirectoryList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportSection::initializeProperties() {
    p_import_directories = createAndParent<SgAsmPEImportDirectoryList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
