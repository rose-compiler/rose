#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmException            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmException::get_start_pc() const {
    return p_start_pc;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::set_start_pc(uint16_t const& x) {
    this->p_start_pc = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmException::get_end_pc() const {
    return p_end_pc;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::set_end_pc(uint16_t const& x) {
    this->p_end_pc = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmException::get_handler_pc() const {
    return p_handler_pc;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::set_handler_pc(uint16_t const& x) {
    this->p_handler_pc = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmException::get_catch_type() const {
    return p_catch_type;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::set_catch_type(uint16_t const& x) {
    this->p_catch_type = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmException::~SgAsmJvmException() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmException::SgAsmJvmException()
    : p_start_pc(0)
    , p_end_pc(0)
    , p_handler_pc(0)
    , p_catch_type(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmException::initializeProperties() {
    p_start_pc = 0;
    p_end_pc = 0;
    p_handler_pc = 0;
    p_catch_type = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
