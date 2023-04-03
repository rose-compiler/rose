#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPESection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmPESectionTableEntry* const&
SgAsmPESection::get_section_entry() const {
    return p_section_entry;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESection::set_section_entry(SgAsmPESectionTableEntry* const& x) {
    this->p_section_entry = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmPESection::~SgAsmPESection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmPESection::SgAsmPESection()
    : p_section_entry(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmPESection::initializeProperties() {
    p_section_entry = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
