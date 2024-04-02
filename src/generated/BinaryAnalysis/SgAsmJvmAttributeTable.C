//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmAttributeTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmAttributeTable_IMPL
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
    changeChildPointer(this->p_attributes, const_cast<SgAsmJvmAttributePtrList&>(x));
    set_isModified(true);
}

SgAsmJvmAttributeTable::~SgAsmJvmAttributeTable() {
    destructorHelper();
}

SgAsmJvmAttributeTable::SgAsmJvmAttributeTable() {}

void
SgAsmJvmAttributeTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
