#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmLESection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmLESectionTableEntry* const&
SgAsmLESection::get_st_entry() const {
    return p_st_entry;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESection::set_st_entry(SgAsmLESectionTableEntry* const& x) {
    this->p_st_entry = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmLESection::~SgAsmLESection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmLESection::SgAsmLESection()
    : p_st_entry(NULL) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmLESection::initializeProperties() {
    p_st_entry = NULL;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
