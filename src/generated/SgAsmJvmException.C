//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmException            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>

uint16_t const&
SgAsmJvmException::get_start_pc() const {
    return p_start_pc;
}

void
SgAsmJvmException::set_start_pc(uint16_t const& x) {
    this->p_start_pc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmException::get_end_pc() const {
    return p_end_pc;
}

void
SgAsmJvmException::set_end_pc(uint16_t const& x) {
    this->p_end_pc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmException::get_handler_pc() const {
    return p_handler_pc;
}

void
SgAsmJvmException::set_handler_pc(uint16_t const& x) {
    this->p_handler_pc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmException::get_catch_type() const {
    return p_catch_type;
}

void
SgAsmJvmException::set_catch_type(uint16_t const& x) {
    this->p_catch_type = x;
    set_isModified(true);
}

SgAsmJvmException::~SgAsmJvmException() {
    destructorHelper();
}

SgAsmJvmException::SgAsmJvmException()
    : p_start_pc(0)
    , p_end_pc(0)
    , p_handler_pc(0)
    , p_catch_type(0) {}

void
SgAsmJvmException::initializeProperties() {
    p_start_pc = 0;
    p_end_pc = 0;
    p_handler_pc = 0;
    p_catch_type = 0;
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS
