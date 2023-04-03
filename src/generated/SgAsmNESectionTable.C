#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESectionTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNESectionTable::get_flags() const {
    return p_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNESectionTable::get_sector() const {
    return p_sector;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::set_sector(unsigned const& x) {
    this->p_sector = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmNESectionTable::get_physical_size() const {
    return p_physical_size;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::set_physical_size(rose_addr_t const& x) {
    this->p_physical_size = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmNESectionTable::get_virtual_size() const {
    return p_virtual_size;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::set_virtual_size(rose_addr_t const& x) {
    this->p_virtual_size = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmNESectionTable::~SgAsmNESectionTable() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmNESectionTable::SgAsmNESectionTable()
    : p_flags(0)
    , p_sector(0)
    , p_physical_size(0)
    , p_virtual_size(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::initializeProperties() {
    p_flags = 0;
    p_sector = 0;
    p_physical_size = 0;
    p_virtual_size = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
