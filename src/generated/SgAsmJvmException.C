#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmException            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmException::get_start_pc() const {
    return p_start_pc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::set_start_pc(uint16_t const& x) {
    this->p_start_pc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmException::get_end_pc() const {
    return p_end_pc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::set_end_pc(uint16_t const& x) {
    this->p_end_pc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmException::get_handler_pc() const {
    return p_handler_pc;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::set_handler_pc(uint16_t const& x) {
    this->p_handler_pc = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmException::get_catch_type() const {
    return p_catch_type;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::set_catch_type(uint16_t const& x) {
    this->p_catch_type = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmException::~SgAsmJvmException() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmException::SgAsmJvmException()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_start_pc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_end_pc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_handler_pc(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_catch_type(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::initializeProperties() {
    p_start_pc = 0;
    p_end_pc = 0;
    p_handler_pc = 0;
    p_catch_type = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
