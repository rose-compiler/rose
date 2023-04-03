#line 164 "src/Rosebud/RosettaGenerator.C"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfDynamicEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#line 167 "src/Rosebud/RosettaGenerator.C"
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#line 175 "src/Rosebud/RosettaGenerator.C"
#include <sage3basic.h>

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmElfDynamicEntry::EntryType const&
SgAsmElfDynamicEntry::get_d_tag() const {
    return p_d_tag;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfDynamicEntry::set_d_tag(SgAsmElfDynamicEntry::EntryType const& x) {
    this->p_d_tag = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
rose_rva_t const&
SgAsmElfDynamicEntry::get_d_val() const {
    return p_d_val;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
rose_rva_t&
SgAsmElfDynamicEntry::get_d_val() {
    return p_d_val;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfDynamicEntry::set_d_val(rose_rva_t const& x) {
    this->p_d_val = x;
    set_isModified(true);
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgAsmGenericString* const&
SgAsmElfDynamicEntry::get_name() const {
    return p_name;
}

#line 487 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList const&
SgAsmElfDynamicEntry::get_extra() const {
    return p_extra;
}

#line 493 "src/Rosebud/RosettaGenerator.C"
SgUnsignedCharList&
SgAsmElfDynamicEntry::get_extra() {
    return p_extra;
}

#line 507 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfDynamicEntry::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

#line 244 "src/Rosebud/RosettaGenerator.C"
SgAsmElfDynamicEntry::~SgAsmElfDynamicEntry() {
    destructorHelper();
}

#line 273 "src/Rosebud/RosettaGenerator.C"
SgAsmElfDynamicEntry::SgAsmElfDynamicEntry()
    : p_d_tag(SgAsmElfDynamicEntry::DT_NULL)
    , p_name(nullptr) {}

#line 389 "src/Rosebud/RosettaGenerator.C"
void
SgAsmElfDynamicEntry::initializeProperties() {
    p_d_tag = SgAsmElfDynamicEntry::DT_NULL;
    p_name = nullptr;
}

#line 183 "src/Rosebud/RosettaGenerator.C"
#endif // ROSE_ENABLE_BINARY_ANALYSIS
