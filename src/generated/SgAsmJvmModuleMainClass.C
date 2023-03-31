//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmModuleMainClass                -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint16_t const&
SgAsmJvmModuleMainClass::get_main_class_index() const {
    return p_main_class_index;
}

void
SgAsmJvmModuleMainClass::set_main_class_index(uint16_t const& x) {
    this->p_main_class_index = x;
    set_isModified(true);
}

SgAsmJvmModuleMainClass::~SgAsmJvmModuleMainClass() {
    destructorHelper();
}

SgAsmJvmModuleMainClass::SgAsmJvmModuleMainClass()
    : p_main_class_index(0) {}

void
SgAsmJvmModuleMainClass::initializeProperties() {
    p_main_class_index = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
