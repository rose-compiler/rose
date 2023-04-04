#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmJvmInnerClassesEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmInnerClassesEntry::get_inner_class_info_index() const {
    return p_inner_class_info_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::set_inner_class_info_index(uint16_t const& x) {
    this->p_inner_class_info_index = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmInnerClassesEntry::get_outer_class_info_index() const {
    return p_outer_class_info_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::set_outer_class_info_index(uint16_t const& x) {
    this->p_outer_class_info_index = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmInnerClassesEntry::get_inner_name_index() const {
    return p_inner_name_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::set_inner_name_index(uint16_t const& x) {
    this->p_inner_name_index = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint16_t const&
SgAsmJvmInnerClassesEntry::get_inner_class_access_flags() const {
    return p_inner_class_access_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::set_inner_class_access_flags(uint16_t const& x) {
    this->p_inner_class_access_flags = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInnerClassesEntry::~SgAsmJvmInnerClassesEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmJvmInnerClassesEntry::SgAsmJvmInnerClassesEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_inner_class_info_index(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_outer_class_info_index(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_inner_name_index(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_inner_class_access_flags(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmJvmInnerClassesEntry::initializeProperties() {
    p_inner_class_info_index = 0;
    p_outer_class_info_index = 0;
    p_inner_name_index = 0;
    p_inner_class_access_flags = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
