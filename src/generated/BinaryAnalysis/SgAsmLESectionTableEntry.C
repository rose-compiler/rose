//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLESectionTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmLESectionTableEntry_IMPL
#include <SgAsmLESectionTableEntry.h>

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
SgAsmLESectionTableEntry::get_pageMapIndex() const {
    return p_pageMapIndex;
}

void
SgAsmLESectionTableEntry::set_pageMapIndex(unsigned const& x) {
    this->p_pageMapIndex = x;
    set_isModified(true);
}

unsigned const&
SgAsmLESectionTableEntry::get_pageMapNEntries() const {
    return p_pageMapNEntries;
}

void
SgAsmLESectionTableEntry::set_pageMapNEntries(unsigned const& x) {
    this->p_pageMapNEntries = x;
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
SgAsmLESectionTableEntry::get_mappedSize() const {
    return p_mappedSize;
}

void
SgAsmLESectionTableEntry::set_mappedSize(rose_addr_t const& x) {
    this->p_mappedSize = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmLESectionTableEntry::get_baseAddr() const {
    return p_baseAddr;
}

void
SgAsmLESectionTableEntry::set_baseAddr(rose_addr_t const& x) {
    this->p_baseAddr = x;
    set_isModified(true);
}

SgAsmLESectionTableEntry::~SgAsmLESectionTableEntry() {
    destructorHelper();
}

SgAsmLESectionTableEntry::SgAsmLESectionTableEntry()
    : p_flags(0)
    , p_pageMapIndex(0)
    , p_pageMapNEntries(0)
    , p_res1(0)
    , p_mappedSize(0)
    , p_baseAddr(0) {}

void
SgAsmLESectionTableEntry::initializeProperties() {
    p_flags = 0;
    p_pageMapIndex = 0;
    p_pageMapNEntries = 0;
    p_res1 = 0;
    p_mappedSize = 0;
    p_baseAddr = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
