//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESectionTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

unsigned const&
SgAsmNESectionTable::get_flags() const {
    return p_flags;
}

void
SgAsmNESectionTable::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

unsigned const&
SgAsmNESectionTable::get_sector() const {
    return p_sector;
}

void
SgAsmNESectionTable::set_sector(unsigned const& x) {
    this->p_sector = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNESectionTable::get_physical_size() const {
    return p_physical_size;
}

void
SgAsmNESectionTable::set_physical_size(rose_addr_t const& x) {
    this->p_physical_size = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNESectionTable::get_virtual_size() const {
    return p_virtual_size;
}

void
SgAsmNESectionTable::set_virtual_size(rose_addr_t const& x) {
    this->p_virtual_size = x;
    set_isModified(true);
}

SgAsmNESectionTable::~SgAsmNESectionTable() {
    destructorHelper();
}

SgAsmNESectionTable::SgAsmNESectionTable()
    : p_flags(0)
    , p_sector(0)
    , p_physical_size(0)
    , p_virtual_size(0) {}

void
SgAsmNESectionTable::initializeProperties() {
    p_flags = 0;
    p_sector = 0;
    p_physical_size = 0;
    p_virtual_size = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
