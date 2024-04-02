//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmMethod            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmMethod_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmMethod::get_access_flags() const {
    return p_access_flags;
}

void
SgAsmJvmMethod::set_access_flags(uint16_t const& x) {
    this->p_access_flags = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmMethod::get_name_index() const {
    return p_name_index;
}

void
SgAsmJvmMethod::set_name_index(uint16_t const& x) {
    this->p_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmMethod::get_descriptor_index() const {
    return p_descriptor_index;
}

void
SgAsmJvmMethod::set_descriptor_index(uint16_t const& x) {
    this->p_descriptor_index = x;
    set_isModified(true);
}

SgAsmJvmAttributeTable* const&
SgAsmJvmMethod::get_attribute_table() const {
    return p_attribute_table;
}

void
SgAsmJvmMethod::set_attribute_table(SgAsmJvmAttributeTable* const& x) {
    changeChildPointer(this->p_attribute_table, const_cast<SgAsmJvmAttributeTable*&>(x));
    set_isModified(true);
}

SgAsmInstructionList* const&
SgAsmJvmMethod::get_instruction_list() const {
    return p_instruction_list;
}

void
SgAsmJvmMethod::set_instruction_list(SgAsmInstructionList* const& x) {
    this->p_instruction_list = x;
    set_isModified(true);
}

SgAsmJvmMethod::~SgAsmJvmMethod() {
    destructorHelper();
}

SgAsmJvmMethod::SgAsmJvmMethod()
    : p_access_flags(0)
    , p_name_index(0)
    , p_descriptor_index(0)
    , p_attribute_table(nullptr)
    , p_instruction_list(createAndParent<SgAsmInstructionList>(this)) {}

void
SgAsmJvmMethod::initializeProperties() {
    p_access_flags = 0;
    p_name_index = 0;
    p_descriptor_index = 0;
    p_attribute_table = nullptr;
    p_instruction_list = createAndParent<SgAsmInstructionList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
