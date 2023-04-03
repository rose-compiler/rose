#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmCodeAttribute            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmCodeAttribute::get_max_stack() const {
    return p_max_stack;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmCodeAttribute::set_max_stack(uint16_t const& x) {
    this->p_max_stack = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmCodeAttribute::get_max_locals() const {
    return p_max_locals;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmCodeAttribute::set_max_locals(uint16_t const& x) {
    this->p_max_locals = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmJvmCodeAttribute::get_code_length() const {
    return p_code_length;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmCodeAttribute::set_code_length(uint32_t const& x) {
    this->p_code_length = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
const char* const&
SgAsmJvmCodeAttribute::get_code() const {
    return p_code;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmCodeAttribute::set_code(const char* const& x) {
    this->p_code = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmExceptionTable* const&
SgAsmJvmCodeAttribute::get_exception_table() const {
    return p_exception_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmCodeAttribute::set_exception_table(SgAsmJvmExceptionTable* const& x) {
    this->p_exception_table = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttributeTable* const&
SgAsmJvmCodeAttribute::get_attribute_table() const {
    return p_attribute_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmCodeAttribute::set_attribute_table(SgAsmJvmAttributeTable* const& x) {
    this->p_attribute_table = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_addr_t const&
SgAsmJvmCodeAttribute::get_code_offset() const {
    return p_code_offset;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmCodeAttribute::set_code_offset(rose_addr_t const& x) {
    this->p_code_offset = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmCodeAttribute::~SgAsmJvmCodeAttribute() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmCodeAttribute::SgAsmJvmCodeAttribute()
    : p_max_stack(0)
    , p_max_locals(0)
    , p_code_length(0)
    , p_code(nullptr)
    , p_exception_table(nullptr)
    , p_attribute_table(nullptr)
    , p_code_offset(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmCodeAttribute::initializeProperties() {
    p_max_stack = 0;
    p_max_locals = 0;
    p_code_length = 0;
    p_code = nullptr;
    p_exception_table = nullptr;
    p_attribute_table = nullptr;
    p_code_offset = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
