//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmExceptionTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmJvmExceptionPtrList const&
SgAsmJvmExceptionTable::get_exceptions() const {
    return p_exceptions;
}

SgAsmJvmExceptionPtrList&
SgAsmJvmExceptionTable::get_exceptions() {
    return p_exceptions;
}

void
SgAsmJvmExceptionTable::set_exceptions(SgAsmJvmExceptionPtrList const& x) {
    this->p_exceptions = x;
    set_isModified(true);
}

SgAsmJvmExceptionTable::~SgAsmJvmExceptionTable() {
    destructorHelper();
}

SgAsmJvmExceptionTable::SgAsmJvmExceptionTable() {}

void
SgAsmJvmExceptionTable::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
