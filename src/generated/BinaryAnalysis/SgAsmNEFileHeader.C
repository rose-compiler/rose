//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation for SgAsmNEFileHeader            -- MACHINE GENERATED; DO NOT MODIFY --
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS)
#define ROSE_SgAsmNEFileHeader_IMPL
#include <SgAsmNEFileHeader.h>

unsigned const&
SgAsmNEFileHeader::get_e_linker_major() const {
    return p_e_linker_major;
}

void
SgAsmNEFileHeader::set_e_linker_major(unsigned const& x) {
    this->p_e_linker_major = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_linker_minor() const {
    return p_e_linker_minor;
}

void
SgAsmNEFileHeader::set_e_linker_minor(unsigned const& x) {
    this->p_e_linker_minor = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_checksum() const {
    return p_e_checksum;
}

void
SgAsmNEFileHeader::set_e_checksum(unsigned const& x) {
    this->p_e_checksum = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_flags1() const {
    return p_e_flags1;
}

void
SgAsmNEFileHeader::set_e_flags1(unsigned const& x) {
    this->p_e_flags1 = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_autodata_sn() const {
    return p_e_autodata_sn;
}

void
SgAsmNEFileHeader::set_e_autodata_sn(unsigned const& x) {
    this->p_e_autodata_sn = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_bss_size() const {
    return p_e_bss_size;
}

void
SgAsmNEFileHeader::set_e_bss_size(unsigned const& x) {
    this->p_e_bss_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_stack_size() const {
    return p_e_stack_size;
}

void
SgAsmNEFileHeader::set_e_stack_size(unsigned const& x) {
    this->p_e_stack_size = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_csip() const {
    return p_e_csip;
}

void
SgAsmNEFileHeader::set_e_csip(unsigned const& x) {
    this->p_e_csip = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_sssp() const {
    return p_e_sssp;
}

void
SgAsmNEFileHeader::set_e_sssp(unsigned const& x) {
    this->p_e_sssp = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_nsections() const {
    return p_e_nsections;
}

void
SgAsmNEFileHeader::set_e_nsections(unsigned const& x) {
    this->p_e_nsections = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_nmodrefs() const {
    return p_e_nmodrefs;
}

void
SgAsmNEFileHeader::set_e_nmodrefs(unsigned const& x) {
    this->p_e_nmodrefs = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_nnonresnames() const {
    return p_e_nnonresnames;
}

void
SgAsmNEFileHeader::set_e_nnonresnames(unsigned const& x) {
    this->p_e_nnonresnames = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_nmovable_entries() const {
    return p_e_nmovable_entries;
}

void
SgAsmNEFileHeader::set_e_nmovable_entries(unsigned const& x) {
    this->p_e_nmovable_entries = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_sector_align() const {
    return p_e_sector_align;
}

void
SgAsmNEFileHeader::set_e_sector_align(unsigned const& x) {
    this->p_e_sector_align = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_nresources() const {
    return p_e_nresources;
}

void
SgAsmNEFileHeader::set_e_nresources(unsigned const& x) {
    this->p_e_nresources = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_exetype() const {
    return p_e_exetype;
}

void
SgAsmNEFileHeader::set_e_exetype(unsigned const& x) {
    this->p_e_exetype = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_flags2() const {
    return p_e_flags2;
}

void
SgAsmNEFileHeader::set_e_flags2(unsigned const& x) {
    this->p_e_flags2 = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_res1() const {
    return p_e_res1;
}

void
SgAsmNEFileHeader::set_e_res1(unsigned const& x) {
    this->p_e_res1 = x;
    set_isModified(true);
}

unsigned const&
SgAsmNEFileHeader::get_e_winvers() const {
    return p_e_winvers;
}

void
SgAsmNEFileHeader::set_e_winvers(unsigned const& x) {
    this->p_e_winvers = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_entrytab_rfo() const {
    return p_e_entrytab_rfo;
}

void
SgAsmNEFileHeader::set_e_entrytab_rfo(rose_addr_t const& x) {
    this->p_e_entrytab_rfo = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_entrytab_size() const {
    return p_e_entrytab_size;
}

void
SgAsmNEFileHeader::set_e_entrytab_size(rose_addr_t const& x) {
    this->p_e_entrytab_size = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_sectab_rfo() const {
    return p_e_sectab_rfo;
}

void
SgAsmNEFileHeader::set_e_sectab_rfo(rose_addr_t const& x) {
    this->p_e_sectab_rfo = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_rsrctab_rfo() const {
    return p_e_rsrctab_rfo;
}

void
SgAsmNEFileHeader::set_e_rsrctab_rfo(rose_addr_t const& x) {
    this->p_e_rsrctab_rfo = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_resnametab_rfo() const {
    return p_e_resnametab_rfo;
}

void
SgAsmNEFileHeader::set_e_resnametab_rfo(rose_addr_t const& x) {
    this->p_e_resnametab_rfo = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_modreftab_rfo() const {
    return p_e_modreftab_rfo;
}

void
SgAsmNEFileHeader::set_e_modreftab_rfo(rose_addr_t const& x) {
    this->p_e_modreftab_rfo = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_importnametab_rfo() const {
    return p_e_importnametab_rfo;
}

void
SgAsmNEFileHeader::set_e_importnametab_rfo(rose_addr_t const& x) {
    this->p_e_importnametab_rfo = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_nonresnametab_offset() const {
    return p_e_nonresnametab_offset;
}

void
SgAsmNEFileHeader::set_e_nonresnametab_offset(rose_addr_t const& x) {
    this->p_e_nonresnametab_offset = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_fastload_sector() const {
    return p_e_fastload_sector;
}

void
SgAsmNEFileHeader::set_e_fastload_sector(rose_addr_t const& x) {
    this->p_e_fastload_sector = x;
    set_isModified(true);
}

rose_addr_t const&
SgAsmNEFileHeader::get_e_fastload_nsectors() const {
    return p_e_fastload_nsectors;
}

void
SgAsmNEFileHeader::set_e_fastload_nsectors(rose_addr_t const& x) {
    this->p_e_fastload_nsectors = x;
    set_isModified(true);
}

SgAsmDOSExtendedHeader* const&
SgAsmNEFileHeader::get_dos2Header() const {
    return p_dos2Header;
}

void
SgAsmNEFileHeader::set_dos2Header(SgAsmDOSExtendedHeader* const& x) {
    changeChildPointer(this->p_dos2Header, const_cast<SgAsmDOSExtendedHeader*&>(x));
    set_isModified(true);
}

SgAsmNESectionTable* const&
SgAsmNEFileHeader::get_sectionTable() const {
    return p_sectionTable;
}

void
SgAsmNEFileHeader::set_sectionTable(SgAsmNESectionTable* const& x) {
    this->p_sectionTable = x;
    set_isModified(true);
}

SgAsmNENameTable* const&
SgAsmNEFileHeader::get_residentNameTable() const {
    return p_residentNameTable;
}

void
SgAsmNEFileHeader::set_residentNameTable(SgAsmNENameTable* const& x) {
    changeChildPointer(this->p_residentNameTable, const_cast<SgAsmNENameTable*&>(x));
    set_isModified(true);
}

SgAsmNENameTable* const&
SgAsmNEFileHeader::get_nonresidentNameTable() const {
    return p_nonresidentNameTable;
}

void
SgAsmNEFileHeader::set_nonresidentNameTable(SgAsmNENameTable* const& x) {
    changeChildPointer(this->p_nonresidentNameTable, const_cast<SgAsmNENameTable*&>(x));
    set_isModified(true);
}

SgAsmNEModuleTable* const&
SgAsmNEFileHeader::get_moduleTable() const {
    return p_moduleTable;
}

void
SgAsmNEFileHeader::set_moduleTable(SgAsmNEModuleTable* const& x) {
    changeChildPointer(this->p_moduleTable, const_cast<SgAsmNEModuleTable*&>(x));
    set_isModified(true);
}

SgAsmNEEntryTable* const&
SgAsmNEFileHeader::get_entryTable() const {
    return p_entryTable;
}

void
SgAsmNEFileHeader::set_entryTable(SgAsmNEEntryTable* const& x) {
    changeChildPointer(this->p_entryTable, const_cast<SgAsmNEEntryTable*&>(x));
    set_isModified(true);
}

SgAsmNEFileHeader::~SgAsmNEFileHeader() {
    destructorHelper();
}

SgAsmNEFileHeader::SgAsmNEFileHeader()
    : p_e_linker_major(0)
    , p_e_linker_minor(0)
    , p_e_checksum(0)
    , p_e_flags1(0)
    , p_e_autodata_sn(0)
    , p_e_bss_size(0)
    , p_e_stack_size(0)
    , p_e_csip(0)
    , p_e_sssp(0)
    , p_e_nsections(0)
    , p_e_nmodrefs(0)
    , p_e_nnonresnames(0)
    , p_e_nmovable_entries(0)
    , p_e_sector_align(0)
    , p_e_nresources(0)
    , p_e_exetype(0)
    , p_e_flags2(0)
    , p_e_res1(0)
    , p_e_winvers(0)
    , p_e_entrytab_rfo(0)
    , p_e_entrytab_size(0)
    , p_e_sectab_rfo(0)
    , p_e_rsrctab_rfo(0)
    , p_e_resnametab_rfo(0)
    , p_e_modreftab_rfo(0)
    , p_e_importnametab_rfo(0)
    , p_e_nonresnametab_offset(0)
    , p_e_fastload_sector(0)
    , p_e_fastload_nsectors(0)
    , p_dos2Header(nullptr)
    , p_sectionTable(nullptr)
    , p_residentNameTable(nullptr)
    , p_nonresidentNameTable(nullptr)
    , p_moduleTable(nullptr)
    , p_entryTable(nullptr) {}

void
SgAsmNEFileHeader::initializeProperties() {
    p_e_linker_major = 0;
    p_e_linker_minor = 0;
    p_e_checksum = 0;
    p_e_flags1 = 0;
    p_e_autodata_sn = 0;
    p_e_bss_size = 0;
    p_e_stack_size = 0;
    p_e_csip = 0;
    p_e_sssp = 0;
    p_e_nsections = 0;
    p_e_nmodrefs = 0;
    p_e_nnonresnames = 0;
    p_e_nmovable_entries = 0;
    p_e_sector_align = 0;
    p_e_nresources = 0;
    p_e_exetype = 0;
    p_e_flags2 = 0;
    p_e_res1 = 0;
    p_e_winvers = 0;
    p_e_entrytab_rfo = 0;
    p_e_entrytab_size = 0;
    p_e_sectab_rfo = 0;
    p_e_rsrctab_rfo = 0;
    p_e_resnametab_rfo = 0;
    p_e_modreftab_rfo = 0;
    p_e_importnametab_rfo = 0;
    p_e_nonresnametab_offset = 0;
    p_e_fastload_sector = 0;
    p_e_fastload_nsectors = 0;
    p_dos2Header = nullptr;
    p_sectionTable = nullptr;
    p_residentNameTable = nullptr;
    p_nonresidentNameTable = nullptr;
    p_moduleTable = nullptr;
    p_entryTable = nullptr;
}

#endif // defined(ROSE_ENABLE_BINARY_ANALYSIS)
