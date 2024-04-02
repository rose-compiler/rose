//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmExceptionHandler            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmExceptionHandler_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmExceptionHandler::get_start_pc() const {
    return p_start_pc;
}

void
SgAsmJvmExceptionHandler::set_start_pc(uint16_t const& x) {
    this->p_start_pc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmExceptionHandler::get_end_pc() const {
    return p_end_pc;
}

void
SgAsmJvmExceptionHandler::set_end_pc(uint16_t const& x) {
    this->p_end_pc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmExceptionHandler::get_handler_pc() const {
    return p_handler_pc;
}

void
SgAsmJvmExceptionHandler::set_handler_pc(uint16_t const& x) {
    this->p_handler_pc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmExceptionHandler::get_catch_type() const {
    return p_catch_type;
}

void
SgAsmJvmExceptionHandler::set_catch_type(uint16_t const& x) {
    this->p_catch_type = x;
    set_isModified(true);
}

SgAsmJvmExceptionHandler::~SgAsmJvmExceptionHandler() {
    destructorHelper();
}

SgAsmJvmExceptionHandler::SgAsmJvmExceptionHandler()
    : p_start_pc(0)
    , p_end_pc(0)
    , p_handler_pc(0)
    , p_catch_type(0) {}

void
SgAsmJvmExceptionHandler::initializeProperties() {
    p_start_pc = 0;
    p_end_pc = 0;
    p_handler_pc = 0;
    p_catch_type = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
