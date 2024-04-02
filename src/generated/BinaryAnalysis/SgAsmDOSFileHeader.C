//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmDOSFileHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmDOSFileHeader_IMPL
#include <SgAsmDOSFileHeader.h>

uint16_t const&
SgAsmDOSFileHeader::get_e_last_page_size() const {
    return p_e_last_page_size;
}

void
SgAsmDOSFileHeader::set_e_last_page_size(uint16_t const& x) {
    this->p_e_last_page_size = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_total_pages() const {
    return p_e_total_pages;
}

void
SgAsmDOSFileHeader::set_e_total_pages(uint16_t const& x) {
    this->p_e_total_pages = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_nrelocs() const {
    return p_e_nrelocs;
}

void
SgAsmDOSFileHeader::set_e_nrelocs(uint16_t const& x) {
    this->p_e_nrelocs = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_header_paragraphs() const {
    return p_e_header_paragraphs;
}

void
SgAsmDOSFileHeader::set_e_header_paragraphs(uint16_t const& x) {
    this->p_e_header_paragraphs = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_minalloc() const {
    return p_e_minalloc;
}

void
SgAsmDOSFileHeader::set_e_minalloc(uint16_t const& x) {
    this->p_e_minalloc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_maxalloc() const {
    return p_e_maxalloc;
}

void
SgAsmDOSFileHeader::set_e_maxalloc(uint16_t const& x) {
    this->p_e_maxalloc = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_ss() const {
    return p_e_ss;
}

void
SgAsmDOSFileHeader::set_e_ss(uint16_t const& x) {
    this->p_e_ss = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_sp() const {
    return p_e_sp;
}

void
SgAsmDOSFileHeader::set_e_sp(uint16_t const& x) {
    this->p_e_sp = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_cksum() const {
    return p_e_cksum;
}

void
SgAsmDOSFileHeader::set_e_cksum(uint16_t const& x) {
    this->p_e_cksum = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_ip() const {
    return p_e_ip;
}

void
SgAsmDOSFileHeader::set_e_ip(uint16_t const& x) {
    this->p_e_ip = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_cs() const {
    return p_e_cs;
}

void
SgAsmDOSFileHeader::set_e_cs(uint16_t const& x) {
    this->p_e_cs = x;
    set_isModified(true);
}

uint16_t const&
SgAsmDOSFileHeader::get_e_overlay() const {
    return p_e_overlay;
}

void
SgAsmDOSFileHeader::set_e_overlay(uint16_t const& x) {
    this->p_e_overlay = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmDOSFileHeader::get_e_relocs_offset() const {
    return p_e_relocs_offset;
}

void
SgAsmDOSFileHeader::set_e_relocs_offset(rose_addr_t const& x) {
    this->p_e_relocs_offset = x;
    set_isModified(true);
}

unsigned const&
SgAsmDOSFileHeader::get_e_res1() const {
    return p_e_res1;
}

void
SgAsmDOSFileHeader::set_e_res1(unsigned const& x) {
    this->p_e_res1 = x;
    set_isModified(true);
}

SgAsmGenericSection* const&
SgAsmDOSFileHeader::get_relocs() const {
    return p_relocs;
}

void
SgAsmDOSFileHeader::set_relocs(SgAsmGenericSection* const& x) {
    this->p_relocs = x;
    set_isModified(true);
}

SgAsmGenericSection* const&
SgAsmDOSFileHeader::get_rm_section() const {
    return p_rm_section;
}

void
SgAsmDOSFileHeader::set_rm_section(SgAsmGenericSection* const& x) {
    this->p_rm_section = x;
    set_isModified(true);
}

SgAsmDOSFileHeader::~SgAsmDOSFileHeader() {
    destructorHelper();
}

SgAsmDOSFileHeader::SgAsmDOSFileHeader()
    : p_e_last_page_size(0)
    , p_e_total_pages(0)
    , p_e_nrelocs(0)
    , p_e_header_paragraphs(0)
    , p_e_minalloc(0)
    , p_e_maxalloc(0)
    , p_e_ss(0)
    , p_e_sp(0)
    , p_e_cksum(0)
    , p_e_ip(0)
    , p_e_cs(0)
    , p_e_overlay(0)
    , p_e_relocs_offset(0)
    , p_e_res1(0)
    , p_relocs(nullptr)
    , p_rm_section(nullptr) {}

void
SgAsmDOSFileHeader::initializeProperties() {
    p_e_last_page_size = 0;
    p_e_total_pages = 0;
    p_e_nrelocs = 0;
    p_e_header_paragraphs = 0;
    p_e_minalloc = 0;
    p_e_maxalloc = 0;
    p_e_ss = 0;
    p_e_sp = 0;
    p_e_cksum = 0;
    p_e_ip = 0;
    p_e_cs = 0;
    p_e_overlay = 0;
    p_e_relocs_offset = 0;
    p_e_res1 = 0;
    p_relocs = nullptr;
    p_rm_section = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
