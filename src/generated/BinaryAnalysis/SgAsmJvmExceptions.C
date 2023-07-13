//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmExceptions            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgUnsignedList const&
SgAsmJvmExceptions::get_exception_index_table() const {
    return p_exception_index_table;
}

void
SgAsmJvmExceptions::set_exception_index_table(SgUnsignedList const& x) {
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
