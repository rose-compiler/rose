//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmFieldTable                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
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
    this->p_fields = x;
    set_isModified(true);
}

SgAsmJvmFieldTable::~SgAsmJvmFieldTable() {
    destructorHelper();
}

SgAsmJvmFieldTable::SgAsmJvmFieldTable() {}

void
SgAsmJvmFieldTable::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
