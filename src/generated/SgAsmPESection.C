#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPESection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmPESectionTableEntry* const&
SgAsmPESection::get_section_entry() const {
    return p_section_entry;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESection::set_section_entry(SgAsmPESectionTableEntry* const& x) {
    this->p_section_entry = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmPESection::~SgAsmPESection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmPESection::SgAsmPESection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_section_entry(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESection::initializeProperties() {
    p_section_entry = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
