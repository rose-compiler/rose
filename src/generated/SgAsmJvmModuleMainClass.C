#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmModuleMainClass            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmModuleMainClass::get_main_class_index() const {
    return p_main_class_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmModuleMainClass::set_main_class_index(uint16_t const& x) {
    this->p_main_class_index = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmModuleMainClass::~SgAsmJvmModuleMainClass() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmModuleMainClass::SgAsmJvmModuleMainClass()
    : p_main_class_index(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmModuleMainClass::initializeProperties() {
    p_main_class_index = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
