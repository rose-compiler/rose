#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLESectionTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLESectionTableEntry::get_flags() const {
    return p_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLESectionTableEntry::get_pagemap_index() const {
    return p_pagemap_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_pagemap_index(unsigned const& x) {
    this->p_pagemap_index = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLESectionTableEntry::get_pagemap_nentries() const {
    return p_pagemap_nentries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_pagemap_nentries(unsigned const& x) {
    this->p_pagemap_nentries = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLESectionTableEntry::get_res1() const {
    return p_res1;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_res1(unsigned const& x) {
    this->p_res1 = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmLESectionTableEntry::get_mapped_size() const {
    return p_mapped_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_mapped_size(rose_addr_t const& x) {
    this->p_mapped_size = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmLESectionTableEntry::get_base_addr() const {
    return p_base_addr;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_base_addr(rose_addr_t const& x) {
    this->p_base_addr = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmLESectionTableEntry::~SgAsmLESectionTableEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmLESectionTableEntry::SgAsmLESectionTableEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_flags(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_pagemap_index(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_pagemap_nentries(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_res1(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_mapped_size(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_base_addr(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::initializeProperties() {
    p_flags = 0;
    p_pagemap_index = 0;
    p_pagemap_nentries = 0;
    p_res1 = 0;
    p_mapped_size = 0;
    p_base_addr = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
