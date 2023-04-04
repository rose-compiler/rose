#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmSourceFile            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmSourceFile::get_sourcefile_index() const {
    return p_sourcefile_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmSourceFile::set_sourcefile_index(uint16_t const& x) {
    this->p_sourcefile_index = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmSourceFile::~SgAsmJvmSourceFile() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmSourceFile::SgAsmJvmSourceFile()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_sourcefile_index(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmSourceFile::initializeProperties() {
    p_sourcefile_index = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
