#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmConstantValue            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmConstantValue::get_constantvalue_index() const {
    return p_constantvalue_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmConstantValue::set_constantvalue_index(uint16_t const& x) {
    this->p_constantvalue_index = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmConstantValue::~SgAsmJvmConstantValue() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmConstantValue::SgAsmJvmConstantValue()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_constantvalue_index(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmConstantValue::initializeProperties() {
    p_constantvalue_index = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
