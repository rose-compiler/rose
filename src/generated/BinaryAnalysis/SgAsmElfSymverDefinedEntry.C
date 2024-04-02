//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfSymverDefinedEntry_IMPL
#include <sage3basic.h>

size_t const&
SgAsmElfSymverDefinedEntry::get_version() const {
    return p_version;
}

void
SgAsmElfSymverDefinedEntry::set_version(size_t const& x) {
    this->p_version = x;
    set_isModified(true);
}

int const&
SgAsmElfSymverDefinedEntry::get_flags() const {
    return p_flags;
}

void
SgAsmElfSymverDefinedEntry::set_flags(int const& x) {
    this->p_flags = x;
    set_isModified(true);
}

size_t const&
SgAsmElfSymverDefinedEntry::get_index() const {
    return p_index;
}

void
SgAsmElfSymverDefinedEntry::set_index(size_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

uint32_t const&
SgAsmElfSymverDefinedEntry::get_hash() const {
    return p_hash;
}

void
SgAsmElfSymverDefinedEntry::set_hash(uint32_t const& x) {
    this->p_hash = x;
    set_isModified(true);
}

SgAsmElfSymverDefinedAuxList* const&
SgAsmElfSymverDefinedEntry::get_entries() const {
    return p_entries;
}

void
SgAsmElfSymverDefinedEntry::set_entries(SgAsmElfSymverDefinedAuxList* const& x) {
    changeChildPointer(this->p_entries, const_cast<SgAsmElfSymverDefinedAuxList*&>(x));
    set_isModified(true);
}

SgAsmElfSymverDefinedEntry::~SgAsmElfSymverDefinedEntry() {
    destructorHelper();
}

SgAsmElfSymverDefinedEntry::SgAsmElfSymverDefinedEntry()
    : p_version(0)
    , p_flags(0)
    , p_index(0)
    , p_hash(0)
    , p_entries(createAndParent<SgAsmElfSymverDefinedAuxList>(this)) {}

void
SgAsmElfSymverDefinedEntry::initializeProperties() {
    p_version = 0;
    p_flags = 0;
    p_index = 0;
    p_hash = 0;
    p_entries = createAndParent<SgAsmElfSymverDefinedAuxList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
