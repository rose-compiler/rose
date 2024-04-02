//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInnerClassesEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmJvmInnerClassesEntry_IMPL
#include <sage3basic.h>

uint16_t const&
SgAsmJvmInnerClassesEntry::get_inner_class_info_index() const {
    return p_inner_class_info_index;
}

void
SgAsmJvmInnerClassesEntry::set_inner_class_info_index(uint16_t const& x) {
    this->p_inner_class_info_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmInnerClassesEntry::get_outer_class_info_index() const {
    return p_outer_class_info_index;
}

void
SgAsmJvmInnerClassesEntry::set_outer_class_info_index(uint16_t const& x) {
    this->p_outer_class_info_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmInnerClassesEntry::get_inner_name_index() const {
    return p_inner_name_index;
}

void
SgAsmJvmInnerClassesEntry::set_inner_name_index(uint16_t const& x) {
    this->p_inner_name_index = x;
    set_isModified(true);
}

uint16_t const&
SgAsmJvmInnerClassesEntry::get_inner_class_access_flags() const {
    return p_inner_class_access_flags;
}

void
SgAsmJvmInnerClassesEntry::set_inner_class_access_flags(uint16_t const& x) {
    this->p_inner_class_access_flags = x;
    set_isModified(true);
}

SgAsmJvmInnerClassesEntry::~SgAsmJvmInnerClassesEntry() {
    destructorHelper();
}

SgAsmJvmInnerClassesEntry::SgAsmJvmInnerClassesEntry()
    : p_inner_class_info_index(0)
    , p_outer_class_info_index(0)
    , p_inner_name_index(0)
    , p_inner_class_access_flags(0) {}

void
SgAsmJvmInnerClassesEntry::initializeProperties() {
    p_inner_class_info_index = 0;
    p_outer_class_info_index = 0;
    p_inner_name_index = 0;
    p_inner_class_access_flags = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
