#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmElfSymverNeededEntry::get_version() const {
    return p_version;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntry::set_version(size_t const& x) {
    this->p_version = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmElfSymverNeededEntry::get_file_name() const {
    return p_file_name;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntry::set_file_name(SgAsmGenericString* const& x) {
    this->p_file_name = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededAuxList* const&
SgAsmElfSymverNeededEntry::get_entries() const {
    return p_entries;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntry::set_entries(SgAsmElfSymverNeededAuxList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntry::~SgAsmElfSymverNeededEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntry::SgAsmElfSymverNeededEntry()
    : p_version(0)
    , p_file_name(nullptr)
    , p_entries(createAndParent<SgAsmElfSymverNeededAuxList>(this)) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntry::initializeProperties() {
    p_version = 0;
    p_file_name = nullptr;
    p_entries = createAndParent<SgAsmElfSymverNeededAuxList>(this);
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
