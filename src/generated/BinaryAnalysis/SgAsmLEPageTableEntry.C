//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEPageTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmLEPageTableEntry_IMPL
#include <sage3basic.h>

unsigned const&
SgAsmLEPageTableEntry::get_pageno() const {
    return p_pageno;
}

void
SgAsmLEPageTableEntry::set_pageno(unsigned const& x) {
    this->p_pageno = x;
    set_isModified(true);
}

unsigned const&
SgAsmLEPageTableEntry::get_flags() const {
    return p_flags;
}

void
SgAsmLEPageTableEntry::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

SgAsmLEPageTableEntry::~SgAsmLEPageTableEntry() {
    destructorHelper();
}

SgAsmLEPageTableEntry::SgAsmLEPageTableEntry()
    : p_pageno(0)
    , p_flags(0) {}

void
SgAsmLEPageTableEntry::initializeProperties() {
    p_pageno = 0;
    p_flags = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
