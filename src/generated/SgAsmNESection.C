#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmNESectionTableEntry* const&
SgAsmNESection::get_st_entry() const {
    return p_st_entry;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESection::set_st_entry(SgAsmNESectionTableEntry* const& x) {
    this->p_st_entry = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmNERelocTable* const&
SgAsmNESection::get_reloc_table() const {
    return p_reloc_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESection::set_reloc_table(SgAsmNERelocTable* const& x) {
    this->p_reloc_table = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmNESection::~SgAsmNESection() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmNESection::SgAsmNESection()
    : p_st_entry(nullptr)
    , p_reloc_table(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESection::initializeProperties() {
    p_st_entry = nullptr;
    p_reloc_table = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
