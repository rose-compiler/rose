#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStringStorage            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericStrtab* const&
SgAsmStringStorage::get_strtab() const {
    return p_strtab;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStringStorage::set_strtab(SgAsmGenericStrtab* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmStringStorage::get_string() const {
    return p_string;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStringStorage::set_string(std::string const& x) {
    this->p_string = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmStringStorage::get_offset() const {
    return p_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStringStorage::set_offset(rose_addr_t const& x) {
    this->p_offset = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmStringStorage::~SgAsmStringStorage() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmStringStorage::SgAsmStringStorage()
    : p_strtab(nullptr)
    , p_offset(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStringStorage::initializeProperties() {
    p_strtab = nullptr;
    p_offset = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
