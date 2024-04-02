//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmGenericSection            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmGenericSection_IMPL
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
SgAsmGenericSection::get_fileAlignment() const {
    return p_fileAlignment;
}

void
SgAsmGenericSection::set_fileAlignment(rose_addr_t const& x) {
    this->p_fileAlignment = x;
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

SgAsmGenericString* const&
SgAsmGenericSection::get_name() const {
    return p_name;
}

void
SgAsmGenericSection::set_shortName(std::string const& x) {
    this->p_shortName = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmGenericSection::get_mappedPreferredRva() const {
    return p_mappedPreferredRva;
}

rose_addr_t const&
SgAsmGenericSection::get_mappedSize() const {
    return p_mappedSize;
}

rose_addr_t const&
SgAsmGenericSection::get_mappedAlignment() const {
    return p_mappedAlignment;
}

void
SgAsmGenericSection::set_mappedAlignment(rose_addr_t const& x) {
    this->p_mappedAlignment = x;
    set_isModified(true);
}

bool const&
SgAsmGenericSection::get_mappedReadPermission() const {
    return p_mappedReadPermission;
}

void
SgAsmGenericSection::set_mappedReadPermission(bool const& x) {
    this->p_mappedReadPermission = x;
    set_isModified(true);
}

bool const&
SgAsmGenericSection::get_mappedWritePermission() const {
    return p_mappedWritePermission;
}

void
SgAsmGenericSection::set_mappedWritePermission(bool const& x) {
    this->p_mappedWritePermission = x;
    set_isModified(true);
}

bool const&
SgAsmGenericSection::get_mappedExecutePermission() const {
    return p_mappedExecutePermission;
}

void
SgAsmGenericSection::set_mappedExecutePermission(bool const& x) {
    this->p_mappedExecutePermission = x;
    set_isModified(true);
}

bool const&
SgAsmGenericSection::get_containsCode() const {
    return p_containsCode;
}

void
SgAsmGenericSection::set_containsCode(bool const& x) {
    this->p_containsCode = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmGenericSection::get_mappedActualVa() const {
    return p_mappedActualVa;
}

void
SgAsmGenericSection::set_mappedActualVa(rose_addr_t const& x) {
    this->p_mappedActualVa = x;
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
    , p_fileAlignment(0)
    , p_purpose(SgAsmGenericSection::SP_UNSPECIFIED)
    , p_synthesized(false)
    , p_id(-1)
    , p_name(createAndParent<SgAsmBasicString>(this))
    , p_mappedPreferredRva(0)
    , p_mappedSize(0)
    , p_mappedAlignment(0)
    , p_mappedReadPermission(false)
    , p_mappedWritePermission(false)
    , p_mappedExecutePermission(false)
    , p_containsCode(false)
    , p_mappedActualVa(0) {}

void
SgAsmGenericSection::initializeProperties() {
    p_file = nullptr;
    p_header = nullptr;
    p_size = 0;
    p_offset = 0;
    p_fileAlignment = 0;
    p_purpose = SgAsmGenericSection::SP_UNSPECIFIED;
    p_synthesized = false;
    p_id = -1;
    p_name = createAndParent<SgAsmBasicString>(this);
    p_mappedPreferredRva = 0;
    p_mappedSize = 0;
    p_mappedAlignment = 0;
    p_mappedReadPermission = false;
    p_mappedWritePermission = false;
    p_mappedExecutePermission = false;
    p_containsCode = false;
    p_mappedActualVa = 0;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
