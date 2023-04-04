//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStoredString            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

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

#endif // ROSE_ENABLE_BINARY_ANALYSIS
