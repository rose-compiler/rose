//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfDynamicEntry            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfDynamicEntry_IMPL
#include <sage3basic.h>

SgAsmElfDynamicEntry::EntryType const&
SgAsmElfDynamicEntry::get_d_tag() const {
    return p_d_tag;
}

void
SgAsmElfDynamicEntry::set_d_tag(SgAsmElfDynamicEntry::EntryType const& x) {
    this->p_d_tag = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmElfDynamicEntry::get_d_val() const {
    return p_d_val;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmElfDynamicEntry::get_d_val() {
    return p_d_val;
}

void
SgAsmElfDynamicEntry::set_d_val(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_d_val = x;
    set_isModified(true);
}

SgAsmGenericString* const&
SgAsmElfDynamicEntry::get_name() const {
    return p_name;
}

SgUnsignedCharList const&
SgAsmElfDynamicEntry::get_extra() const {
    return p_extra;
}

SgUnsignedCharList&
SgAsmElfDynamicEntry::get_extra() {
    return p_extra;
}

void
SgAsmElfDynamicEntry::set_extra(SgUnsignedCharList const& x) {
    this->p_extra = x;
    set_isModified(true);
}

SgAsmElfDynamicEntry::~SgAsmElfDynamicEntry() {
    destructorHelper();
}

SgAsmElfDynamicEntry::SgAsmElfDynamicEntry()
    : p_d_tag(SgAsmElfDynamicEntry::DT_NULL)
    , p_name(nullptr) {}

void
SgAsmElfDynamicEntry::initializeProperties() {
    p_d_tag = SgAsmElfDynamicEntry::DT_NULL;
    p_name = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
