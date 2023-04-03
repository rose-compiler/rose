#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInnerClassesEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmInnerClassesEntry::get_inner_class_info_index() const {
    return p_inner_class_info_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::set_inner_class_info_index(uint16_t const& x) {
    this->p_inner_class_info_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmInnerClassesEntry::get_outer_class_info_index() const {
    return p_outer_class_info_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::set_outer_class_info_index(uint16_t const& x) {
    this->p_outer_class_info_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmInnerClassesEntry::get_inner_name_index() const {
    return p_inner_name_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::set_inner_name_index(uint16_t const& x) {
    this->p_inner_name_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmInnerClassesEntry::get_inner_class_access_flags() const {
    return p_inner_class_access_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::set_inner_class_access_flags(uint16_t const& x) {
    this->p_inner_class_access_flags = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInnerClassesEntry::~SgAsmJvmInnerClassesEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInnerClassesEntry::SgAsmJvmInnerClassesEntry()
    : p_inner_class_info_index(0)
    , p_outer_class_info_index(0)
    , p_inner_name_index(0)
    , p_inner_class_access_flags(0) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::initializeProperties() {
    p_inner_class_info_index = 0;
    p_outer_class_info_index = 0;
    p_inner_name_index = 0;
    p_inner_class_access_flags = 0;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
