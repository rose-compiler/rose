//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmFieldTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmFieldTable_IMPL
#include <sage3basic.h>

SgAsmJvmFieldPtrList const&
SgAsmJvmFieldTable::get_fields() const {
    return p_fields;
}

SgAsmJvmFieldPtrList&
SgAsmJvmFieldTable::get_fields() {
    return p_fields;
}

void
SgAsmJvmFieldTable::set_fields(SgAsmJvmFieldPtrList const& x) {
    changeChildPointer(this->p_fields, const_cast<SgAsmJvmFieldPtrList&>(x));
    set_isModified(true);
}

SgAsmJvmFieldTable::~SgAsmJvmFieldTable() {
    destructorHelper();
}

SgAsmJvmFieldTable::SgAsmJvmFieldTable() {}

void
SgAsmJvmFieldTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
