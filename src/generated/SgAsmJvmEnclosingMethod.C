#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmEnclosingMethod            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmEnclosingMethod::get_class_index() const {
    return p_class_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmEnclosingMethod::set_class_index(uint16_t const& x) {
    this->p_class_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmEnclosingMethod::get_method_index() const {
    return p_method_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmEnclosingMethod::set_method_index(uint16_t const& x) {
    this->p_method_index = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmEnclosingMethod::~SgAsmJvmEnclosingMethod() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmEnclosingMethod::SgAsmJvmEnclosingMethod()
    : p_class_index(0)
    , p_method_index(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmEnclosingMethod::initializeProperties() {
    p_class_index = 0;
    p_method_index = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
