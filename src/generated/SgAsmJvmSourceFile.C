#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmSourceFile            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmSourceFile::get_sourcefile_index() const {
    return p_sourcefile_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmSourceFile::set_sourcefile_index(uint16_t const& x) {
    this->p_sourcefile_index = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmSourceFile::~SgAsmJvmSourceFile() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmSourceFile::SgAsmJvmSourceFile()
    : p_sourcefile_index(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmSourceFile::initializeProperties() {
    p_sourcefile_index = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
