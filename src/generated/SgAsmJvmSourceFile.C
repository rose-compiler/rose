//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmSourceFile                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint16_t const&
SgAsmJvmSourceFile::get_sourcefile_index() const {
    return p_sourcefile_index;
}

void
SgAsmJvmSourceFile::set_sourcefile_index(uint16_t const& x) {
    this->p_sourcefile_index = x;
    set_isModified(true);
}

SgAsmJvmSourceFile::~SgAsmJvmSourceFile() {
    destructorHelper();
}

SgAsmJvmSourceFile::SgAsmJvmSourceFile()
    : p_sourcefile_index(0) {}

void
SgAsmJvmSourceFile::initializeProperties() {
    p_sourcefile_index = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
