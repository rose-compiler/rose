#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEPageTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEPageTableEntry::get_pageno() const {
    return p_pageno;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEPageTableEntry::set_pageno(unsigned const& x) {
    this->p_pageno = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEPageTableEntry::get_flags() const {
    return p_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEPageTableEntry::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmLEPageTableEntry::~SgAsmLEPageTableEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmLEPageTableEntry::SgAsmLEPageTableEntry()
    : p_pageno(0)
    , p_flags(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEPageTableEntry::initializeProperties() {
    p_pageno = 0;
    p_flags = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
