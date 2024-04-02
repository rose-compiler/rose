//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmGenericHeader_IMPL
#include <SgAsmGenericHeader.h>

SgAsmGenericFormat* const&
SgAsmGenericHeader::get_executableFormat() const {
    return p_executableFormat;
}

void
SgAsmGenericHeader::set_executableFormat(SgAsmGenericFormat* const& x) {
    changeChildPointer(this->p_executableFormat, const_cast<SgAsmGenericFormat*&>(x));
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
SgAsmGenericHeader::get_baseVa() const {
    return p_baseVa;
}

void
SgAsmGenericHeader::set_baseVa(rose_addr_t const& x) {
    this->p_baseVa = x;
    set_isModified(true);
}

SgRVAList const&
SgAsmGenericHeader::get_entryRvas() const {
    return p_entryRvas;
}

SgRVAList&
SgAsmGenericHeader::get_entryRvas() {
    return p_entryRvas;
}

void
SgAsmGenericHeader::set_entryRvas(SgRVAList const& x) {
    this->p_entryRvas = x;
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
    : p_executableFormat(createAndParent<SgAsmGenericFormat>(this))
    , p_isa(SgAsmGenericFormat::ISA_UNSPECIFIED)
    , p_baseVa(0)
    , p_dlls(createAndParent<SgAsmGenericDLLList>(this))
    , p_sections(createAndParent<SgAsmGenericSectionList>(this)) {}

void
SgAsmGenericHeader::initializeProperties() {
    p_executableFormat = createAndParent<SgAsmGenericFormat>(this);
    p_isa = SgAsmGenericFormat::ISA_UNSPECIFIED;
    p_baseVa = 0;
    p_dlls = createAndParent<SgAsmGenericDLLList>(this);
    p_sections = createAndParent<SgAsmGenericSectionList>(this);
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
