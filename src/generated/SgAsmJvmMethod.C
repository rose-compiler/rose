#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmMethod            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmMethod::get_access_flags() const {
    return p_access_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmMethod::set_access_flags(uint16_t const& x) {
    this->p_access_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmMethod::get_name_index() const {
    return p_name_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmMethod::set_name_index(uint16_t const& x) {
    this->p_name_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmMethod::get_descriptor_index() const {
    return p_descriptor_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmMethod::set_descriptor_index(uint16_t const& x) {
    this->p_descriptor_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmAttributeTable* const&
SgAsmJvmMethod::get_attribute_table() const {
    return p_attribute_table;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmMethod::set_attribute_table(SgAsmJvmAttributeTable* const& x) {
    this->p_attribute_table = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmInstructionList* const&
SgAsmJvmMethod::get_instruction_list() const {
    return p_instruction_list;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmMethod::set_instruction_list(SgAsmInstructionList* const& x) {
    this->p_instruction_list = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmMethod::~SgAsmJvmMethod() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmMethod::SgAsmJvmMethod()
    : p_access_flags(0)
    , p_name_index(0)
    , p_descriptor_index(0)
    , p_attribute_table(nullptr)
    , p_instruction_list(createAndParent<SgAsmInstructionList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmMethod::initializeProperties() {
    p_access_flags = 0;
    p_name_index = 0;
    p_descriptor_index = 0;
    p_attribute_table = nullptr;
    p_instruction_list = createAndParent<SgAsmInstructionList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
