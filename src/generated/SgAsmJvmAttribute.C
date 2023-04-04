#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmAttribute            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmAttribute::get_attribute_name_index() const {
    return p_attribute_name_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmAttribute::set_attribute_name_index(uint16_t const& x) {
    this->p_attribute_name_index = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmJvmAttribute::get_attribute_length() const {
    return p_attribute_length;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmAttribute::set_attribute_length(uint32_t const& x) {
    this->p_attribute_length = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttribute::~SgAsmJvmAttribute() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttribute::SgAsmJvmAttribute()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_attribute_name_index(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_attribute_length(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmAttribute::initializeProperties() {
    p_attribute_name_index = 0;
    p_attribute_length = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
