//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmCilAssemblyRefProcessor            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

uint32_t const&
SgAsmCilAssemblyRefProcessor::get_Processor() const {
    return p_Processor;
}

void
SgAsmCilAssemblyRefProcessor::set_Processor(uint32_t const& x) {
    this->p_Processor = x;
    set_isModified(true);
}

uint32_t const&
SgAsmCilAssemblyRefProcessor::get_AssemblyRef() const {
    return p_AssemblyRef;
}

void
SgAsmCilAssemblyRefProcessor::set_AssemblyRef(uint32_t const& x) {
    this->p_AssemblyRef = x;
    set_isModified(true);
}

SgAsmCilAssemblyRefProcessor::~SgAsmCilAssemblyRefProcessor() {
    destructorHelper();
}

SgAsmCilAssemblyRefProcessor::SgAsmCilAssemblyRefProcessor() {}

void
SgAsmCilAssemblyRefProcessor::initializeProperties() {
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
