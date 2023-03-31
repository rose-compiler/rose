//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverEntry                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

size_t const&
SgAsmElfSymverEntry::get_value() const {
    return p_value;
}

void
SgAsmElfSymverEntry::set_value(size_t const& x) {
    this->p_value = x;
    set_isModified(true);
}

SgAsmElfSymverEntry::~SgAsmElfSymverEntry() {
    destructorHelper();
}

SgAsmElfSymverEntry::SgAsmElfSymverEntry()
    : p_value(0) {}

void
SgAsmElfSymverEntry::initializeProperties() {
    p_value = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
