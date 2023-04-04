#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericFileList            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFilePtrList const&
SgAsmGenericFileList::get_files() const {
    return p_files;
}

#line 552 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFilePtrList&
SgAsmGenericFileList::get_files() {
    return p_files;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFileList::set_files(SgAsmGenericFilePtrList const& x) {
    this->p_files = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFileList::~SgAsmGenericFileList() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericFileList::SgAsmGenericFileList() {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericFileList::initializeProperties() {
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
