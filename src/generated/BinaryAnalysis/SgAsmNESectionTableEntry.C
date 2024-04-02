//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESectionTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNESectionTableEntry_IMPL
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
SgAsmNESectionTableEntry::get_physicalSize() const {
    return p_physicalSize;
}

void
SgAsmNESectionTableEntry::set_physicalSize(rose_addr_t const& x) {
    this->p_physicalSize = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNESectionTableEntry::get_virtualSize() const {
    return p_virtualSize;
}

void
SgAsmNESectionTableEntry::set_virtualSize(rose_addr_t const& x) {
    this->p_virtualSize = x;
    set_isModified(true);
}

SgAsmNESectionTableEntry::~SgAsmNESectionTableEntry() {
    destructorHelper();
}

SgAsmNESectionTableEntry::SgAsmNESectionTableEntry()
    : p_flags(0)
    , p_sector(0)
    , p_physicalSize(0)
    , p_virtualSize(0) {}

void
SgAsmNESectionTableEntry::initializeProperties() {
    p_flags = 0;
    p_sector = 0;
    p_physicalSize = 0;
    p_virtualSize = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
