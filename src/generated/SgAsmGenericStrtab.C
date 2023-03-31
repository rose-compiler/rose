//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericStrtab                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

SgAsmGenericSection* const&
SgAsmGenericStrtab::get_container() const {
    return p_container;
}

void
SgAsmGenericStrtab::set_container(SgAsmGenericSection* const& x) {
    this->p_container = x;
    set_isModified(true);
}

SgAsmGenericStrtab::referenced_t const&
SgAsmGenericStrtab::get_storage_list() const {
    return p_storage_list;
}

void
SgAsmGenericStrtab::set_storage_list(SgAsmGenericStrtab::referenced_t const& x) {
    this->p_storage_list = x;
    set_isModified(true);
}

SgAsmStringStorage* const&
SgAsmGenericStrtab::get_dont_free() const {
    return p_dont_free;
}

void
SgAsmGenericStrtab::set_dont_free(SgAsmStringStorage* const& x) {
    this->p_dont_free = x;
    set_isModified(true);
}

size_t const&
SgAsmGenericStrtab::get_num_freed() const {
    return p_num_freed;
}

void
SgAsmGenericStrtab::set_num_freed(size_t const& x) {
    this->p_num_freed = x;
    set_isModified(true);
}

SgAsmGenericStrtab::~SgAsmGenericStrtab() {
    destructorHelper();
}

SgAsmGenericStrtab::SgAsmGenericStrtab()
    : p_container(nullptr)
    , p_dont_free(nullptr)
    , p_num_freed(0) {}

void
SgAsmGenericStrtab::initializeProperties() {
    p_container = nullptr;
    p_dont_free = nullptr;
    p_num_freed = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
