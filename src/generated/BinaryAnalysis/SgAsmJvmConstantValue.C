//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmConstantValue            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmConstantValue_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmConstantValue::get_constantvalue_index() const {
    return p_constantvalue_index;
}

void
SgAsmJvmConstantValue::set_constantvalue_index(uint16_t const& x) {
    this->p_constantvalue_index = x;
    set_isModified(true);
}

SgAsmJvmConstantValue::~SgAsmJvmConstantValue() {
    destructorHelper();
}

SgAsmJvmConstantValue::SgAsmJvmConstantValue()
    : p_constantvalue_index(0) {}

void
SgAsmJvmConstantValue::initializeProperties() {
    p_constantvalue_index = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
