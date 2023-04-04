#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLEPageTableEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEPageTableEntry::get_pageno() const {
    return p_pageno;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEPageTableEntry::set_pageno(unsigned const& x) {
    this->p_pageno = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmLEPageTableEntry::get_flags() const {
    return p_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEPageTableEntry::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmLEPageTableEntry::~SgAsmLEPageTableEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmLEPageTableEntry::SgAsmLEPageTableEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_pageno(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_flags(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLEPageTableEntry::initializeProperties() {
    p_pageno = 0;
    p_flags = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
