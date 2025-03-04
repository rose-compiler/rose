/* ELF Symbol Tables (SgAsmElfSymbolSection and related classes) */
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include "sage3basic.h"

#include <Rose/BinaryAnalysis/Hexdump.h>
#include "stringify.h"

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#if defined(__GNUC__) && __GNUC__ >= 9
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
#endif

using namespace Rose;

SgAsmElfSymbol::SgAsmElfSymbol(SgAsmElfSymbolSection *symtab) {
    initializeProperties();
    ASSERT_not_null(symtab);
    SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(symtab->get_linkedSection());
    ASSERT_not_null(strsec);
    
    set_name(new SgAsmStoredString(strsec->get_strtab(), 0));

    ASSERT_not_null(symtab->get_symbols());
    symtab->get_symbols()->get_symbols().push_back(this);
    ASSERT_require(symtab->get_symbols()->get_symbols().size() > 0);
    set_parent(symtab->get_symbols());

    set_st_info(0);
    set_st_res1(0);
    set_st_shndx(0);
    set_st_size(0);
}

void
SgAsmElfSymbol::parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const Elf32SymbolEntry_disk *disk)
{
    p_st_info  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_info);
    p_st_res1  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_res1);
    p_st_shndx = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_shndx);
    p_st_size  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_size);

    p_value    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_value);
    p_size     = p_st_size;

    Rose::BinaryAnalysis::Address name_offset  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_name);
    get_name()->set_string(name_offset);

    parse_common();
}

void
SgAsmElfSymbol::parse(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const Elf64SymbolEntry_disk *disk)
{
    p_st_info  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_info);
    p_st_res1  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_res1);
    p_st_shndx = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_shndx);
    p_st_size  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_size);

    p_value    = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_value);
    p_size     = p_st_size;

    Rose::BinaryAnalysis::Address name_offset  = Rose::BinaryAnalysis::ByteOrder::diskToHost(sex, disk->st_name);
    get_name()->set_string(name_offset);

    parse_common();
}

void
SgAsmElfSymbol::parse_common()
{
    /* Binding */
    switch (get_elfBinding()) {
        case STB_LOCAL:   p_binding = SYM_LOCAL;      break;
        case STB_GLOBAL:  p_binding = SYM_GLOBAL;     break;
        case STB_WEAK:    p_binding = SYM_WEAK;       break;
        default:          p_binding = SYM_NO_BINDING; break;
    }

    /* Type */
    switch (get_elfType()) {
        case STT_NOTYPE:  p_type = SYM_NO_TYPE; break;
        case STT_OBJECT:  p_type = SYM_DATA;    break;
        case STT_FUNC:    p_type = SYM_FUNC;    break;
        case STT_SECTION: p_type = SYM_SECTION; break;
        case STT_FILE:    p_type = SYM_FILE;    break;
        case STT_COMMON:  p_type = SYM_COMMON;  break;
        case STT_TLS:     p_type = SYM_TLS;     break;
        case STT_IFUNC:   p_type = SYM_IFUNC;   break;
        default:          p_type = SYM_NO_TYPE; break;
    }

    /* Definition state */
    if (p_value || p_size) {
        p_definitionState = SYM_DEFINED;
    } else if (p_name->get_string().size() > 0 || get_elfType()) {
        p_definitionState = SYM_TENTATIVE;
    } else {
        p_definitionState = SYM_UNDEFINED;
    }
}

void
SgAsmElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    dump(f, prefix, idx, NULL);
}

std::string
SgAsmElfSymbol::to_string(ElfSymBinding val)
{
    return toString(val);
}

std::string
SgAsmElfSymbol::toString(ElfSymBinding val)
{
#ifndef _MSC_VER
    return stringifySgAsmElfSymbolElfSymBinding(val);
#else
    ROSE_ABORT();
#endif
}

std::string
SgAsmElfSymbol::to_string(ElfSymType val)
{
    return toString(val);
}

std::string
SgAsmElfSymbol::toString(ElfSymType val)
{
#ifndef _MSC_VER
    return stringifySgAsmElfSymbolElfSymType(val);
#else
    ROSE_ABORT();
#endif
}  

SgAsmElfSymbol::ElfSymBinding
SgAsmElfSymbol::get_elf_binding() const
{
    return get_elfBinding();
}

SgAsmElfSymbol::ElfSymBinding
SgAsmElfSymbol::get_elfBinding() const
{
    return (ElfSymBinding)(p_st_info >> 4);
}

SgAsmElfSymbol::ElfSymType
SgAsmElfSymbol::get_elf_type() const
{
    return get_elfType();
}

SgAsmElfSymbol::ElfSymType
SgAsmElfSymbol::get_elfType() const
{
    return (ElfSymType)(p_st_info & 0xf);
}

void *
SgAsmElfSymbol::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf32SymbolEntry_disk *disk) const
{
    Rose::BinaryAnalysis::Address st_name = p_name->get_offset();
    ROSE_ASSERT(st_name!=SgAsmGenericString::unallocated);
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, st_name,     &(disk->st_name));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_st_info,   &(disk->st_info));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_st_res1,   &(disk->st_res1));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_st_shndx,  &(disk->st_shndx));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_st_size,   &(disk->st_size));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_value(), &(disk->st_value));
    return disk;
}
void *
SgAsmElfSymbol::encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, Elf64SymbolEntry_disk *disk) const
{
    Rose::BinaryAnalysis::Address st_name = p_name->get_offset();
    ROSE_ASSERT(st_name!=SgAsmGenericString::unallocated);
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, st_name,     &(disk->st_name));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_st_info,   &(disk->st_info));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_st_res1,   &(disk->st_res1));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_st_shndx,  &(disk->st_shndx));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, p_st_size,   &(disk->st_size));
    Rose::BinaryAnalysis::ByteOrder::hostToDisk(sex, get_value(), &(disk->st_value));
    return disk;
}

void
SgAsmElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmGenericSection *section) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sElfSymbol[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sElfSymbol.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    SgAsmGenericSymbol::dump(f, p, -1);

    fprintf(f, "%s%-*s = %u",          p, w, "st_info",  p_st_info);
    fprintf(f, " (%s %s)\n",toString(get_elfBinding()).c_str(),toString(get_elfType()).c_str());
    fprintf(f, "%s%-*s = %u\n",         p, w, "st_res1", p_st_res1);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",  p, w, "st_size", p_st_size);

    if (section && section->get_id() == (int)p_st_shndx) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "st_shndx", section->get_id(), section->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = %u\n",         p, w, "st_shndx", p_st_shndx);        
    }

    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %" PRIuPTR " bytes\n", p, w, "extra", p_extra.size());
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

SgAsmElfSymbolSection::SgAsmElfSymbolSection(SgAsmElfFileHeader *fhdr, SgAsmElfStringSection *strings)
    : SgAsmElfSection(fhdr) {
    initializeProperties();
    ASSERT_not_null(strings);
    set_linkedSection(strings);
}

SgAsmElfSymbolSection *
SgAsmElfSymbolSection::parse()
{
    SgAsmElfSection::parse();

    SgAsmElfFileHeader *fhdr = get_elfHeader();
    ROSE_ASSERT(fhdr!=NULL);
    SgAsmElfSectionTableEntry *shdr = get_sectionEntry();
    ASSERT_always_require(shdr!=NULL);

    size_t entry_size, struct_size, extra_size, nentries;
    calculateSizes(&entry_size, &struct_size, &extra_size, &nentries);
    ROSE_ASSERT(entry_size==shdr->get_sh_entsize());

    /* Parse each entry */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfSymbol *entry=0;
        if (4==fhdr->get_wordSize()) {
            entry = new SgAsmElfSymbol(this); /*adds symbol to this symbol table*/
            SgAsmElfSymbol::Elf32SymbolEntry_disk disk;
            readContentLocal(i*entry_size, &disk, struct_size);
            entry->parse(fhdr->get_sex(), &disk);
        } else if (8==fhdr->get_wordSize()) {
            entry = new SgAsmElfSymbol(this); /*adds symbol to this symbol table*/
            SgAsmElfSymbol::Elf64SymbolEntry_disk disk;
            readContentLocal(i*entry_size, &disk, struct_size);
            entry->parse(fhdr->get_sex(), &disk);
        } else {
            throw FormatError("unsupported ELF word size");
        }
        if (extra_size>0)
            entry->get_extra() = readContentLocalUcl(i*entry_size+struct_size, extra_size);
    }
    return this;
}

Rose::BinaryAnalysis::Address
SgAsmElfSymbolSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    return calculateSizes(entsize, required, optional, entcount);
}

Rose::BinaryAnalysis::Address
SgAsmElfSymbolSection::calculateSizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    std::vector<size_t> extra_sizes;
    for (size_t i=0; i<p_symbols->get_symbols().size(); i++)
        extra_sizes.push_back(p_symbols->get_symbols()[i]->get_extra().size());
    return calculateSizes(sizeof(SgAsmElfSymbol::Elf32SymbolEntry_disk),
                           sizeof(SgAsmElfSymbol::Elf64SymbolEntry_disk),
                           extra_sizes,
                           entsize, required, optional, entcount);
}

void
SgAsmElfSymbolSection::finish_parsing()
{
    finishParsing();
}

void
SgAsmElfSymbolSection::finishParsing()
{
    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmElfSymbol *symbol = p_symbols->get_symbols()[i];

        /* Get bound section ptr */
        if (symbol->get_st_shndx() > 0 && symbol->get_st_shndx() < 0xff00) {
            if (SgAsmGenericSection *bound = get_file()->get_sectionById(symbol->get_st_shndx()))
                symbol->set_bound(bound);
        }
    }
}

size_t
SgAsmElfSymbolSection::index_of(SgAsmElfSymbol *symbol)
{
    return indexOf(symbol);
}

size_t
SgAsmElfSymbolSection::indexOf(SgAsmElfSymbol *symbol)
{
    for (size_t i=0; i<p_symbols->get_symbols().size(); i++) {
        if (p_symbols->get_symbols()[i]==symbol)
            return i;
    }
    throw FormatError("symbol is not in symbol table");
}

bool
SgAsmElfSymbolSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();

    /* Update parts of the section and segment tables not updated by superclass */
    SgAsmElfSectionTableEntry *secent = get_sectionEntry();
    if (secent)
        secent->set_sh_type(get_isDynamic() ?
                            SgAsmElfSectionTableEntry::SHT_DYNSYM :
                            SgAsmElfSectionTableEntry::SHT_SYMTAB);
    return reallocated;
}

void
SgAsmElfSymbolSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elfHeader();
    ROSE_ASSERT(fhdr);
    Rose::BinaryAnalysis::ByteOrder::Endianness sex = fhdr->get_sex();

    size_t entry_size, struct_size, extra_size, nentries;
    calculateSizes(&entry_size, &struct_size, &extra_size, &nentries);
    
    /* Adjust the entry size stored in the ELF Section Table */
    get_sectionEntry()->set_sh_entsize(entry_size);

    /* Write each entry's required part followed by the optional part */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfSymbol::Elf32SymbolEntry_disk disk32;
        SgAsmElfSymbol::Elf64SymbolEntry_disk disk64;
        void *disk=NULL;

        SgAsmElfSymbol *entry = p_symbols->get_symbols()[i];
        
        if (4==fhdr->get_wordSize()) {
            disk = entry->encode(sex, &disk32);
        } else if (8==fhdr->get_wordSize()) {
            disk = entry->encode(sex, &disk64);
        } else {
            ROSE_ASSERT(!"unsupported word size");
        }

        Rose::BinaryAnalysis::Address spos = i * entry_size;
        spos = write(f, spos, struct_size, disk);
        if (entry->get_extra().size()>0) {
            ROSE_ASSERT(entry->get_extra().size()<=extra_size);
            write(f, spos, entry->get_extra());
        }
    }

    unparseHoles(f);
}

void
SgAsmElfSymbolSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        snprintf(p, sizeof(p), "%sElfSymbolSection[%zd].", prefix, idx);
    } else {
        snprintf(p, sizeof(p), "%sElfSymbolSection.", prefix);
    }
    const int w = std::max(size_t{1}, DUMP_FIELD_WIDTH - strlen(p));

    SgAsmElfSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %s\n", p, w, "is_dynamic", get_isDynamic() ? "yes" : "no");
    fprintf(f, "%s%-*s = %" PRIuPTR " symbols\n", p, w, "ElfSymbol.size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmGenericSection *section = get_file()->get_sectionById(p_symbols->get_symbols()[i]->get_st_shndx());
        p_symbols->get_symbols()[i]->dump(f, p, i, section);
    }

    if (variantT() == V_SgAsmElfSymbolSection) //unless a base class
        Rose::BinaryAnalysis::hexdump(f, 0, std::string(p)+"data at ", p_data);
}

bool
SgAsmElfSymbolSection::get_is_dynamic() const {
    return get_isDynamic();
}

void
SgAsmElfSymbolSection::set_is_dynamic(bool x) {
    set_isDynamic(x);
}

#endif
