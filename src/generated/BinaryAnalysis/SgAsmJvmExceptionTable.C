//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmExceptionTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmExceptionTable_IMPL
#include <sage3basic.h>

SgAsmJvmExceptionHandlerPtrList const&
SgAsmJvmExceptionTable::get_handlers() const {
    return p_handlers;
}

SgAsmJvmExceptionHandlerPtrList&
SgAsmJvmExceptionTable::get_handlers() {
    return p_handlers;
}

void
SgAsmJvmExceptionTable::set_handlers(SgAsmJvmExceptionHandlerPtrList const& x) {
    this->p_handlers = x;
    set_isModified(true);
}

SgAsmJvmExceptionTable::~SgAsmJvmExceptionTable() {
    destructorHelper();
}

SgAsmJvmExceptionTable::SgAsmJvmExceptionTable() {}

void
SgAsmJvmExceptionTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
