#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmElfSymverDefinedEntry::get_version() const {
    return p_version;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_version(size_t const& x) {
    this->p_version = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmElfSymverDefinedEntry::get_flags() const {
    return p_flags;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_flags(int const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmElfSymverDefinedEntry::get_index() const {
    return p_index;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_index(size_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmElfSymverDefinedEntry::get_hash() const {
    return p_hash;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_hash(uint32_t const& x) {
    this->p_hash = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedAuxList* const&
SgAsmElfSymverDefinedEntry::get_entries() const {
    return p_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_entries(SgAsmElfSymverDefinedAuxList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedEntry::~SgAsmElfSymverDefinedEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedEntry::SgAsmElfSymverDefinedEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_version(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_flags(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_index(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_hash(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_entries(createAndParent<SgAsmElfSymverDefinedAuxList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::initializeProperties() {
    p_version = 0;
    p_flags = 0;
    p_index = 0;
    p_hash = 0;
    p_entries = createAndParent<SgAsmElfSymverDefinedAuxList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
