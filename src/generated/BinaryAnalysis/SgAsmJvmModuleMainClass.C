//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmModuleMainClass            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmModuleMainClass_IMPL
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

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
