//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmStackMapTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmJvmStackMapFramePtrList const&
SgAsmJvmStackMapTable::get_entries() const {
    return p_entries;
}

SgAsmJvmStackMapFramePtrList&
SgAsmJvmStackMapTable::get_entries() {
    return p_entries;
}

void
SgAsmJvmStackMapTable::set_entries(SgAsmJvmStackMapFramePtrList const& x) {
    this->p_entries = x;
    set_isModified(true);
}

SgAsmJvmStackMapTable::~SgAsmJvmStackMapTable() {
    destructorHelper();
}

SgAsmJvmStackMapTable::SgAsmJvmStackMapTable() {}

void
SgAsmJvmStackMapTable::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
