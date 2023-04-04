#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericStrtab            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericSection* const&
SgAsmGenericStrtab::get_container() const {
    return p_container;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericStrtab::set_container(SgAsmGenericSection* const& x) {
    this->p_container = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericStrtab::referenced_t const&
SgAsmGenericStrtab::get_storage_list() const {
    return p_storage_list;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericStrtab::set_storage_list(SgAsmGenericStrtab::referenced_t const& x) {
    this->p_storage_list = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmStringStorage* const&
SgAsmGenericStrtab::get_dont_free() const {
    return p_dont_free;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericStrtab::set_dont_free(SgAsmStringStorage* const& x) {
    this->p_dont_free = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmGenericStrtab::get_num_freed() const {
    return p_num_freed;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericStrtab::set_num_freed(size_t const& x) {
    this->p_num_freed = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericStrtab::~SgAsmGenericStrtab() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericStrtab::SgAsmGenericStrtab()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_container(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_dont_free(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_num_freed(0) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmGenericStrtab::initializeProperties() {
    p_container = nullptr;
    p_dont_free = nullptr;
    p_num_freed = 0;
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
