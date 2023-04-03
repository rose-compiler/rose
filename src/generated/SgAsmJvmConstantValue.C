#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmConstantValue            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmConstantValue::get_constantvalue_index() const {
    return p_constantvalue_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmConstantValue::set_constantvalue_index(uint16_t const& x) {
    this->p_constantvalue_index = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmConstantValue::~SgAsmJvmConstantValue() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmConstantValue::SgAsmJvmConstantValue()
    : p_constantvalue_index(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmConstantValue::initializeProperties() {
    p_constantvalue_index = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
