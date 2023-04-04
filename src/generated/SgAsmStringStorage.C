#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmStringStorage            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericStrtab* const&
SgAsmStringStorage::get_strtab() const {
    return p_strtab;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStringStorage::set_strtab(SgAsmGenericStrtab* const& x) {
    this->p_strtab = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
std::string const&
SgAsmStringStorage::get_string() const {
    return p_string;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStringStorage::set_string(std::string const& x) {
    this->p_string = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmStringStorage::get_offset() const {
    return p_offset;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStringStorage::set_offset(rose_addr_t const& x) {
    this->p_offset = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmStringStorage::~SgAsmStringStorage() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmStringStorage::SgAsmStringStorage()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_strtab(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_offset(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmStringStorage::initializeProperties() {
    p_strtab = nullptr;
    p_offset = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
