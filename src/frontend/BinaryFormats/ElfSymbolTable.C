/* ELF Symbol Tables (SgAsmElfSymbolSection and related classes) */
#include "sage3basic.h"
#include "stringify.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/** Adds the newly constructed symbol to the specified ELF Symbol Table. */
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

/** Initialize symbol by parsing a symbol table entry. An ELF String Section must be supplied in order to get the symbol name. */
void
SgAsmElfSymbol::parse(ByteOrder sex, const Elf32SymbolEntry_disk *disk)
{
    p_st_info  = disk_to_host(sex, disk->st_info);
    p_st_res1  = disk_to_host(sex, disk->st_res1);
    p_st_shndx = disk_to_host(sex, disk->st_shndx);
    p_st_size  = disk_to_host(sex, disk->st_size);

    p_value    = disk_to_host(sex, disk->st_value);
    p_size     = p_st_size;

    addr_t name_offset  = disk_to_host(sex, disk->st_name);
    get_name()->set_string(name_offset);

    parse_common();
}

/** Initialize symbol by parsing a symbol table entry. An ELF String Section must be supplied in order to get the symbol name. */
void
SgAsmElfSymbol::parse(ByteOrder sex, const Elf64SymbolEntry_disk *disk)
{
    p_st_info  = disk_to_host(sex, disk->st_info);
    p_st_res1  = disk_to_host(sex, disk->st_res1);
    p_st_shndx = disk_to_host(sex, disk->st_shndx);
    p_st_size  = disk_to_host(sex, disk->st_size);

    p_value    = disk_to_host(sex, disk->st_value);
    p_size     = p_st_size;

    addr_t name_offset  = disk_to_host(sex, disk->st_name);
    get_name()->set_string(name_offset);

    parse_common();
}

void
SgAsmElfSymbol::parse_common()
{
    /* Binding */
    switch (get_elf_binding()) {
      case STB_LOCAL:   p_binding = SYM_LOCAL;  break;
      case STB_GLOBAL:  p_binding = SYM_GLOBAL; break;
      case STB_WEAK:    p_binding = SYM_WEAK;   break;
      default:
        fprintf(stderr, "unknown elf symbol binding: %u\n", get_elf_binding());
        ROSE_ASSERT(0);
        break;
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
      default:
        fprintf(stderr, "unknown elf symbol type: %u\n", get_elf_type());
        ROSE_ASSERT(0);
        break;
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

/** Encode a symbol into disk format */
void *
SgAsmElfSymbol::encode(ByteOrder sex, Elf32SymbolEntry_disk *disk) const
{
    addr_t st_name = p_name->get_offset();
    ROSE_ASSERT(st_name!=SgAsmGenericString::unallocated);
    host_to_disk(sex, st_name,     &(disk->st_name));
    host_to_disk(sex, p_st_info,   &(disk->st_info));
    host_to_disk(sex, p_st_res1,   &(disk->st_res1));
    host_to_disk(sex, p_st_shndx,  &(disk->st_shndx));
    host_to_disk(sex, p_st_size,   &(disk->st_size));
    host_to_disk(sex, get_value(), &(disk->st_value));
    return disk;
}
void *
SgAsmElfSymbol::encode(ByteOrder sex, Elf64SymbolEntry_disk *disk) const
{
    addr_t st_name = p_name->get_offset();
    ROSE_ASSERT(st_name!=SgAsmGenericString::unallocated);
    host_to_disk(sex, st_name,     &(disk->st_name));
    host_to_disk(sex, p_st_info,   &(disk->st_info));
    host_to_disk(sex, p_st_res1,   &(disk->st_res1));
    host_to_disk(sex, p_st_shndx,  &(disk->st_shndx));
    host_to_disk(sex, p_st_size,   &(disk->st_size));
    host_to_disk(sex, get_value(), &(disk->st_value));
    return disk;
}

/** Print some debugging info. The 'section' is an optional section pointer for the st_shndx member. */
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
    fprintf(f, "%s%-*s = %"PRIu64"\n",  p, w, "st_size", p_st_size);

    if (section && section->get_id() == (int)p_st_shndx) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "st_shndx", section->get_id(), section->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = %u\n",         p, w, "st_shndx", p_st_shndx);        
    }

    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

/** Non-parsing constructor */
void
SgAsmElfSymbolSection::ctor(SgAsmElfStringSection *strings)
{
    p_symbols = new SgAsmElfSymbolList;
    p_symbols->set_parent(this);
    ROSE_ASSERT(strings!=NULL);
    p_linked_section = strings;
}

/** Initializes this ELF Symbol Section by parsing a file. */
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

/** Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. */
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

/** Update section pointers for locally-bound symbols since we know that the section table has been read and all
 *  non-synthesized sections have been created.
 * 
 *  The st_shndx is the index (ID) of the section to which the symbol is bound. Special values are:
 *   0x0000        no section (section table entry zero should be all zeros anyway)
 *   0xff00-0xffff reserved values, not an index
 *   0xff00-0xff1f processor specific values
 *   0xfff1        symbol has absolute value not affected by relocation
 *   0xfff2        symbol is fortran common or unallocated C extern */
void
SgAsmElfSymbolSection::finish_parsing()
{
    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmElfSymbol *symbol = p_symbols->get_symbols()[i];

        /* Get bound section ptr */
        if (symbol->get_st_shndx() > 0 && symbol->get_st_shndx() < 0xff00) {
            SgAsmGenericSection *bound = get_file()->get_section_by_id(symbol->get_st_shndx());
            ROSE_ASSERT(bound != NULL);
            symbol->set_bound(bound);
        }
    }
}

/** Given a symbol, return its index in this symbol table. */
size_t
SgAsmElfSymbolSection::index_of(SgAsmElfSymbol *symbol)
{
    for (size_t i=0; i<p_symbols->get_symbols().size(); i++) {
        if (p_symbols->get_symbols()[i]==symbol)
            return i;
    }
    throw FormatError("symbol is not in symbol table");
}

/** Called prior to unparsing. Updates symbol entries with name offsets */
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

/** Write symbol table sections back to disk */
void
SgAsmElfSymbolSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    ByteOrder sex = fhdr->get_sex();

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

        addr_t spos = i * entry_size;
        spos = write(f, spos, struct_size, disk);
        if (entry->get_extra().size()>0) {
            ROSE_ASSERT(entry->get_extra().size()<=extra_size);
            write(f, spos, entry->get_extra());
        }
    }

    unparse_holes(f);
}

/** Print some debugging info */
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
    fprintf(f, "%s%-*s = %zu symbols\n", p, w, "ElfSymbol.size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmGenericSection *section = get_file()->get_section_by_id(p_symbols->get_symbols()[i]->get_st_shndx());
        p_symbols->get_symbols()[i]->dump(f, p, i, section);
    }

    if (variantT() == V_SgAsmElfSymbolSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
