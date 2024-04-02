//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmExceptions            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmExceptions_IMPL
#include <SgAsmJvmExceptions.h>

SgUnsigned16List const&
SgAsmJvmExceptions::get_exception_index_table() const {
    return p_exception_index_table;
}

void
SgAsmJvmExceptions::set_exception_index_table(SgUnsigned16List const& x) {
    this->p_exception_index_table = x;
    set_isModified(true);
}

SgAsmJvmExceptions::~SgAsmJvmExceptions() {
    destructorHelper();
}

SgAsmJvmExceptions::SgAsmJvmExceptions() {}

void
SgAsmJvmExceptions::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
