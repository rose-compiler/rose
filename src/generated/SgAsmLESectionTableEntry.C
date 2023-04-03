#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLESectionTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLESectionTableEntry::get_flags() const {
    return p_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLESectionTableEntry::get_pagemap_index() const {
    return p_pagemap_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_pagemap_index(unsigned const& x) {
    this->p_pagemap_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLESectionTableEntry::get_pagemap_nentries() const {
    return p_pagemap_nentries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_pagemap_nentries(unsigned const& x) {
    this->p_pagemap_nentries = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLESectionTableEntry::get_res1() const {
    return p_res1;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_res1(unsigned const& x) {
    this->p_res1 = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmLESectionTableEntry::get_mapped_size() const {
    return p_mapped_size;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_mapped_size(rose_addr_t const& x) {
    this->p_mapped_size = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmLESectionTableEntry::get_base_addr() const {
    return p_base_addr;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::set_base_addr(rose_addr_t const& x) {
    this->p_base_addr = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmLESectionTableEntry::~SgAsmLESectionTableEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmLESectionTableEntry::SgAsmLESectionTableEntry()
    : p_flags(0)
    , p_pagemap_index(0)
    , p_pagemap_nentries(0)
    , p_res1(0)
    , p_mapped_size(0)
    , p_base_addr(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESectionTableEntry::initializeProperties() {
    p_flags = 0;
    p_pagemap_index = 0;
    p_pagemap_nentries = 0;
    p_res1 = 0;
    p_mapped_size = 0;
    p_base_addr = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
