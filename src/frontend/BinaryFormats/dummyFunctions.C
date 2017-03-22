#include "sage3basic.h"

// This file contains the function definitions required within ROSE if Binary Analysis 
// support is NOT enabled.  In each case I can't eliminate the function (usually the 
// reason why I can't eliminate them is provides at the top of each list of associated 
// functions).  This list permits definitions to exist and the language specific options
// that are NOT using the binary analysis support to be compiled.  This file allows for 
// a significant simplification of ROSE when Binary Analysis is not selected as one of 
// requested languages to be supported.  In general, executables are treated as a simplily
// another input langauge in ROSE.

// These are not required, except that they are defined as virtual functions and so
// we need to have definitions available so that the class can be used (even though
// these functions are not called.
void SgAsmPESection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmPESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmPESectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmNEFileHeader *fhdr) const {}
void SgAsmNESection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNESectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNENameTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNEModuleTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNEStringTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNEEntryPoint::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNEEntryTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNERelocEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmNERelocTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymverEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymverSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymverDefinedAux::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymverDefinedEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymverDefinedSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymverNeededAux::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymverNeededEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymverNeededSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfEHFrameEntryCI::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfEHFrameEntryFD::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfEHFrameSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmGenericDLL::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmGenericSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmGenericFile::dump(FILE *f) const {}
void SgAsmGenericString::dump(FILE*, const char *prefix, ssize_t idx) const {}
void SgAsmBasicString::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmStoredString::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmStringStorage::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmGenericStrtab::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLEPageTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLEPageTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLESectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLESection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLESectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLENameTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLEEntryPoint::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLEEntryTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmLERelocTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmPEStringSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmGenericFormat::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfDynamicEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfDynamicSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfNoteEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfNoteSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmPEImportDirectory::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmPEImportSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
bool SgAsmPEImportSection::reallocate() { return false; }
void SgAsmElfSectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfStringSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void MemoryMap::dump(FILE *f, const char *prefix) const {}
void SgAsmPEFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmGenericHeader::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmGenericSection *section) const {}
void SgAsmElfSymbolSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmCoffSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmCoffSymbolTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmPEExportDirectory::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmPEExportEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmPEExportSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfRelocEntry::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmElfSymbolSection *symtab) const {}
void SgAsmElfRelocSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSegmentTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmElfSegmentTable::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmDOSFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmDOSExtendedHeader::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmGenericSection::dump(FILE *f, const char *prefix, ssize_t idx) const {}
void SgAsmPEImportItem::dump(FILE*f, char const *prefix, ssize_t idx) const {}

// These are needed because they are virtual functions and need to be defined
// so that the associated classes can be used.
SgAsmPEFileHeader* SgAsmPEFileHeader::parse() { return NULL; }
SgAsmElfStrtab* SgAsmElfStrtab::parse() { return NULL; }
SgAsmPEExportSection *SgAsmPEExportSection::parse() { return NULL; }
SgAsmElfSegmentTable* SgAsmElfSegmentTable::parse() { return NULL; }
SgAsmElfSymverSection* SgAsmElfSymverSection::parse() { return NULL; }
SgAsmElfRelocSection* SgAsmElfRelocSection::parse() { return NULL; }
SgAsmElfEHFrameSection* SgAsmElfEHFrameSection::parse() { return NULL; }
SgAsmElfStringSection* SgAsmElfStringSection::parse() { return NULL; }
SgAsmCoffSymbolTable* SgAsmCoffSymbolTable::parse() { return NULL; }
SgAsmElfSymverDefinedSection* SgAsmElfSymverDefinedSection::parse() { return NULL; }
SgAsmElfSymverNeededSection* SgAsmElfSymverNeededSection::parse() { return NULL; }
SgAsmPESectionTable* SgAsmPESectionTable::parse() { return NULL; }
SgAsmElfFileHeader* SgAsmElfFileHeader::parse() { return NULL; }
SgAsmElfNoteSection* SgAsmElfNoteSection::parse() { return NULL; }
SgAsmElfDynamicSection* SgAsmElfDynamicSection::parse() { return NULL; }
SgAsmPEStringSection* SgAsmPEStringSection::parse() { return NULL; }
SgAsmElfSymbolSection* SgAsmElfSymbolSection::parse() { return NULL; }
SgAsmDOSExtendedHeader* SgAsmDOSExtendedHeader::parse() { return NULL; }
SgAsmPEImportSection* SgAsmPEImportSection::parse() { return NULL; }
SgAsmElfSectionTable* SgAsmElfSectionTable::parse() { return NULL; }
SgAsmDOSFileHeader* SgAsmDOSFileHeader::parse(bool) { return NULL; }

// These are needed because they are implemented elsewhere than in the SOURCE
// section so the ROSETTA can maintain them.
// NOTE that "~SgAsmGenericStrtab() {}" is implemented in the Cxx_Header.h header file.
// NOTE that "~SgAsmPEStringSection() {}" is implemented in the Cxx_Header.h header file.
SgAsmCoffStrtab::~SgAsmCoffStrtab() {}
SgAsmGenericHeader::~SgAsmGenericHeader() {}
SgAsmGenericSection::~SgAsmGenericSection() {}
SgAsmGenericFile::~SgAsmGenericFile() {}
SgAsmElfStrtab::~SgAsmElfStrtab() {}

// These need only compile...
void SgAsmGenericHeader::unparse(std::ostream &f) const {}
void SgAsmGenericSection::unparse(std::ostream &f, const ExtentMap &map) const {}
void SgAsmDOSExtendedHeader::unparse(std::ostream &f) const {}
void SgAsmElfSymbolSection::unparse(std::ostream &f) const {}
void SgAsmDOSFileHeader::unparse(std::ostream &f) const {}
void SgAsmElfSegmentTable::unparse(std::ostream &f) const {}
void SgAsmCoffSymbolTable::unparse(std::ostream &f) const {}
void SgAsmElfRelocSection::unparse(std::ostream &f) const {}
void SgAsmNERelocTable::unparse(std::ostream &f) const {}
void SgAsmPEStringSection::unparse(std::ostream &f) const {}
void SgAsmLESectionTable::unparse(std::ostream &f) const {}
void SgAsmLENameTable::unparse(std::ostream &f) const {}
void SgAsmGenericSection::unparse(std::ostream &f) const {}
void SgAsmLEPageTable::unparse(std::ostream &f) const {}
void SgAsmElfStrtab::unparse(std::ostream &f) const {}
void SgAsmNEEntryTable::unparse(std::ostream &f) const {}
void SgAsmElfEHFrameSection::unparse(std::ostream &f) const {}
void SgAsmNEFileHeader::unparse(std::ostream &f) const {}
void SgAsmElfFileHeader::unparse(std::ostream &f) const {}
void SgAsmNESectionTable::unparse(std::ostream &f) const {}
void SgAsmElfSymverDefinedSection::unparse(std::ostream &f) const {}
void SgAsmLEEntryTable::unparse(std::ostream &f) const {}
void SgAsmNENameTable::unparse(std::ostream &f) const {}
void SgAsmPEFileHeader::unparse(std::ostream &f) const {}
void SgAsmPEImportDirectory::unparse(std::ostream &f, const SgAsmPEImportSection *section, size_t idx) const {}
void SgAsmElfNoteSection::unparse(std::ostream &f) const {}
void SgAsmElfSymverNeededSection::unparse(std::ostream &f) const {}
void SgAsmElfStringSection::unparse(std::ostream &f) const {}
void SgAsmNEModuleTable::unparse(std::ostream &f) const {}
void SgAsmLEFileHeader::unparse(std::ostream &f) const {}
void SgAsmElfSectionTable::unparse(std::ostream &f) const {}
void SgAsmElfDynamicSection::unparse(std::ostream &f) const {}
void SgAsmPEImportSection::unparse(std::ostream &f) const {}
void SgAsmElfSymverSection::unparse(std::ostream &f) const {}
void SgAsmCoffStrtab::unparse(std::ostream &f) const {}
void SgAsmPESectionTable::unparse(std::ostream &f) const {}
void SgAsmNESection::unparse(std::ostream &f) const {}


std::ostream & operator<< ( std::ostream & os, const SgAsmNERelocEntry::iref_type    & x ) { return os; }
std::ostream & operator<< ( std::ostream & os, const SgAsmNERelocEntry::iord_type    & x ) { return os; }
std::ostream & operator<< ( std::ostream & os, const SgAsmNERelocEntry::iname_type   & x ) { return os; }
std::ostream & operator<< ( std::ostream & os, const SgAsmNERelocEntry::osfixup_type & x ) { return os; }
std::ostream & operator<< ( std::ostream & os, const RegisterDescriptor & x ) { return os; }
std::ostream & operator<< ( std::ostream & os, const rose_rva_t & x ) { return os; }
std::ostream& operator<<(std::ostream &os, const AddressIntervalSet&) { return os; }

bool SgAsmDOSFileHeader::reallocate() { return false; }
void SgAsmPEStringSection::set_size(rose_addr_t) {}
void SgAsmGenericSection::set_mapped_size(rose_addr_t) {}
SgAsmGenericSection* SgAsmGenericSection::parse() { return NULL; }
bool SgAsmElfStringSection::reallocate() { return false; }
bool SgAsmElfNoteSection::reallocate() { return false; }
void SgAsmElfStrtab::rebind(SgAsmStringStorage*, rose_addr_t) {}
rose_addr_t SgAsmElfStrtab::get_storage_size(SgAsmStringStorage const*) { return 0;}
void SgAsmElfStrtab::allocate_overlap(SgAsmStringStorage*) {}
bool SgAsmElfSegmentTable::reallocate() { return false; }

rose_addr_t SgAsmElfSymverDefinedSection::calculate_sizes(size_t*, size_t*, size_t*, size_t*) const { return 0;}
rose_addr_t SgAsmElfSection::calculate_sizes(size_t*, size_t*, size_t*, size_t*) const { return 0;}

bool SgAsmPEFileHeader::reallocate() { return false; }
SgAsmStringStorage* SgAsmElfStrtab::create_storage(rose_addr_t, bool) { return NULL;}
void SgAsmElfDynamicSection::finish_parsing() {}
SgAsmNERelocEntry::osfixup_type::osfixup_type() {}

rose_addr_t SgAsmElfRelocSection::calculate_sizes(size_t*, size_t*, size_t*, size_t*) const { return 0;}

std::string SgAsmStoredString::get_string(bool escape) const { return ""; }
rose_addr_t SgAsmStoredString::get_offset() const { return 0;}
bool SgAsmPESectionTable::reallocate() { return false; }
bool SgAsmPEStringSection::reallocate() { return false; }
SgAsmStringStorage* SgAsmCoffStrtab::create_storage(rose_addr_t, bool) { return NULL;}
SgAsmNERelocEntry::iname_type::iname_type() {}

rose_addr_t SgAsmElfSymbolSection::calculate_sizes(size_t*, size_t*, size_t*, size_t*) const { return 0;}

rose_addr_t SgAsmCoffStrtab::get_storage_size(SgAsmStringStorage const*) { return 0;}
bool SgAsmElfDynamicSection::reallocate() { return false; }

rose_addr_t SgAsmElfEHFrameSection::calculate_sizes(size_t*, size_t*, size_t*, size_t*) const { return 0;}

const char* SgAsmLEFileHeader::format_name() const { return NULL;}

rose_addr_t SgAsmElfDynamicSection::calculate_sizes(size_t*, size_t*, size_t*, size_t*) const { return 0;}

void SgAsmBasicString::set_string(rose_addr_t) {}
void SgAsmElfStringSection::set_size(rose_addr_t) {}
SgAsmNERelocEntry::iord_type::iord_type() {}
void SgAsmStoredString::set_string(const std::string&) {}
void SgAsmGenericSection::set_mapped_preferred_rva(rose_addr_t) {}
bool SgAsmElfSection::reallocate() { return false; }
std::string SgAsmGenericString::get_string(bool escape) const { return ""; }
SgAsmNERelocEntry::iref_type::iref_type() {}
bool SgAsmPESection::reallocate() { return false; }
void SgAsmBasicString::set_string(std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) {}
void SgAsmGenericString::set_string(rose_addr_t) {}
std::string SgAsmBasicString::get_string(bool escape) const { return ""; }
bool SgAsmElfRelocSection::reallocate() { return false; }
void SgAsmElfSymbolSection::finish_parsing() {}
bool SgAsmElfFileHeader::reallocate() { return false; }
void SgAsmGenericSection::set_offset(rose_addr_t) {}
void SgAsmGenericString::set_string(std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) {}
void SgAsmStoredString::set_string(rose_addr_t) {}
bool SgAsmGenericHeader::reallocate() { return false; }
bool SgAsmElfSectionTable::reallocate() { return false; }

rose_addr_t SgAsmElfSymverNeededSection::calculate_sizes(size_t*, size_t*, size_t*, size_t*) const { return 0;}

bool SgAsmElfSymbolSection::reallocate() { return false; }

rose_addr_t SgAsmElfSymverSection::calculate_sizes(size_t*, size_t*, size_t*, size_t*) const { return 0;}

void SgAsmGenericSection::set_size(rose_addr_t) {}
SgAsmGenericString* SgAsmElfNoteEntry::get_name() const { return NULL; }
SgAsmGenericString* SgAsmGenericSymbol::get_name() const { return NULL; }

const char* SgAsmElfFileHeader::format_name() const { return NULL; }


rose_rva_t::rose_rva_t() {
    addr = 0;
    section = NULL;
}

rose_rva_t::rose_rva_t(rose_addr_t rva, SgAsmGenericSection*) {
    addr = rva;
    section = NULL;
}

rose_rva_t::rose_rva_t(const rose_rva_t &other) {
    addr = other.addr;
    section = other.section;
}

rose_rva_t rose_rva_t::operator=(const rose_rva_t &other) {
    addr = other.addr;
    section = other.section;
    return *this;
}

rose_addr_t
rose_rva_t::get_rva() const
{
    rose_addr_t rva = addr;
    return rva;
}

void
SgAsmGenericFile::shift_extend(SgAsmGenericSection*, rose_addr_t sa, rose_addr_t sn, AddressSpace, Elasticity) {}

// Constructor helpers
void SgAsmInterpretation::ctor() {}
void SgAsmGenericFile::ctor() {}
void SgAsmElfEHFrameEntryFD::ctor(SgAsmElfEHFrameEntryCI *cie) {}
void SgAsmGenericHeader::ctor() {}
void SgAsmPEFileHeader::ctor() {}
void SgAsmLEFileHeader::ctor(SgAsmGenericFile *f, rose_addr_t offset) {}
void SgAsmNEFileHeader::ctor(SgAsmGenericFile *f, rose_addr_t offset) {}
void SgAsmDOSFileHeader::ctor() {}
void SgAsmElfFileHeader::ctor() {}
void SgAsmElfSymbolSection::ctor(SgAsmElfStringSection *s) {}
void SgAsmElfRelocSection::ctor(SgAsmElfSymbolSection *s1,SgAsmElfSection *s2) {}
void SgAsmElfDynamicSection::ctor(SgAsmElfStringSection *s) {}
void SgAsmElfNoteSection::ctor() {}
void SgAsmElfEHFrameSection::ctor() {}
void SgAsmElfSymverSection::ctor() {}
void SgAsmElfSymverDefinedSection::ctor(SgAsmElfStringSection *s) {}
void SgAsmElfSymverNeededSection::ctor(SgAsmElfStringSection *s) {}
void SgAsmElfSectionTable::ctor() {}
void SgAsmElfSegmentTable::ctor() {}
void SgAsmPEImportSection::ctor() {}
void SgAsmPEExportSection::ctor() {}
void SgAsmPEStringSection::ctor() {}
void SgAsmPESectionTable::ctor() {}
void SgAsmDOSExtendedHeader::ctor() {}
void SgAsmCoffSymbolTable::ctor() {}
void SgAsmNESectionTable::ctor() {}
void SgAsmNENameTable::ctor(rose_addr_t i) {}
void SgAsmNEModuleTable::ctor(rose_addr_t i1, rose_addr_t i2) {}
void SgAsmNEStringTable::ctor(rose_addr_t i1, rose_addr_t i2) {}
void SgAsmNEEntryTable::ctor(rose_addr_t i1, rose_addr_t i2) {}
void SgAsmNERelocTable::ctor(SgAsmNESection *s) {}
void SgAsmLESectionTable::ctor(rose_addr_t i1, rose_addr_t i2) {}
void SgAsmLENameTable::ctor(rose_addr_t i) {}
void SgAsmLEPageTable::ctor(rose_addr_t i1, rose_addr_t i2) {}
void SgAsmLEEntryTable::ctor(rose_addr_t i) {}
void SgAsmLERelocTable::ctor(rose_addr_t i) {}
void SgAsmElfSymbol::ctor(SgAsmElfSymbolSection *s) {}
void SgAsmElfStrtab::ctor() {}
void SgAsmBasicString::ctor() {}
void SgAsmStoredString::ctor(SgAsmGenericStrtab *s,rose_addr_t i,bool b) {}
void SgAsmStoredString::ctor(SgAsmGenericStrtab *strtab, const std::string &s) {}
void SgAsmStoredString::ctor(SgAsmStringStorage *storage) {}
void SgAsmElfRelocEntry::ctor(SgAsmElfRelocSection *s) {}
void SgAsmPEExportEntry::ctor(SgAsmGenericString *fname, unsigned ordinal, rose_rva_t expaddr, SgAsmGenericString *forwarder) {}
void SgAsmElfDynamicEntry::ctor(SgAsmElfDynamicSection *s){}
void SgAsmElfNoteEntry::ctor(SgAsmElfNoteSection *s){}
void SgAsmElfSymverEntry::ctor(SgAsmElfSymverSection *s){}
void SgAsmElfSymverDefinedEntry::ctor(SgAsmElfSymverDefinedSection *s){}
void SgAsmElfSymverDefinedAux::ctor(SgAsmElfSymverDefinedEntry* entry, SgAsmElfSymverDefinedSection* symver){}
void SgAsmElfSymverNeededEntry::ctor(SgAsmElfSymverNeededSection *s){}
void SgAsmElfSymverNeededAux::ctor(SgAsmElfSymverNeededEntry* entry, SgAsmElfSymverNeededSection* symver){}
void SgAsmPEImportDirectory::ctor(SgAsmPEImportSection *section, const std::string &dll_name){}
void SgAsmPEExportDirectory::ctor(SgAsmPEExportSection *section){}
void SgAsmPERVASizePair::ctor(SgAsmPERVASizePairList *parent, const RVASizePair_disk *disk){}
void SgAsmPERVASizePair::ctor(SgAsmPERVASizePairList *parent, rose_addr_t rva, rose_addr_t size){}
void SgAsmElfEHFrameEntryCI::ctor(SgAsmElfEHFrameSection *ehframe){}
void SgAsmPEImportItem::ctor(SgAsmPEImportItemList *parent){}
void SgAsmPEImportItem::ctor(SgAsmPEImportDirectory *idir){}
void SgAsmPEImportItem::ctor(SgAsmPEImportDirectory *idir, const std::string &name, unsigned hint){}
void SgAsmPEImportItem::ctor(SgAsmPEImportDirectory *idir, unsigned ordinal){}
void SgAsmElfStringSection::ctor() {}
void SgAsmElfStringSection::ctor(SgAsmElfSectionTable*) {}
void SgAsmElfSection::ctor() {}
void SgAsmGenericSymbol::ctor() {}
void SgAsmGenericSection::ctor(SgAsmGenericFile*, SgAsmGenericHeader*) {}
void SgAsmGenericDLL::ctor() {}

size_t SgAsmPEImportSection::mesg_nprinted = 0;
rose_addr_t rose_rva_t::get_va() const { return 0; }
std::string SgAsmGenericSection::read_content_str(const MemoryMap*, rose_addr_t, bool) { return ""; }
std::string SgAsmGenericSection::read_content_str(rose_addr_t, bool) { return ""; }

SgAsmGenericString* SgAsmGenericDLL::get_name() const { return NULL; }
void SgAsmGenericDLL::set_name(SgAsmGenericString*) {}
void SgAsmElfNoteEntry::set_name(SgAsmGenericString*) {}
void SgAsmGenericSymbol::set_name(SgAsmGenericString*) {}
void SgAsmPEExportEntry::set_forwarder(SgAsmGenericString*) {}
void SgAsmPEExportEntry::set_name(SgAsmGenericString*) {}
void SgAsmElfDynamicEntry::set_name(SgAsmGenericString*) {}
void SgAsmElfSection::set_linked_section(SgAsmElfSection*) {}
const char* SgAsmGenericHeader::format_name() const { return NULL; }
