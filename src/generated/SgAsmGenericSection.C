//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

SgAsmGenericFile* const&
SgAsmGenericSection::get_file() const {
    return p_file;
}

void
SgAsmGenericSection::set_file(SgAsmGenericFile* const& x) {
    this->p_file = x;
    set_isModified(true);
}

SgAsmGenericHeader* const&
SgAsmGenericSection::get_header() const {
    return p_header;
}

void
SgAsmGenericSection::set_header(SgAsmGenericHeader* const& x) {
    this->p_header = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmGenericSection::get_file_alignment() const {
    return p_file_alignment;
}

void
SgAsmGenericSection::set_file_alignment(rose_addr_t const& x) {
    this->p_file_alignment = x;
    set_isModified(true);
}

SgFileContentList const&
SgAsmGenericSection::get_data() const {
    return p_data;
}

void
SgAsmGenericSection::set_data(SgFileContentList const& x) {
    this->p_data = x;
    set_isModified(true);
}

SgAsmGenericSection::SectionPurpose const&
SgAsmGenericSection::get_purpose() const {
    return p_purpose;
}

void
SgAsmGenericSection::set_purpose(SgAsmGenericSection::SectionPurpose const& x) {
    this->p_purpose = x;
    set_isModified(true);
}

bool const&
SgAsmGenericSection::get_synthesized() const {
    return p_synthesized;
}

void
SgAsmGenericSection::set_synthesized(bool const& x) {
    this->p_synthesized = x;
    set_isModified(true);
}

int const&
SgAsmGenericSection::get_id() const {
    return p_id;
}

void
SgAsmGenericSection::set_id(int const& x) {
    this->p_id = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmGenericSection::get_mapped_alignment() const {
    return p_mapped_alignment;
}

void
SgAsmGenericSection::set_mapped_alignment(rose_addr_t const& x) {
    this->p_mapped_alignment = x;
    set_isModified(true);
}

bool const&
SgAsmGenericSection::get_mapped_rperm() const {
    return p_mapped_rperm;
}

void
SgAsmGenericSection::set_mapped_rperm(bool const& x) {
    this->p_mapped_rperm = x;
    set_isModified(true);
}

bool const&
SgAsmGenericSection::get_mapped_wperm() const {
    return p_mapped_wperm;
}

void
SgAsmGenericSection::set_mapped_wperm(bool const& x) {
    this->p_mapped_wperm = x;
    set_isModified(true);
}

bool const&
SgAsmGenericSection::get_mapped_xperm() const {
    return p_mapped_xperm;
}

void
SgAsmGenericSection::set_mapped_xperm(bool const& x) {
    this->p_mapped_xperm = x;
    set_isModified(true);
}

bool const&
SgAsmGenericSection::get_contains_code() const {
    return p_contains_code;
}

void
SgAsmGenericSection::set_contains_code(bool const& x) {
    this->p_contains_code = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmGenericSection::get_mapped_actual_va() const {
    return p_mapped_actual_va;
}

void
SgAsmGenericSection::set_mapped_actual_va(rose_addr_t const& x) {
    this->p_mapped_actual_va = x;
    set_isModified(true);
}

SgAsmGenericSection::~SgAsmGenericSection() {
    destructorHelper();
}

SgAsmGenericSection::SgAsmGenericSection()
    : p_file(nullptr)
    , p_header(nullptr)
    , p_size(0)
    , p_offset(0)
    , p_file_alignment(0)
    , p_purpose(SgAsmGenericSection::SP_UNSPECIFIED)
    , p_synthesized(false)
    , p_id(-1)
    , p_name(createAndParent<SgAsmBasicString>(this))
    , p_mapped_preferred_rva(0)
    , p_mapped_size(0)
    , p_mapped_alignment(0)
    , p_mapped_rperm(false)
    , p_mapped_wperm(false)
    , p_mapped_xperm(false)
    , p_contains_code(false)
    , p_mapped_actual_va(0) {}

void
SgAsmGenericSection::initializeProperties() {
    p_file = nullptr;
    p_header = nullptr;
    p_size = 0;
    p_offset = 0;
    p_file_alignment = 0;
    p_purpose = SgAsmGenericSection::SP_UNSPECIFIED;
    p_synthesized = false;
    p_id = -1;
    p_name = createAndParent<SgAsmBasicString>(this);
    p_mapped_preferred_rva = 0;
    p_mapped_size = 0;
    p_mapped_alignment = 0;
    p_mapped_rperm = false;
    p_mapped_wperm = false;
    p_mapped_xperm = false;
    p_contains_code = false;
    p_mapped_actual_va = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
