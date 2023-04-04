#line 192 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfSymverNeededEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 195 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#line 204 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 546 "src/Rosebud/RosettaGenerator.C"
size_t const&
SgAsmElfSymverNeededEntry::get_version() const {
    return p_version;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntry::set_version(size_t const& x) {
    this->p_version = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmElfSymverNeededEntry::get_file_name() const {
    return p_file_name;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntry::set_file_name(SgAsmGenericString* const& x) {
    this->p_file_name = x;
    set_isModified(true);
}

#line 546 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededAuxList* const&
SgAsmElfSymverNeededEntry::get_entries() const {
    return p_entries;
}

#line 566 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntry::set_entries(SgAsmElfSymverNeededAuxList* const& x) {
    this->p_entries = x;
    set_isModified(true);
}

#line 274 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntry::~SgAsmElfSymverNeededEntry() {
    destructorHelper();
}

#line 303 "src/Rosebud/RosettaGenerator.C"
SgAsmElfSymverNeededEntry::SgAsmElfSymverNeededEntry()
#line 308 "src/Rosebud/RosettaGenerator.C"
    : p_version(0)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_file_name(nullptr)
#line 308 "src/Rosebud/RosettaGenerator.C"
    , p_entries(createAndParent<SgAsmElfSymverNeededAuxList>(this)) {}

#line 448 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfSymverNeededEntry::initializeProperties() {
    p_version = 0;
    p_file_name = nullptr;
    p_entries = createAndParent<SgAsmElfSymverNeededAuxList>(this);
}

#line 213 "src/Rosebud/RosettaGenerator.C"
#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
