#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmField            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmField::get_access_flags() const {
    return p_access_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmField::set_access_flags(uint16_t const& x) {
    this->p_access_flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmField::get_name_index() const {
    return p_name_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmField::set_name_index(uint16_t const& x) {
    this->p_name_index = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmField::get_descriptor_index() const {
    return p_descriptor_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmField::set_descriptor_index(uint16_t const& x) {
    this->p_descriptor_index = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttributeTable* const&
SgAsmJvmField::get_attribute_table() const {
    return p_attribute_table;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmField::set_attribute_table(SgAsmJvmAttributeTable* const& x) {
    this->p_attribute_table = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmField::~SgAsmJvmField() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmField::SgAsmJvmField()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_access_flags(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_name_index(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_descriptor_index(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_attribute_table(nullptr) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmField::initializeProperties() {
    p_access_flags = 0;
    p_name_index = 0;
    p_descriptor_index = 0;
    p_attribute_table = nullptr;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
