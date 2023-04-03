#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmAttribute            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmAttribute::get_attribute_name_index() const {
    return p_attribute_name_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmAttribute::set_attribute_name_index(uint16_t const& x) {
    this->p_attribute_name_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmJvmAttribute::get_attribute_length() const {
    return p_attribute_length;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmAttribute::set_attribute_length(uint32_t const& x) {
    this->p_attribute_length = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttribute::~SgAsmJvmAttribute() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttribute::SgAsmJvmAttribute()
    : p_attribute_name_index(0)
    , p_attribute_length(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmAttribute::initializeProperties() {
    p_attribute_name_index = 0;
    p_attribute_length = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
