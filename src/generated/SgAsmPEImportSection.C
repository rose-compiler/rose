#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEImportSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportDirectoryList* const&
SgAsmPEImportSection::get_import_directories() const {
    return p_import_directories;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportSection::set_import_directories(SgAsmPEImportDirectoryList* const& x) {
    this->p_import_directories = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportSection::~SgAsmPEImportSection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmPEImportSection::SgAsmPEImportSection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_import_directories(createAndParent<SgAsmPEImportDirectoryList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPEImportSection::initializeProperties() {
    p_import_directories = createAndParent<SgAsmPEImportDirectoryList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
