#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLESection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmLESectionTableEntry* const&
SgAsmLESection::get_st_entry() const {
    return p_st_entry;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESection::set_st_entry(SgAsmLESectionTableEntry* const& x) {
    this->p_st_entry = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmLESection::~SgAsmLESection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmLESection::SgAsmLESection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_st_entry(NULL) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESection::initializeProperties() {
    p_st_entry = NULL;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
