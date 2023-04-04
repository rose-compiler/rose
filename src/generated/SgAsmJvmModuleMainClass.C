#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmModuleMainClass            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmModuleMainClass::get_main_class_index() const {
    return p_main_class_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmModuleMainClass::set_main_class_index(uint16_t const& x) {
    this->p_main_class_index = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmModuleMainClass::~SgAsmJvmModuleMainClass() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmModuleMainClass::SgAsmJvmModuleMainClass()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_main_class_index(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmModuleMainClass::initializeProperties() {
    p_main_class_index = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
