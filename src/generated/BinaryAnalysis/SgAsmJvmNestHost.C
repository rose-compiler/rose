//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmNestHost            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmNestHost_IMPL
#include <SgAsmJvmNestHost.h>

uint16_t const&
SgAsmJvmNestHost::get_host_class_index() const {
    return p_host_class_index;
}

void
SgAsmJvmNestHost::set_host_class_index(uint16_t const& x) {
    this->p_host_class_index = x;
    set_isModified(true);
}

SgAsmJvmNestHost::~SgAsmJvmNestHost() {
    destructorHelper();
}

SgAsmJvmNestHost::SgAsmJvmNestHost()
    : p_host_class_index(0) {}

void
SgAsmJvmNestHost::initializeProperties() {
    p_host_class_index = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
