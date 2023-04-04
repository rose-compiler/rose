#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNESectionTableEntry* const&
SgAsmNESection::get_st_entry() const {
    return p_st_entry;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESection::set_st_entry(SgAsmNESectionTableEntry* const& x) {
    this->p_st_entry = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocTable* const&
SgAsmNESection::get_reloc_table() const {
    return p_reloc_table;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESection::set_reloc_table(SgAsmNERelocTable* const& x) {
    this->p_reloc_table = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmNESection::~SgAsmNESection() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmNESection::SgAsmNESection()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_st_entry(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_reloc_table(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESection::initializeProperties() {
    p_st_entry = nullptr;
    p_reloc_table = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
