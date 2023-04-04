//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmConstantPool            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmJvmConstantPoolEntryPtrList const&
SgAsmJvmConstantPool::get_entries() const {
    return p_entries;
}

SgAsmJvmConstantPoolEntryPtrList&
SgAsmJvmConstantPool::get_entries() {
    return p_entries;
}

void
SgAsmJvmConstantPool::set_entries(SgAsmJvmConstantPoolEntryPtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmJvmConstantPool::~SgAsmJvmConstantPool() {
    destructorHelper();
}

SgAsmJvmConstantPool::SgAsmJvmConstantPool() {}

void
SgAsmJvmConstantPool::initializeProperties() {
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
