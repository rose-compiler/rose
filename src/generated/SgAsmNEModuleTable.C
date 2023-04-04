#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEModuleTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmNEStringTable* const&
SgAsmNEModuleTable::get_strtab() const {
    return p_strtab;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEModuleTable::set_strtab(SgAsmNEStringTable* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAddressList const&
SgAsmNEModuleTable::get_name_offsets() const {
    return p_name_offsets;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEModuleTable::set_name_offsets(SgAddressList const& x) {
    this->p_name_offsets = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgStringList const&
SgAsmNEModuleTable::get_names() const {
    return p_names;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEModuleTable::set_names(SgStringList const& x) {
    this->p_names = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmNEModuleTable::~SgAsmNEModuleTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmNEModuleTable::SgAsmNEModuleTable()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_strtab(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEModuleTable::initializeProperties() {
    p_strtab = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
