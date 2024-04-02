//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmElfFileHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmElfFileHeader_IMPL
#include <sage3basic.h>

unsigned char const&
SgAsmElfFileHeader::get_e_ident_file_class() const {
    return p_e_ident_file_class;
}

void
SgAsmElfFileHeader::set_e_ident_file_class(unsigned char const& x) {
    this->p_e_ident_file_class = x;
    set_isModified(true);
}

unsigned char const&
SgAsmElfFileHeader::get_e_ident_data_encoding() const {
    return p_e_ident_data_encoding;
}

void
SgAsmElfFileHeader::set_e_ident_data_encoding(unsigned char const& x) {
    this->p_e_ident_data_encoding = x;
    set_isModified(true);
}

unsigned char const&
SgAsmElfFileHeader::get_e_ident_file_version() const {
    return p_e_ident_file_version;
}

void
SgAsmElfFileHeader::set_e_ident_file_version(unsigned char const& x) {
    this->p_e_ident_file_version = x;
    set_isModified(true);
}

SgUnsignedCharList const&
SgAsmElfFileHeader::get_e_ident_padding() const {
    return p_e_ident_padding;
}

void
SgAsmElfFileHeader::set_e_ident_padding(SgUnsignedCharList const& x) {
    this->p_e_ident_padding = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfFileHeader::get_e_type() const {
    return p_e_type;
}

void
SgAsmElfFileHeader::set_e_type(unsigned long const& x) {
    this->p_e_type = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfFileHeader::get_e_machine() const {
    return p_e_machine;
}

void
SgAsmElfFileHeader::set_e_machine(unsigned long const& x) {
    this->p_e_machine = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfFileHeader::get_e_flags() const {
    return p_e_flags;
}

void
SgAsmElfFileHeader::set_e_flags(unsigned long const& x) {
    this->p_e_flags = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfFileHeader::get_e_ehsize() const {
    return p_e_ehsize;
}

void
SgAsmElfFileHeader::set_e_ehsize(unsigned long const& x) {
    this->p_e_ehsize = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfFileHeader::get_phextrasz() const {
    return p_phextrasz;
}

void
SgAsmElfFileHeader::set_phextrasz(unsigned long const& x) {
    this->p_phextrasz = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfFileHeader::get_e_phnum() const {
    return p_e_phnum;
}

void
SgAsmElfFileHeader::set_e_phnum(unsigned long const& x) {
    this->p_e_phnum = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfFileHeader::get_shextrasz() const {
    return p_shextrasz;
}

void
SgAsmElfFileHeader::set_shextrasz(unsigned long const& x) {
    this->p_shextrasz = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfFileHeader::get_e_shnum() const {
    return p_e_shnum;
}

void
SgAsmElfFileHeader::set_e_shnum(unsigned long const& x) {
    this->p_e_shnum = x;
    set_isModified(true);
}

unsigned long const&
SgAsmElfFileHeader::get_e_shstrndx() const {
    return p_e_shstrndx;
}

void
SgAsmElfFileHeader::set_e_shstrndx(unsigned long const& x) {
    this->p_e_shstrndx = x;
    set_isModified(true);
}

SgAsmElfSectionTable* const&
SgAsmElfFileHeader::get_sectionTable() const {
    return p_sectionTable;
}

void
SgAsmElfFileHeader::set_sectionTable(SgAsmElfSectionTable* const& x) {
    this->p_sectionTable = x;
    set_isModified(true);
}

SgAsmElfSegmentTable* const&
SgAsmElfFileHeader::get_segmentTable() const {
    return p_segmentTable;
}

void
SgAsmElfFileHeader::set_segmentTable(SgAsmElfSegmentTable* const& x) {
    this->p_segmentTable = x;
    set_isModified(true);
}

SgAsmElfFileHeader::~SgAsmElfFileHeader() {
    destructorHelper();
}

SgAsmElfFileHeader::SgAsmElfFileHeader()
    : p_e_ident_file_class(0)
    , p_e_ident_data_encoding(0)
    , p_e_ident_file_version(0)
    , p_e_type(0)
    , p_e_machine(0)
    , p_e_flags(0)
    , p_e_ehsize(0)
    , p_phextrasz(0)
    , p_e_phnum(0)
    , p_shextrasz(0)
    , p_e_shnum(0)
    , p_e_shstrndx(0)
    , p_sectionTable(nullptr)
    , p_segmentTable(nullptr) {}

void
SgAsmElfFileHeader::initializeProperties() {
    p_e_ident_file_class = 0;
    p_e_ident_data_encoding = 0;
    p_e_ident_file_version = 0;
    p_e_type = 0;
    p_e_machine = 0;
    p_e_flags = 0;
    p_e_ehsize = 0;
    p_phextrasz = 0;
    p_e_phnum = 0;
    p_shextrasz = 0;
    p_e_shnum = 0;
    p_e_shstrndx = 0;
    p_sectionTable = nullptr;
    p_segmentTable = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
