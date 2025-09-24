//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmRuntimeAnnotationValue            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmRuntimeAnnotationValue_IMPL
#include <SgAsmJvmRuntimeAnnotationValue.h>

uint8_t const&
SgAsmJvmRuntimeAnnotationValue::get_tag() const {
    return p_tag;
}

void
SgAsmJvmRuntimeAnnotationValue::set_tag(uint8_t const& x) {
    this->p_tag = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmRuntimeAnnotationValue::get_const_value_index() const {
    return p_const_value_index;
}

void
SgAsmJvmRuntimeAnnotationValue::set_const_value_index(uint16_t const& x) {
    this->p_const_value_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmRuntimeAnnotationValue::get_type_name_index() const {
    return p_type_name_index;
}

void
SgAsmJvmRuntimeAnnotationValue::set_type_name_index(uint16_t const& x) {
    this->p_type_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmRuntimeAnnotationValue::get_const_name_index() const {
    return p_const_name_index;
}

void
SgAsmJvmRuntimeAnnotationValue::set_const_name_index(uint16_t const& x) {
    this->p_const_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmRuntimeAnnotationValue::get_class_info_index() const {
    return p_class_info_index;
}

void
SgAsmJvmRuntimeAnnotationValue::set_class_info_index(uint16_t const& x) {
    this->p_class_info_index = x;
    set_isModified(true);
}

bool const&
SgAsmJvmRuntimeAnnotationValue::get_is_annotation_value() const {
    return p_is_annotation_value;
}

void
SgAsmJvmRuntimeAnnotationValue::set_is_annotation_value(bool const& x) {
    this->p_is_annotation_value = x;
    set_isModified(true);
}

SgAsmJvmRuntimeAnnotation* const&
SgAsmJvmRuntimeAnnotationValue::get_annotation_value() const {
    return p_annotation_value;
}

void
SgAsmJvmRuntimeAnnotationValue::set_annotation_value(SgAsmJvmRuntimeAnnotation* const& x) {
    this->p_annotation_value = x;
    set_isModified(true);
}

std::vector<SgAsmJvmRuntimeAnnotationValue*> const&
SgAsmJvmRuntimeAnnotationValue::get_values() const {
    return p_values;
}

std::vector<SgAsmJvmRuntimeAnnotationValue*>&
SgAsmJvmRuntimeAnnotationValue::get_values() {
    return p_values;
}

SgAsmJvmRuntimeAnnotationValue::~SgAsmJvmRuntimeAnnotationValue() {
    destructorHelper();
}

SgAsmJvmRuntimeAnnotationValue::SgAsmJvmRuntimeAnnotationValue()
    : p_tag(0)
    , p_const_value_index(0)
    , p_type_name_index(0)
    , p_const_name_index(0)
    , p_class_info_index(0)
    , p_is_annotation_value(false)
    , p_annotation_value(nullptr) {}

void
SgAsmJvmRuntimeAnnotationValue::initializeProperties() {
    p_tag = 0;
    p_const_value_index = 0;
    p_type_name_index = 0;
    p_const_name_index = 0;
    p_class_info_index = 0;
    p_is_annotation_value = false;
    p_annotation_value = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
