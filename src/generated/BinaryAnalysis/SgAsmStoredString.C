//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStoredString            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmStoredString_IMPL
#include <SgAsmStoredString.h>

SgAsmStringStorage* const&
SgAsmStoredString::get_storage() const {
    return p_storage;
}

void
SgAsmStoredString::set_storage(SgAsmStringStorage* const& x) {
    this->p_storage = x;
    set_isModified(true);
}

SgAsmStoredString::~SgAsmStoredString() {
    destructorHelper();
}

SgAsmStoredString::SgAsmStoredString()
    : p_storage(nullptr) {}

void
SgAsmStoredString::initializeProperties() {
    p_storage = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
