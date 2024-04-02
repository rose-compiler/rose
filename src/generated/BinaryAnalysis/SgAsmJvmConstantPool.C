//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmConstantPool            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmConstantPool_IMPL
#include <SgAsmJvmConstantPool.h>

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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
