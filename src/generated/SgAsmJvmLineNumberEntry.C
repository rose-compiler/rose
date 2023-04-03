#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLineNumberEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmLineNumberEntry::get_start_pc() const {
    return p_start_pc;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberEntry::set_start_pc(uint16_t const& x) {
    this->p_start_pc = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmLineNumberEntry::get_line_number() const {
    return p_line_number;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberEntry::set_line_number(uint16_t const& x) {
    this->p_line_number = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberEntry::~SgAsmJvmLineNumberEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberEntry::SgAsmJvmLineNumberEntry()
    : p_start_pc(0)
    , p_line_number(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberEntry::initializeProperties() {
    p_start_pc = 0;
    p_line_number = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
