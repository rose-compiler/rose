#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNESectionTable            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNESectionTable::get_flags() const {
    return p_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::set_flags(unsigned const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
unsigned const&
SgAsmNESectionTable::get_sector() const {
    return p_sector;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::set_sector(unsigned const& x) {
    this->p_sector = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmNESectionTable::get_physical_size() const {
    return p_physical_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::set_physical_size(rose_addr_t const& x) {
    this->p_physical_size = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmNESectionTable::get_virtual_size() const {
    return p_virtual_size;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::set_virtual_size(rose_addr_t const& x) {
    this->p_virtual_size = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmNESectionTable::~SgAsmNESectionTable() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmNESectionTable::SgAsmNESectionTable()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_flags(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_sector(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_physical_size(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_virtual_size(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmNESectionTable::initializeProperties() {
    p_flags = 0;
    p_sector = 0;
    p_physical_size = 0;
    p_virtual_size = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
