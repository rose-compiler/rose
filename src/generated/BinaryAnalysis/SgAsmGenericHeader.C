//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmGenericFormat* const&
SgAsmGenericHeader::get_exec_format() const {
    return p_exec_format;
}

void
SgAsmGenericHeader::set_exec_format(SgAsmGenericFormat* const& x) {
    changeChildPointer(this->p_exec_format, const_cast<SgAsmGenericFormat*&>(x));
    set_isModified(true);
}

SgCharList const&
SgAsmGenericHeader::get_magic() const {
    return p_magic;
}

SgCharList&
SgAsmGenericHeader::get_magic() {
    return p_magic;
}

void
SgAsmGenericHeader::set_magic(SgCharList const& x) {
    this->p_magic = x;
    set_isModified(true);
}

SgAsmGenericFormat::InsSetArchitecture const&
SgAsmGenericHeader::get_isa() const {
    return p_isa;
}

void
SgAsmGenericHeader::set_isa(SgAsmGenericFormat::InsSetArchitecture const& x) {
    this->p_isa = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmGenericHeader::get_base_va() const {
    return p_base_va;
}

void
SgAsmGenericHeader::set_base_va(rose_addr_t const& x) {
    this->p_base_va = x;
    set_isModified(true);
}

SgRVAList const&
SgAsmGenericHeader::get_entry_rvas() const {
    return p_entry_rvas;
}

SgRVAList&
SgAsmGenericHeader::get_entry_rvas() {
    return p_entry_rvas;
}

void
SgAsmGenericHeader::set_entry_rvas(SgRVAList const& x) {
    this->p_entry_rvas = x;
    set_isModified(true);
}

SgAsmGenericDLLList* const&
SgAsmGenericHeader::get_dlls() const {
    return p_dlls;
}

void
SgAsmGenericHeader::set_dlls(SgAsmGenericDLLList* const& x) {
    changeChildPointer(this->p_dlls, const_cast<SgAsmGenericDLLList*&>(x));
    set_isModified(true);
}

SgAsmGenericSectionList* const&
SgAsmGenericHeader::get_sections() const {
    return p_sections;
}

void
SgAsmGenericHeader::set_sections(SgAsmGenericSectionList* const& x) {
    changeChildPointer(this->p_sections, const_cast<SgAsmGenericSectionList*&>(x));
    set_isModified(true);
}

SgAsmGenericHeader::~SgAsmGenericHeader() {
    destructorHelper();
}

SgAsmGenericHeader::SgAsmGenericHeader()
    : p_exec_format(createAndParent<SgAsmGenericFormat>(this))
    , p_isa(SgAsmGenericFormat::ISA_UNSPECIFIED)
    , p_base_va(0)
    , p_dlls(createAndParent<SgAsmGenericDLLList>(this))
    , p_sections(createAndParent<SgAsmGenericSectionList>(this)) {}

void
SgAsmGenericHeader::initializeProperties() {
    p_exec_format = createAndParent<SgAsmGenericFormat>(this);
    p_isa = SgAsmGenericFormat::ISA_UNSPECIFIED;
    p_base_va = 0;
    p_dlls = createAndParent<SgAsmGenericDLLList>(this);
    p_sections = createAndParent<SgAsmGenericSectionList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
