#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverDefinedEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmElfSymverDefinedEntry::get_version() const {
    return p_version;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_version(size_t const& x) {
    this->p_version = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
int const&
SgAsmElfSymverDefinedEntry::get_flags() const {
    return p_flags;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_flags(int const& x) {
    this->p_flags = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmElfSymverDefinedEntry::get_index() const {
    return p_index;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_index(size_t const& x) {
    this->p_index = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
uint32_t const&
SgAsmElfSymverDefinedEntry::get_hash() const {
    return p_hash;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_hash(uint32_t const& x) {
    this->p_hash = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedAuxList* const&
SgAsmElfSymverDefinedEntry::get_entries() const {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::set_entries(SgAsmElfSymverDefinedAuxList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedEntry::~SgAsmElfSymverDefinedEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverDefinedEntry::SgAsmElfSymverDefinedEntry()
    : p_version(0)
    , p_flags(0)
    , p_index(0)
    , p_hash(0)
    , p_entries(createAndParent<SgAsmElfSymverDefinedAuxList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverDefinedEntry::initializeProperties() {
    p_version = 0;
    p_flags = 0;
    p_index = 0;
    p_hash = 0;
    p_entries = createAndParent<SgAsmElfSymverDefinedAuxList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
