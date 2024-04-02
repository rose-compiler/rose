//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmMethodTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmMethodTable_IMPL
#include <sage3basic.h>

SgAsmJvmMethodPtrList const&
SgAsmJvmMethodTable::get_methods() const {
    return p_methods;
}

SgAsmJvmMethodPtrList&
SgAsmJvmMethodTable::get_methods() {
    return p_methods;
}

void
SgAsmJvmMethodTable::set_methods(SgAsmJvmMethodPtrList const& x) {
    changeChildPointer(this->p_methods, const_cast<SgAsmJvmMethodPtrList&>(x));
    set_isModified(true);
}

SgAsmJvmMethodTable::~SgAsmJvmMethodTable() {
    destructorHelper();
}

SgAsmJvmMethodTable::SgAsmJvmMethodTable() {}

void
SgAsmJvmMethodTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
