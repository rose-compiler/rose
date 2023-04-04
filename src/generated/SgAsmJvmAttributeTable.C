//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmAttributeTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmJvmAttributePtrList const&
SgAsmJvmAttributeTable::get_attributes() const {
    return p_attributes;
}

SgAsmJvmAttributePtrList&
SgAsmJvmAttributeTable::get_attributes() {
    return p_attributes;
}

void
SgAsmJvmAttributeTable::set_attributes(SgAsmJvmAttributePtrList const& x) {
    this->p_attributes = x;
    set_isModified(true);
}

SgAsmJvmAttributeTable::~SgAsmJvmAttributeTable() {
    destructorHelper();
}

SgAsmJvmAttributeTable::SgAsmJvmAttributeTable() {}

void
SgAsmJvmAttributeTable::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
