#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmEnclosingMethod            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmEnclosingMethod::get_class_index() const {
    return p_class_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmEnclosingMethod::set_class_index(uint16_t const& x) {
    this->p_class_index = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmEnclosingMethod::get_method_index() const {
    return p_method_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmEnclosingMethod::set_method_index(uint16_t const& x) {
    this->p_method_index = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmEnclosingMethod::~SgAsmJvmEnclosingMethod() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmEnclosingMethod::SgAsmJvmEnclosingMethod()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_class_index(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_method_index(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmEnclosingMethod::initializeProperties() {
    p_class_index = 0;
    p_method_index = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
