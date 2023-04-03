#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFileList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFilePtrList const&
SgAsmGenericFileList::get_files() const {
    return p_files;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFilePtrList&
SgAsmGenericFileList::get_files() {
    return p_files;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFileList::set_files(SgAsmGenericFilePtrList const& x) {
    this->p_files = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFileList::~SgAsmGenericFileList() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFileList::SgAsmGenericFileList() {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFileList::initializeProperties() {
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
