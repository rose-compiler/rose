/* ELF Symbol Tables (SgAsmElfSymbolSection and related classes) */
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "stringify.h"

// In order to efficiently (in terms of amount of code) parse a file format that's defined for a different architecture, we
// need to occassionally take addresses of structs that don't follow alignment rules for this architecture.
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"

using namespace Rose;

void
SgAsmElfSymbol::ctor(SgAsmElfSymbolSection *symtab)
{
    ROSE_ASSERT(symtab!=NULL);
    SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(symtab->get_linked_section());
    ROSE_ASSERT(strsec!=NULL);
    
    set_name(new SgAsmStoredString(strsec->get_strtab(), 0));

    ROSE_ASSERT(symtab->get_symbols()!=NULL);
    symtab->get_symbols()->get_symbols().push_back(this);
    ROSE_ASSERT(symtab->get_symbols()->get_symbols().size()>0);
    set_parent(symtab->get_symbols());

    set_st_info(0);
    set_st_res1(0);
    set_st_shndx(0);
    set_st_size(0);
}

void
SgAsmElfSymbol::parse(ByteOrder::Endianness sex, const Elf32SymbolEntry_disk *disk)
{
    p_st_info  = ByteOrder::disk_to_host(sex, disk->st_info);
    p_st_res1  = ByteOrder::disk_to_host(sex, disk->st_res1);
    p_st_shndx = ByteOrder::disk_to_host(sex, disk->st_shndx);
    p_st_size  = ByteOrder::disk_to_host(sex, disk->st_size);

    p_value    = ByteOrder::disk_to_host(sex, disk->st_value);
    p_size     = p_st_size;

    rose_addr_t name_offset  = ByteOrder::disk_to_host(sex, disk->st_name);
    get_name()->set_string(name_offset);

    parse_common();
}

void
SgAsmElfSymbol::parse(ByteOrder::Endianness sex, const Elf64SymbolEntry_disk *disk)
{
    p_st_info  = ByteOrder::disk_to_host(sex, disk->st_info);
    p_st_res1  = ByteOrder::disk_to_host(sex, disk->st_res1);
    p_st_shndx = ByteOrder::disk_to_host(sex, disk->st_shndx);
    p_st_size  = ByteOrder::disk_to_host(sex, disk->st_size);

    p_value    = ByteOrder::disk_to_host(sex, disk->st_value);
    p_size     = p_st_size;

    rose_addr_t name_offset  = ByteOrder::disk_to_host(sex, disk->st_name);
    get_name()->set_string(name_offset);

    parse_common();
}

void
SgAsmElfSymbol::parse_common()
{
    /* Binding */
    switch (get_elf_binding()) {
        case STB_LOCAL:   p_binding = SYM_LOCAL;      break;
        case STB_GLOBAL:  p_binding = SYM_GLOBAL;     break;
        case STB_WEAK:    p_binding = SYM_WEAK;       break;
        default:          p_binding = SYM_NO_BINDING; break;
    }

    /* Type */
    switch (get_elf_type()) {
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
        p_def_state = SYM_DEFINED;
    } else if (p_name->get_string().size() > 0 || get_elf_type()) {
        p_def_state = SYM_TENTATIVE;
    } else {
        p_def_state = SYM_UNDEFINED;
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
#ifndef _MSC_VER
    return stringifySgAsmElfSymbolElfSymBinding(val);
#else
    ROSE_ASSERT(false);
    return "";
#endif
}

std::string
SgAsmElfSymbol::to_string(ElfSymType val)
{
#ifndef _MSC_VER
    return stringifySgAsmElfSymbolElfSymType(val);
#else
    ROSE_ASSERT(false);
    return "";
#endif
}  


SgAsmElfSymbol::ElfSymBinding
SgAsmElfSymbol::get_elf_binding() const
{
    return (ElfSymBinding)(p_st_info >> 4);
}

SgAsmElfSymbol::ElfSymType
SgAsmElfSymbol::get_elf_type() const
{
    return (ElfSymType)(p_st_info & 0xf);
}

void *
SgAsmElfSymbol::encode(ByteOrder::Endianness sex, Elf32SymbolEntry_disk *disk) const
{
    rose_addr_t st_name = p_name->get_offset();
    ROSE_ASSERT(st_name!=SgAsmGenericString::unallocated);
    ByteOrder::host_to_disk(sex, st_name,     &(disk->st_name));
    ByteOrder::host_to_disk(sex, p_st_info,   &(disk->st_info));
    ByteOrder::host_to_disk(sex, p_st_res1,   &(disk->st_res1));
    ByteOrder::host_to_disk(sex, p_st_shndx,  &(disk->st_shndx));
    ByteOrder::host_to_disk(sex, p_st_size,   &(disk->st_size));
    ByteOrder::host_to_disk(sex, get_value(), &(disk->st_value));
    return disk;
}
void *
SgAsmElfSymbol::encode(ByteOrder::Endianness sex, Elf64SymbolEntry_disk *disk) const
{
    rose_addr_t st_name = p_name->get_offset();
    ROSE_ASSERT(st_name!=SgAsmGenericString::unallocated);
    ByteOrder::host_to_disk(sex, st_name,     &(disk->st_name));
    ByteOrder::host_to_disk(sex, p_st_info,   &(disk->st_info));
    ByteOrder::host_to_disk(sex, p_st_res1,   &(disk->st_res1));
    ByteOrder::host_to_disk(sex, p_st_shndx,  &(disk->st_shndx));
    ByteOrder::host_to_disk(sex, p_st_size,   &(disk->st_size));
    ByteOrder::host_to_disk(sex, get_value(), &(disk->st_value));
    return disk;
}

void
SgAsmElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmGenericSection *section) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericSymbol::dump(f, p, -1);

    fprintf(f, "%s%-*s = %u",          p, w, "st_info",  p_st_info);
    fprintf(f, " (%s %s)\n",to_string(get_elf_binding()).c_str(),to_string(get_elf_type()).c_str());
    fprintf(f, "%s%-*s = %u\n",         p, w, "st_res1", p_st_res1);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",  p, w, "st_size", p_st_size);

    if (section && section->get_id() == (int)p_st_shndx) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "st_shndx", section->get_id(), section->get_name()->get_string(true).c_str());
    } else {
        fprintf(f, "%s%-*s = %u\n",         p, w, "st_shndx", p_st_shndx);        
    }

    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %" PRIuPTR " bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

void
SgAsmElfSymbolSection::ctor(SgAsmElfStringSection *strings)
{
    p_symbols = new SgAsmElfSymbolList;
    p_symbols->set_parent(this);
    ROSE_ASSERT(strings!=NULL);
    p_linked_section = strings;
}

SgAsmElfSymbolSection *
SgAsmElfSymbolSection::parse()
{
    SgAsmElfSection::parse();

    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr!=NULL);
    SgAsmElfSectionTableEntry *shdr = get_section_entry();
    ROSE_ASSERT(shdr!=NULL);
    SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(get_linked_section());
    ROSE_ASSERT(strsec!=NULL);

    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);
    ROSE_ASSERT(entry_size==shdr->get_sh_entsize());

    /* Parse each entry */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfSymbol *entry=0;
        if (4==fhdr->get_word_size()) {
            entry = new SgAsmElfSymbol(this); /*adds symbol to this symbol table*/
            SgAsmElfSymbol::Elf32SymbolEntry_disk disk;
            read_content_local(i*entry_size, &disk, struct_size);
            entry->parse(fhdr->get_sex(), &disk);
        } else if (8==fhdr->get_word_size()) {
            entry = new SgAsmElfSymbol(this); /*adds symbol to this symbol table*/
            SgAsmElfSymbol::Elf64SymbolEntry_disk disk;
            read_content_local(i*entry_size, &disk, struct_size);
            entry->parse(fhdr->get_sex(), &disk);
        } else {
            throw FormatError("unsupported ELF word size");
        }
        if (extra_size>0)
            entry->get_extra() = read_content_local_ucl(i*entry_size+struct_size, extra_size);
    }
    return this;
}

rose_addr_t
SgAsmElfSymbolSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    std::vector<size_t> extra_sizes;
    for (size_t i=0; i<p_symbols->get_symbols().size(); i++)
        extra_sizes.push_back(p_symbols->get_symbols()[i]->get_extra().size());
    return calculate_sizes(sizeof(SgAsmElfSymbol::Elf32SymbolEntry_disk),
                           sizeof(SgAsmElfSymbol::Elf64SymbolEntry_disk),
                           extra_sizes,
                           entsize, required, optional, entcount);
}

void
SgAsmElfSymbolSection::finish_parsing()
{
    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmElfSymbol *symbol = p_symbols->get_symbols()[i];

        /* Get bound section ptr */
        if (symbol->get_st_shndx() > 0 && symbol->get_st_shndx() < 0xff00) {
            if (SgAsmGenericSection *bound = get_file()->get_section_by_id(symbol->get_st_shndx()))
                symbol->set_bound(bound);
        }
    }
}

size_t
SgAsmElfSymbolSection::index_of(SgAsmElfSymbol *symbol)
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
    SgAsmElfSectionTableEntry *secent = get_section_entry();
    if (secent)
        secent->set_sh_type(p_is_dynamic ?
                            SgAsmElfSectionTableEntry::SHT_DYNSYM :
                            SgAsmElfSectionTableEntry::SHT_SYMTAB);
    return reallocated;
}

void
SgAsmElfSymbolSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    ByteOrder::Endianness sex = fhdr->get_sex();

    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);
    
    /* Adjust the entry size stored in the ELF Section Table */
    get_section_entry()->set_sh_entsize(entry_size);

    /* Write each entry's required part followed by the optional part */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfSymbol::Elf32SymbolEntry_disk disk32;
        SgAsmElfSymbol::Elf64SymbolEntry_disk disk64;
        void *disk=NULL;

        SgAsmElfSymbol *entry = p_symbols->get_symbols()[i];
        
        if (4==fhdr->get_word_size()) {
            disk = entry->encode(sex, &disk32);
        } else if (8==fhdr->get_word_size()) {
            disk = entry->encode(sex, &disk64);
        } else {
            ROSE_ASSERT(!"unsupported word size");
        }

        rose_addr_t spos = i * entry_size;
        spos = write(f, spos, struct_size, disk);
        if (entry->get_extra().size()>0) {
            ROSE_ASSERT(entry->get_extra().size()<=extra_size);
            write(f, spos, entry->get_extra());
        }
    }

    unparse_holes(f);
}

void
SgAsmElfSymbolSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymbolSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymbolSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmElfSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %s\n", p, w, "is_dynamic", p_is_dynamic ? "yes" : "no");
    fprintf(f, "%s%-*s = %" PRIuPTR " symbols\n", p, w, "ElfSymbol.size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmGenericSection *section = get_file()->get_section_by_id(p_symbols->get_symbols()[i]->get_st_shndx());
        p_symbols->get_symbols()[i]->dump(f, p, i, section);
    }

    if (variantT() == V_SgAsmElfSymbolSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

#endif
