//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESectionTableEntry                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

unsigned const&
SgAsmNESectionTableEntry::get_flags() const {
    return p_flags;
}

void
SgAsmNESectionTableEntry::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

unsigned const&
SgAsmNESectionTableEntry::get_sector() const {
    return p_sector;
}

void
SgAsmNESectionTableEntry::set_sector(unsigned const& x) {
    this->p_sector = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNESectionTableEntry::get_physical_size() const {
    return p_physical_size;
}

void
SgAsmNESectionTableEntry::set_physical_size(rose_addr_t const& x) {
    this->p_physical_size = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNESectionTableEntry::get_virtual_size() const {
    return p_virtual_size;
}

void
SgAsmNESectionTableEntry::set_virtual_size(rose_addr_t const& x) {
    this->p_virtual_size = x;
    set_isModified(true);
}

SgAsmNESectionTableEntry::~SgAsmNESectionTableEntry() {
    destructorHelper();
}

SgAsmNESectionTableEntry::SgAsmNESectionTableEntry()
    : p_flags(0)
    , p_sector(0)
    , p_physical_size(0)
    , p_virtual_size(0) {}

void
SgAsmNESectionTableEntry::initializeProperties() {
    p_flags = 0;
    p_sector = 0;
    p_physical_size = 0;
    p_virtual_size = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
