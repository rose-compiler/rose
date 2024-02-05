//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmPEFileHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#include <sage3basic.h>

unsigned const&
SgAsmPEFileHeader::get_e_cpu_type() const {
    return p_e_cpu_type;
}

void
SgAsmPEFileHeader::set_e_cpu_type(unsigned const& x) {
    this->p_e_cpu_type = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_nsections() const {
    return p_e_nsections;
}

void
SgAsmPEFileHeader::set_e_nsections(unsigned const& x) {
    this->p_e_nsections = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_time() const {
    return p_e_time;
}

void
SgAsmPEFileHeader::set_e_time(unsigned const& x) {
    this->p_e_time = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmPEFileHeader::get_e_coff_symtab() const {
    return p_e_coff_symtab;
}

void
SgAsmPEFileHeader::set_e_coff_symtab(rose_addr_t const& x) {
    this->p_e_coff_symtab = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmPEFileHeader::get_e_nt_hdr_size() const {
    return p_e_nt_hdr_size;
}

void
SgAsmPEFileHeader::set_e_nt_hdr_size(rose_addr_t const& x) {
    this->p_e_nt_hdr_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_coff_nsyms() const {
    return p_e_coff_nsyms;
}

void
SgAsmPEFileHeader::set_e_coff_nsyms(unsigned const& x) {
    this->p_e_coff_nsyms = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_flags() const {
    return p_e_flags;
}

void
SgAsmPEFileHeader::set_e_flags(unsigned const& x) {
    this->p_e_flags = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_opt_magic() const {
    return p_e_opt_magic;
}

void
SgAsmPEFileHeader::set_e_opt_magic(unsigned const& x) {
    this->p_e_opt_magic = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_lmajor() const {
    return p_e_lmajor;
}

void
SgAsmPEFileHeader::set_e_lmajor(unsigned const& x) {
    this->p_e_lmajor = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_lminor() const {
    return p_e_lminor;
}

void
SgAsmPEFileHeader::set_e_lminor(unsigned const& x) {
    this->p_e_lminor = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_code_size() const {
    return p_e_code_size;
}

void
SgAsmPEFileHeader::set_e_code_size(unsigned const& x) {
    this->p_e_code_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_data_size() const {
    return p_e_data_size;
}

void
SgAsmPEFileHeader::set_e_data_size(unsigned const& x) {
    this->p_e_data_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_bss_size() const {
    return p_e_bss_size;
}

void
SgAsmPEFileHeader::set_e_bss_size(unsigned const& x) {
    this->p_e_bss_size = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmPEFileHeader::get_e_code_rva() const {
    return p_e_code_rva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmPEFileHeader::get_e_code_rva() {
    return p_e_code_rva;
}

void
SgAsmPEFileHeader::set_e_code_rva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_e_code_rva = x;
    set_isModified(true);
}

Rose::BinaryAnalysis::RelativeVirtualAddress const&
SgAsmPEFileHeader::get_e_data_rva() const {
    return p_e_data_rva;
}

Rose::BinaryAnalysis::RelativeVirtualAddress&
SgAsmPEFileHeader::get_e_data_rva() {
    return p_e_data_rva;
}

void
SgAsmPEFileHeader::set_e_data_rva(Rose::BinaryAnalysis::RelativeVirtualAddress const& x) {
    this->p_e_data_rva = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_section_align() const {
    return p_e_section_align;
}

void
SgAsmPEFileHeader::set_e_section_align(unsigned const& x) {
    this->p_e_section_align = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_file_align() const {
    return p_e_file_align;
}

void
SgAsmPEFileHeader::set_e_file_align(unsigned const& x) {
    this->p_e_file_align = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_os_major() const {
    return p_e_os_major;
}

void
SgAsmPEFileHeader::set_e_os_major(unsigned const& x) {
    this->p_e_os_major = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_os_minor() const {
    return p_e_os_minor;
}

void
SgAsmPEFileHeader::set_e_os_minor(unsigned const& x) {
    this->p_e_os_minor = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_user_major() const {
    return p_e_user_major;
}

void
SgAsmPEFileHeader::set_e_user_major(unsigned const& x) {
    this->p_e_user_major = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_user_minor() const {
    return p_e_user_minor;
}

void
SgAsmPEFileHeader::set_e_user_minor(unsigned const& x) {
    this->p_e_user_minor = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_subsys_major() const {
    return p_e_subsys_major;
}

void
SgAsmPEFileHeader::set_e_subsys_major(unsigned const& x) {
    this->p_e_subsys_major = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_subsys_minor() const {
    return p_e_subsys_minor;
}

void
SgAsmPEFileHeader::set_e_subsys_minor(unsigned const& x) {
    this->p_e_subsys_minor = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_reserved9() const {
    return p_e_reserved9;
}

void
SgAsmPEFileHeader::set_e_reserved9(unsigned const& x) {
    this->p_e_reserved9 = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_image_size() const {
    return p_e_image_size;
}

void
SgAsmPEFileHeader::set_e_image_size(unsigned const& x) {
    this->p_e_image_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_header_size() const {
    return p_e_header_size;
}

void
SgAsmPEFileHeader::set_e_header_size(unsigned const& x) {
    this->p_e_header_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_file_checksum() const {
    return p_e_file_checksum;
}

void
SgAsmPEFileHeader::set_e_file_checksum(unsigned const& x) {
    this->p_e_file_checksum = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_subsystem() const {
    return p_e_subsystem;
}

void
SgAsmPEFileHeader::set_e_subsystem(unsigned const& x) {
    this->p_e_subsystem = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_dll_flags() const {
    return p_e_dll_flags;
}

void
SgAsmPEFileHeader::set_e_dll_flags(unsigned const& x) {
    this->p_e_dll_flags = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_stack_reserve_size() const {
    return p_e_stack_reserve_size;
}

void
SgAsmPEFileHeader::set_e_stack_reserve_size(unsigned const& x) {
    this->p_e_stack_reserve_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_stack_commit_size() const {
    return p_e_stack_commit_size;
}

void
SgAsmPEFileHeader::set_e_stack_commit_size(unsigned const& x) {
    this->p_e_stack_commit_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_heap_reserve_size() const {
    return p_e_heap_reserve_size;
}

void
SgAsmPEFileHeader::set_e_heap_reserve_size(unsigned const& x) {
    this->p_e_heap_reserve_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_heap_commit_size() const {
    return p_e_heap_commit_size;
}

void
SgAsmPEFileHeader::set_e_heap_commit_size(unsigned const& x) {
    this->p_e_heap_commit_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_loader_flags() const {
    return p_e_loader_flags;
}

void
SgAsmPEFileHeader::set_e_loader_flags(unsigned const& x) {
    this->p_e_loader_flags = x;
    set_isModified(true);
}

unsigned const&
SgAsmPEFileHeader::get_e_num_rvasize_pairs() const {
    return p_e_num_rvasize_pairs;
}

void
SgAsmPEFileHeader::set_e_num_rvasize_pairs(unsigned const& x) {
    this->p_e_num_rvasize_pairs = x;
    set_isModified(true);
}

SgAsmPERVASizePairList* const&
SgAsmPEFileHeader::get_rvaSizePairs() const {
    return p_rvaSizePairs;
}

void
SgAsmPEFileHeader::set_rvaSizePairs(SgAsmPERVASizePairList* const& x) {
    changeChildPointer(this->p_rvaSizePairs, const_cast<SgAsmPERVASizePairList*&>(x));
    set_isModified(true);
}

SgAsmPESectionTable* const&
SgAsmPEFileHeader::get_sectionTable() const {
    return p_sectionTable;
}

void
SgAsmPEFileHeader::set_sectionTable(SgAsmPESectionTable* const& x) {
    this->p_sectionTable = x;
    set_isModified(true);
}

SgAsmCoffSymbolTable* const&
SgAsmPEFileHeader::get_coffSymbolTable() const {
    return p_coffSymbolTable;
}

void
SgAsmPEFileHeader::set_coffSymbolTable(SgAsmCoffSymbolTable* const& x) {
    this->p_coffSymbolTable = x;
    set_isModified(true);
}

SgAsmPEFileHeader::~SgAsmPEFileHeader() {
    destructorHelper();
}

SgAsmPEFileHeader::SgAsmPEFileHeader()
    : p_e_cpu_type(0)
    , p_e_nsections(0)
    , p_e_time(0)
    , p_e_coff_symtab(0)
    , p_e_nt_hdr_size(0)
    , p_e_coff_nsyms(0)
    , p_e_flags(0)
    , p_e_opt_magic(0)
    , p_e_lmajor(0)
    , p_e_lminor(0)
    , p_e_code_size(0)
    , p_e_data_size(0)
    , p_e_bss_size(0)
    , p_e_section_align(0)
    , p_e_file_align(0)
    , p_e_os_major(0)
    , p_e_os_minor(0)
    , p_e_user_major(0)
    , p_e_user_minor(0)
    , p_e_subsys_major(0)
    , p_e_subsys_minor(0)
    , p_e_reserved9(0)
    , p_e_image_size(0)
    , p_e_header_size(0)
    , p_e_file_checksum(0)
    , p_e_subsystem(0)
    , p_e_dll_flags(0)
    , p_e_stack_reserve_size(0)
    , p_e_stack_commit_size(0)
    , p_e_heap_reserve_size(0)
    , p_e_heap_commit_size(0)
    , p_e_loader_flags(0)
    , p_e_num_rvasize_pairs(0)
    , p_rvaSizePairs(nullptr)
    , p_sectionTable(nullptr)
    , p_coffSymbolTable(nullptr) {}

void
SgAsmPEFileHeader::initializeProperties() {
    p_e_cpu_type = 0;
    p_e_nsections = 0;
    p_e_time = 0;
    p_e_coff_symtab = 0;
    p_e_nt_hdr_size = 0;
    p_e_coff_nsyms = 0;
    p_e_flags = 0;
    p_e_opt_magic = 0;
    p_e_lmajor = 0;
    p_e_lminor = 0;
    p_e_code_size = 0;
    p_e_data_size = 0;
    p_e_bss_size = 0;
    p_e_section_align = 0;
    p_e_file_align = 0;
    p_e_os_major = 0;
    p_e_os_minor = 0;
    p_e_user_major = 0;
    p_e_user_minor = 0;
    p_e_subsys_major = 0;
    p_e_subsys_minor = 0;
    p_e_reserved9 = 0;
    p_e_image_size = 0;
    p_e_header_size = 0;
    p_e_file_checksum = 0;
    p_e_subsystem = 0;
    p_e_dll_flags = 0;
    p_e_stack_reserve_size = 0;
    p_e_stack_commit_size = 0;
    p_e_heap_reserve_size = 0;
    p_e_heap_commit_size = 0;
    p_e_loader_flags = 0;
    p_e_num_rvasize_pairs = 0;
    p_rvaSizePairs = nullptr;
    p_sectionTable = nullptr;
    p_coffSymbolTable = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
