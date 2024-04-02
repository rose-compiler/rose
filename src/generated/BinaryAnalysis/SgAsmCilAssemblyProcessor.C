//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilAssemblyProcessor            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmCilAssemblyProcessor_IMPL
#include <sage3basic.h>

uint32_t const&
SgAsmCilAssemblyProcessor::get_Processor() const {
    return p_Processor;
}

void
SgAsmCilAssemblyProcessor::set_Processor(uint32_t const& x) {
    this->p_Processor = x;
    set_isModified(true);
}

SgAsmCilAssemblyProcessor::~SgAsmCilAssemblyProcessor() {
    destructorHelper();
}

SgAsmCilAssemblyProcessor::SgAsmCilAssemblyProcessor()
    : p_Processor(0) {}

void
SgAsmCilAssemblyProcessor::initializeProperties() {
    p_Processor = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
