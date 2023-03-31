//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLESectionTableEntry                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

unsigned const&
SgAsmLESectionTableEntry::get_flags() const {
    return p_flags;
}

void
SgAsmLESectionTableEntry::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

unsigned const&
SgAsmLESectionTableEntry::get_pagemap_index() const {
    return p_pagemap_index;
}

void
SgAsmLESectionTableEntry::set_pagemap_index(unsigned const& x) {
    this->p_pagemap_index = x;
    set_isModified(true);
}

unsigned const&
SgAsmLESectionTableEntry::get_pagemap_nentries() const {
    return p_pagemap_nentries;
}

void
SgAsmLESectionTableEntry::set_pagemap_nentries(unsigned const& x) {
    this->p_pagemap_nentries = x;
    set_isModified(true);
}

unsigned const&
SgAsmLESectionTableEntry::get_res1() const {
    return p_res1;
}

void
SgAsmLESectionTableEntry::set_res1(unsigned const& x) {
    this->p_res1 = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmLESectionTableEntry::get_mapped_size() const {
    return p_mapped_size;
}

void
SgAsmLESectionTableEntry::set_mapped_size(rose_addr_t const& x) {
    this->p_mapped_size = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmLESectionTableEntry::get_base_addr() const {
    return p_base_addr;
}

void
SgAsmLESectionTableEntry::set_base_addr(rose_addr_t const& x) {
    this->p_base_addr = x;
    set_isModified(true);
}

SgAsmLESectionTableEntry::~SgAsmLESectionTableEntry() {
    destructorHelper();
}

SgAsmLESectionTableEntry::SgAsmLESectionTableEntry()
    : p_flags(0)
    , p_pagemap_index(0)
    , p_pagemap_nentries(0)
    , p_res1(0)
    , p_mapped_size(0)
    , p_base_addr(0) {}

void
SgAsmLESectionTableEntry::initializeProperties() {
    p_flags = 0;
    p_pagemap_index = 0;
    p_pagemap_nentries = 0;
    p_res1 = 0;
    p_mapped_size = 0;
    p_base_addr = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
