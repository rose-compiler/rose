#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmLineNumberEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmLineNumberEntry::get_start_pc() const {
    return p_start_pc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberEntry::set_start_pc(uint16_t const& x) {
    this->p_start_pc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmLineNumberEntry::get_line_number() const {
    return p_line_number;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberEntry::set_line_number(uint16_t const& x) {
    this->p_line_number = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberEntry::~SgAsmJvmLineNumberEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmLineNumberEntry::SgAsmJvmLineNumberEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_start_pc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_line_number(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmLineNumberEntry::initializeProperties() {
    p_start_pc = 0;
    p_line_number = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
