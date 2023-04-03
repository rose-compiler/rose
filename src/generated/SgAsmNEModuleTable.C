#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEModuleTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmNEStringTable* const&
SgAsmNEModuleTable::get_strtab() const {
    return p_strtab;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEModuleTable::set_strtab(SgAsmNEStringTable* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAddressList const&
SgAsmNEModuleTable::get_name_offsets() const {
    return p_name_offsets;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEModuleTable::set_name_offsets(SgAddressList const& x) {
    this->p_name_offsets = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgStringList const&
SgAsmNEModuleTable::get_names() const {
    return p_names;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEModuleTable::set_names(SgStringList const& x) {
    this->p_names = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmNEModuleTable::~SgAsmNEModuleTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmNEModuleTable::SgAsmNEModuleTable()
    : p_strtab(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNEModuleTable::initializeProperties() {
    p_strtab = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
