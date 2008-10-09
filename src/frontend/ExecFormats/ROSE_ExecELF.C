/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
    
/* Truncate an address, ADDR, to be a multiple of the alignment, ALMNT, where ALMNT is a power of two and of the same
 * unsigned datatype as the address. */
#define ALIGN(ADDR,ALMNT) ((ADDR) & ~((ALMNT)-1))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File headers
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor reads and decodes the ELF header, whether it's 32- or 64-bit.  The 'offset' argument is normally zero since
 * ELF headers are at the beginning of the file. As mentioned in the header file, the section size is initialized as if we had
 * 32-bit words and if necessary we extend the section for 64-bit words herein. */
void
SgAsmElfFileHeader::ctor(SgAsmGenericFile *f, addr_t offset)
{
    set_name(new SgAsmBasicString("ELF File Header"));
    set_synthesized(true);
    set_purpose(SP_HEADER);

    ROSE_ASSERT(f != NULL);
    // DQ (8/16/2008): Added code to set SgAsmPEFileHeader as parent of input SgAsmGenericFile
    f->set_parent(this);

    /* Read 32-bit header for now. Might need to re-read as 64-bit later. */
    ROSE_ASSERT(0 == get_size());
    Elf32FileHeader_disk disk32;
    extend_up_to(sizeof(disk32));
    content(0, sizeof(disk32), &disk32);

    ROSE_ASSERT(p_exec_format != NULL);

    /* Check magic number early */
    if (disk32.e_ident_magic[0]!=0x7f || disk32.e_ident_magic[1]!='E' ||
        disk32.e_ident_magic[2]!='L'  || disk32.e_ident_magic[3]!='F')
        throw FormatError("Bad ELF magic number");

    /* File byte order should be 1 or 2. However, we've seen at least one example that left the byte order at zero, implying
     * that it was the native order. We don't have the luxury of decoding the file on the native machine, so in that case we
     * try to infer the byte order by looking at one of the other multi-byte fields of the file. */
    ByteOrder sex;
    if (1 == disk32.e_ident_data_encoding) {
        sex = ORDER_LSB;
    } else if (2==disk32.e_ident_data_encoding) {
        sex = ORDER_MSB;
    } else if ((disk32.e_type & 0xff00)==0xff00) {
        /* One of the 0xffxx processor-specific flags in native order */
        if ((disk32.e_type & 0x00ff)==0xff)
            throw FormatError("invalid ELF header byte order"); /*ambiguous*/
        sex = host_order();
    } else if ((disk32.e_type & 0x00ff)==0x00ff) {
        /* One of the 0xffxx processor specific orders in reverse native order */
        sex = host_order()==ORDER_LSB ? ORDER_MSB : ORDER_LSB;
    } else if ((disk32.e_type & 0xff00)==0) {
        /* One of the low-valued file types in native order */
        if ((disk32.e_type & 0x00ff)==0)
            throw FormatError("invalid ELF header byte order"); /*ambiguous*/
        sex = host_order();
    } else if ((disk32.e_type & 0x00ff)==0) {
        /* One of the low-valued file types in reverse native order */
        sex = host_order() == ORDER_LSB ? ORDER_MSB : ORDER_LSB;
    } else {
        /* Ambiguous order */
        throw FormatError("invalid ELF header byte order");
    }
    ROSE_ASSERT(p_exec_format != NULL);
    p_exec_format->set_sex(sex);

    /* Decode header to native format */
    addr_t entry_rva;
    if (1 == disk32.e_ident_file_class) {
        p_exec_format->set_word_size(4);

	ROSE_ASSERT(0==p_e_ident_padding.size());
        for (size_t i=0; i<sizeof(disk32.e_ident_padding); i++)
             p_e_ident_padding.push_back(disk32.e_ident_padding[i]);

        p_e_ident_file_class    = disk_to_host(sex, disk32.e_ident_file_class);
        p_e_ident_file_version  = disk_to_host(sex, disk32.e_ident_file_version);
        p_e_type                = disk_to_host(sex, disk32.e_type);
        p_e_machine             = disk_to_host(sex, disk32.e_machine);
	p_exec_format->set_version(disk_to_host(sex, disk32.e_version));
        entry_rva               = disk_to_host(sex, disk32.e_entry);
        p_e_phoff               = disk_to_host(sex, disk32.e_phoff);
        p_e_shoff               = disk_to_host(sex, disk32.e_shoff);
        p_e_flags               = disk_to_host(sex, disk32.e_flags);
        p_e_ehsize              = disk_to_host(sex, disk32.e_ehsize);

        p_e_phnum               = disk_to_host(sex, disk32.e_phnum);
        if (p_e_phnum>0) {
            p_phextrasz         = disk_to_host(sex, disk32.e_phentsize);
            ROSE_ASSERT(p_phextrasz>=sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk));
            p_phextrasz -= sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk);
        } else {
            p_phextrasz = 0;
        }

        p_e_shnum               = disk_to_host(sex, disk32.e_shnum);
        if (p_e_shnum>0) {
            p_shextrasz         = disk_to_host(sex, disk32.e_shentsize);
            ROSE_ASSERT(p_shextrasz>=sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk));
            p_shextrasz -= sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk);
        } else {
            p_shextrasz = 0;
        }

        p_e_shstrndx            = disk_to_host(sex, disk32.e_shstrndx);
    } else if (2 == disk32.e_ident_file_class) {
        /* We guessed wrong. This is a 64-bit header, not 32-bit. */
        p_exec_format->set_word_size(8);
        Elf64FileHeader_disk disk64;
        extend_up_to(sizeof(Elf64FileHeader_disk)-sizeof(Elf32FileHeader_disk));
        content(0, sizeof disk64, &disk64);

	ROSE_ASSERT(0==p_e_ident_padding.size());
        for (size_t i=0; i<sizeof(disk64.e_ident_padding); i++)
             p_e_ident_padding.push_back(disk64.e_ident_padding[i]);

        p_e_ident_file_class    = disk_to_host(sex, disk64.e_ident_file_class);
        p_e_ident_file_version  = disk_to_host(sex, disk64.e_ident_file_version);
        p_e_type                = disk_to_host(sex, disk64.e_type);
        p_e_machine             = disk_to_host(sex, disk64.e_machine);
	p_exec_format->set_version(disk_to_host(sex, disk64.e_version));
        entry_rva               = disk_to_host(sex, disk64.e_entry);
        p_e_phoff               = disk_to_host(sex, disk64.e_phoff);
        p_e_shoff               = disk_to_host(sex, disk64.e_shoff);
        p_e_flags               = disk_to_host(sex, disk64.e_flags);
        p_e_ehsize              = disk_to_host(sex, disk64.e_ehsize);

        p_e_phnum               = disk_to_host(sex, disk64.e_phnum);
        if (p_e_phnum>0) {
            p_phextrasz         = disk_to_host(sex, disk64.e_phentsize);
            ROSE_ASSERT(p_phextrasz>=sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk));
            p_phextrasz -= sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk);
        } else {
            p_phextrasz = 0;
        }

        p_e_shnum               = disk_to_host(sex, disk64.e_shnum);
        if (p_e_shnum>0) {
            p_shextrasz         = disk_to_host(sex, disk64.e_shentsize);
            ROSE_ASSERT(p_shextrasz>=sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk));
            p_shextrasz -= sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk);
        } else {
            p_shextrasz = 0;
        }

        p_e_shstrndx            = disk_to_host(sex, disk64.e_shstrndx);
    } else {
        throw FormatError("invalid ELF header file class");
    }
    
    /* Magic number. disk32 and disk64 have header bytes at same offset */
    for (size_t i=0; i<sizeof(disk32.e_ident_magic); i++)
        p_magic.push_back(disk32.e_ident_magic[i]);
    
    /* File format */
    p_exec_format->set_family(FAMILY_ELF);
    switch (p_e_type) {
      case 0:
        p_exec_format->set_purpose(PURPOSE_UNSPECIFIED);
        break;
      case 1:
      case 3:
        p_exec_format->set_purpose(PURPOSE_LIBRARY);
        break;
      case 2:
        p_exec_format->set_purpose(PURPOSE_EXECUTABLE);
        break;
      case 4:
        p_exec_format->set_purpose(PURPOSE_CORE_DUMP);
        break;
      default:
        if (p_e_type >= 0xff00 && p_e_type <= 0xffff) {
            p_exec_format->set_purpose(PURPOSE_PROC_SPECIFIC);
        } else {
            p_exec_format->set_purpose(PURPOSE_OTHER);
        }
        break;
    }
    p_exec_format->set_is_current_version(1 == p_exec_format->get_version());
    p_exec_format->set_abi(ABI_UNSPECIFIED);                 /* ELF specifies a target architecture rather than an ABI */
    p_exec_format->set_abi_version(0);

    /* Target architecture */
    switch (p_e_machine) {                                /* These come from the Portable Formats Specification v1.1 */
      case 0:
        set_isa(ISA_UNSPECIFIED);
        break;
      case 1:
        set_isa(ISA_ATT_WE_32100);
        break;
      case 2:
        set_isa(ISA_SPARC_Family);
        break;
      case 3:
        set_isa(ISA_IA32_386);
        break;
      case 4:
        set_isa(ISA_M68K_Family);
        break;
      case 5:
        set_isa(ISA_M88K_Family);
        break;
      case 7:
        set_isa(ISA_I860_Family);
        break;
      case 8:
        set_isa(ISA_MIPS_Family);
        break;
      case 40:
        set_isa(ISA_ARM_Family);
        break;
      case 62:
        set_isa(ISA_X8664_Family);
        break;
      default:
        /*FIXME: There's a whole lot more. See Dan's Elf reader. */
        set_isa(ISA_OTHER);
        break;
    }

    /* Target architecture */
    /*FIXME*/

    /* Entry point. We will eventually bind the entry point to a particular section (in SgAsmElfFileHeader::parse) so that if
     * sections are rearranged, extended, etc. the entry point will be updated automatically. */
    p_base_va = 0;
    add_entry_rva(entry_rva);
}

/* Maximum page size according to the ABI. This is used by the loader when calculating the program base address. Since parts
 * of the file are mapped into the process address space those parts must be aligned (both in the file and in memory) on the
 * largest possible page boundary so that any smaller page boundary will also work correctly. */
uint64_t
SgAsmElfFileHeader::max_page_size()
{
    /* FIXME:
     *    System V max page size is 4k.
     *    IA32 is 4k
     *    x86_64 is 2MB
     * Other systems may vary! */
    return 4*1024;
}

/* Override ROSETTA version of accessors so the set_* functions check that we're still parsing. It should not be possible to
 * modify these values after parsing. */
void
SgAsmElfFileHeader::set_e_shoff(addr_t addr)
{
    ROSE_ASSERT(!get_congealed()); /*must be still parsing*/
    p_e_shoff = addr;
}
void
SgAsmElfFileHeader::set_e_phoff(addr_t addr)
{
    ROSE_ASSERT(!get_congealed()); /*must be still parsing*/
    p_e_phoff = addr;
}

/* Encode Elf header disk structure */
void *
SgAsmElfFileHeader::encode(ByteOrder sex, Elf32FileHeader_disk *disk)
{
    ROSE_ASSERT(p_magic.size() == NELMTS(disk->e_ident_magic));
    for (size_t i=0; i<NELMTS(disk->e_ident_magic); i++)
        disk->e_ident_magic[i] = p_magic[i];

    unsigned ident_file_class=1;
    switch(get_word_size()) {
    case 4:
      ident_file_class = 1;
      break;
    case 8:
      ident_file_class = 2;
      break;
    default:
      ROSE_ASSERT(!"invalid word size");
      break;
    }
    host_to_disk(sex, ident_file_class, &(disk->e_ident_file_class));

    unsigned data_encoding = ORDER_LSB==get_sex() ? 1 : 2;
    host_to_disk(sex, data_encoding, &(disk->e_ident_data_encoding));
    host_to_disk(sex, p_e_ident_file_version,  &(disk->e_ident_file_version));
    ROSE_ASSERT(p_e_ident_padding.size() == NELMTS(disk->e_ident_padding));
    for (size_t i=0; i<NELMTS(disk->e_ident_padding); i++)
        disk->e_ident_padding[i] = p_e_ident_padding[i];
    host_to_disk(sex, p_e_type,                &(disk->e_type));
    host_to_disk(sex, p_e_machine,             &(disk->e_machine));
    host_to_disk(sex, p_exec_format->get_version(), &(disk->e_version));
    host_to_disk(sex, get_entry_rva(),         &(disk->e_entry));
    host_to_disk(sex, p_e_phoff,               &(disk->e_phoff));
    host_to_disk(sex, p_e_shoff,               &(disk->e_shoff));
    host_to_disk(sex, p_e_flags,               &(disk->e_flags));
    host_to_disk(sex, p_e_ehsize,              &(disk->e_ehsize));

    if (p_e_phnum>0) {
        host_to_disk(sex, p_phextrasz+sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk), &(disk->e_phentsize));
    } else {
        host_to_disk(sex, 0, &(disk->e_phentsize));
    }
    if (p_e_shnum>0) {
        host_to_disk(sex, p_shextrasz+sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk), &(disk->e_shentsize));
    } else {
        host_to_disk(sex, 0, &(disk->e_shentsize));
    }
    host_to_disk(sex, p_e_phnum,               &(disk->e_phnum));
    host_to_disk(sex, p_e_shnum,               &(disk->e_shnum));
    host_to_disk(sex, p_e_shstrndx,            &(disk->e_shstrndx));

    return disk;
}
void *
SgAsmElfFileHeader::encode(ByteOrder sex, Elf64FileHeader_disk *disk)
{
    ROSE_ASSERT(p_magic.size() == NELMTS(disk->e_ident_magic));
    for (size_t i=0; i < NELMTS(disk->e_ident_magic); i++)
        disk->e_ident_magic[i] = p_magic[i];

    unsigned ident_file_class=1;
    switch(get_word_size()) {
    case 4:
      ident_file_class = 1;
      break;
    case 8:
      ident_file_class = 2;
      break;
    default:
      ROSE_ASSERT(!"invalid word size");
      break;
    }
    host_to_disk(sex, ident_file_class, &(disk->e_ident_file_class));

    unsigned data_encoding = ORDER_LSB==get_sex() ? 1 : 2;
    host_to_disk(sex, data_encoding, &(disk->e_ident_data_encoding));
    host_to_disk(sex, p_e_ident_file_version,  &(disk->e_ident_file_version));
    ROSE_ASSERT(p_e_ident_padding.size() == NELMTS(disk->e_ident_padding));
    for (size_t i=0; i<NELMTS(disk->e_ident_padding); i++)
        disk->e_ident_padding[i] = p_e_ident_padding[i];
    host_to_disk(sex, p_e_type,                &(disk->e_type));
    host_to_disk(sex, p_e_machine,             &(disk->e_machine));
    host_to_disk(sex, p_exec_format->get_version(), &(disk->e_version));
    host_to_disk(sex, get_entry_rva(),         &(disk->e_entry));
    host_to_disk(sex, p_e_phoff,               &(disk->e_phoff));
    host_to_disk(sex, p_e_shoff,               &(disk->e_shoff));
    host_to_disk(sex, p_e_flags,               &(disk->e_flags));
    host_to_disk(sex, p_e_ehsize,              &(disk->e_ehsize));
    host_to_disk(sex, p_phextrasz+sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk), &(disk->e_phentsize));
    host_to_disk(sex, p_e_phnum,               &(disk->e_phnum));
    host_to_disk(sex, p_shextrasz+sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk), &(disk->e_shentsize));
    host_to_disk(sex, p_e_shnum,               &(disk->e_shnum));
    host_to_disk(sex, p_e_shstrndx,            &(disk->e_shstrndx));

    return disk;
}

/* Write ELF contents back to a file. */
void
SgAsmElfFileHeader::unparse(FILE *f)
{
    /* Allow sections to reallocate themselves until things settle */
    while (reallocate()) /*void*/;

    /* Write unreferenced areas back to the file before anything else. */
    unparse_holes(f);

    /* Write the ELF segment table and segments first since they generally overlap with more specific things which may have
     * been modified when walking the AST. (We generally don't modify segments, just the more specific sections.) */
    if (p_segment_table) {
        ROSE_ASSERT(p_segment_table->get_header()==this);
        p_segment_table->unparse(f);
        p_e_phoff = p_segment_table->get_offset();
    }

    /* Write the ELF section table and, indirectly, the sections themselves. */
    if (p_section_table) {
        ROSE_ASSERT(p_section_table->get_header()==this);
        p_section_table->unparse(f);
        p_e_shoff = p_section_table->get_offset();
    }
    
    /* Encode and write the ELF file header */
    Elf32FileHeader_disk disk32;
    Elf64FileHeader_disk disk64;
    void *disk = NULL;
    size_t struct_size = 0;
    if (4 == get_word_size()) {
        disk = encode(get_sex(), &disk32);
        struct_size = sizeof(disk32);
    } else if (8 == get_word_size()) {
        disk = encode(get_sex(), &disk64);
        struct_size = sizeof(disk64);
    } else {
        ROSE_ASSERT(!"unsupported word size");
    }
    write(f, p_offset, struct_size, disk);
}

/* Print some debugging info */
void
SgAsmElfFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfFileHeader.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_file_class",     p_e_ident_file_class);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_file_version",   p_e_ident_file_version);
    for (size_t i=0; i < p_e_ident_padding.size(); i++)
        fprintf(f, "%s%-*s = [%zu] %u\n",                   p, w, "e_ident_padding",     i, p_e_ident_padding[i]);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_type",                 p_e_type);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_machine",              p_e_machine);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes into file\n", p, w, "e_phoff",     p_e_phoff, p_e_phoff);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes into file\n", p, w, "e_shoff",     p_e_shoff, p_e_shoff);
    fprintf(f, "%s%-*s = 0x%08lx\n",                        p, w, "e_flags",                p_e_flags);
    fprintf(f, "%s%-*s = 0x%08lx (%lu) bytes\n",            p, w, "e_ehsize",               p_e_ehsize, p_e_ehsize);
    fprintf(f, "%s%-*s = 0x%08lx (%lu) bytes\n",            p, w, "phextrasz",              p_phextrasz, p_phextrasz);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_phnum",                p_e_phnum);
    fprintf(f, "%s%-*s = 0x%08lx (%lu) bytes\n",            p, w, "shextrasz",              p_shextrasz, p_shextrasz);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_shnum",                p_e_shnum);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_shstrndx",             p_e_shstrndx);

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sections
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor for sections defined in the ELF Section Table */
void
SgAsmElfSection::ctor(SgAsmElfSectionTableEntry *shdr)
{
    set_synthesized(false);

    /* Section purpose */
    switch (shdr->get_sh_type()) {
      case SgAsmElfSectionTableEntry::SHT_PROGBITS:
        set_purpose(SP_PROGRAM);
        break;
      case SgAsmElfSectionTableEntry::SHT_STRTAB:
        set_purpose(SP_HEADER);
        break;
      case SgAsmElfSectionTableEntry::SHT_DYNSYM:
      case SgAsmElfSectionTableEntry::SHT_SYMTAB:
        set_purpose(SP_SYMTAB);
        break;
      default:
        set_purpose(SP_OTHER);
        break;
    }

    /* Section mapping */
    if (shdr->get_sh_addr() > 0) {
        set_mapped_rva(shdr->get_sh_addr());
        set_mapped_size(shdr->get_sh_size());
        set_mapped_rperm(true);
    }
}

/* Constructor for sections defined in the ELF Segment Table */
void
SgAsmElfSection::ctor(SgAsmElfSegmentTableEntry *shdr)
{
    set_synthesized(false);
    
    char name[128];
    switch (shdr->get_type()) {
      case SgAsmElfSegmentTableEntry::PT_LOAD:         strcpy(name, "ELF Load");          break;
      case SgAsmElfSegmentTableEntry::PT_DYNAMIC:      strcpy(name, "ELF Dynamic");       break;
      case SgAsmElfSegmentTableEntry::PT_INTERP:       strcpy(name, "ELF Interpreter");   break;
      case SgAsmElfSegmentTableEntry::PT_NOTE:         strcpy(name, "ELF Note");          break;
      case SgAsmElfSegmentTableEntry::PT_SHLIB:        strcpy(name, "ELF Shlib");         break;
      case SgAsmElfSegmentTableEntry::PT_PHDR:         strcpy(name, "ELF Segment Table"); break;
      case SgAsmElfSegmentTableEntry::PT_GNU_EH_FRAME: strcpy(name, "GNU EH Frame");      break;
      case SgAsmElfSegmentTableEntry::PT_GNU_STACK:    strcpy(name, "GNU Stack");         break;
      case SgAsmElfSegmentTableEntry::PT_GNU_RELRO:    strcpy(name, "GNU Reloc RO");      break;
      case SgAsmElfSegmentTableEntry::PT_PAX_FLAGS:    strcpy(name, "PAX Flags");         break;
      case SgAsmElfSegmentTableEntry::PT_SUNWBSS:      strcpy(name, "Sun WBSS");          break;
      case SgAsmElfSegmentTableEntry::PT_SUNWSTACK:    strcpy(name, "Sun WStack");        break;
      default:                                         sprintf(name, "ELF type 0x%08x", shdr->get_type()); break;
    }
    sprintf(name+strlen(name), " (segment %zu)", shdr->get_index());
    set_name(new SgAsmBasicString(name));
    set_purpose(SP_HEADER);

    set_mapped_rva(shdr->get_vaddr());
    set_mapped_size(shdr->get_memsz());
    set_mapped_rperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? true : false);
    set_mapped_wperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? true : false);
    set_mapped_xperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? true : false);
}

/* Print some debugging info */
void
SgAsmElfSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSection.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    SgAsmGenericSection::dump(f, p, -1);
    
    if (get_section_entry())
        get_section_entry()->dump(f, p, -1);
    if (get_segment_entry())
        get_segment_entry()->dump(f, p, -1);

    if (p_linked_section) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n", p, w, "linked_to", p_linked_section->get_id(), p_linked_section->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = NULL\n",    p, w, "linked_to");
    }

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Table Sections
//
//    SgAsmElfStringSection is derived from SgAsmElfSection, which is derived in turn from SgAsmGenericSection. An ELF String
//    Table Section points to the ELF String Table (SgAsmElfStrtab) that is contained in the section.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmElfStringSection::ctor()
{
    p_strtab = new SgAsmElfStrtab(this);
}

/* Reallocate space for the string section if necessary */
bool
SgAsmElfStringSection::reallocate()
{
    return get_strtab()->reallocate();
}

/* Unparse an ElfStringSection by unparsing the ElfStrtab */
void
SgAsmElfStringSection::unparse(FILE *f)
{
    get_strtab()->unparse(f);
    unparse_holes(f);
}

/* Augments superclass to make sure free list and such are adjusted properly */
void
SgAsmElfStringSection::set_size(addr_t newsize)
{
    ROSE_ASSERT(newsize>=get_size()); /*can only enlarge for now*/
    addr_t orig_size = get_size();
    addr_t adjustment = newsize - orig_size;

    SgAsmElfSection::set_size(newsize);

    if (adjustment>0)
        get_strtab()->get_freelist().insert(orig_size, adjustment);
}

/* Print some debugging info */
void
SgAsmElfStringSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfStringSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfStringSection.", prefix);
    }
    
    SgAsmElfSection::dump(f, p, -1);

    ROSE_ASSERT(get_strtab()!=NULL);
    get_strtab()->dump(f, p, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Tables
//
//    An SgAsmElfStrtab is an ELF String Table, inheriting from SgAsmGenericStrtab. String tables point to the
//    SgAsmGenericSection that contains them. In the case of SgAsmElfStrtab it points to an SgAsmElfStringSection.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
SgAsmElfStrtab::ctor()
{
    /* The first byte of an ELF String Table should always be NUL. We don't want the allocation functions to ever free this
     * byte, so we'll create a special storage item for it. */
    if (get_container()->content(0, 1)[0]=='\0')
        p_dont_free = create_storage(0, false);
}

/* Free StringStorage objects associated with this string table. It may not be safe to blow them away yet since other objects
 * may still have SgAsmStoredStrings pointing to these storage objects. So instead, we will mark all this strtab's storage
 * objects as no longer being associated with a string table. This allows the SgAsmStoredString objects to still function
 * properly and their destructors (~SgAsmStoredString) will free their storage. */
SgAsmElfStrtab::~SgAsmElfStrtab()
{
    for (referenced_t::iterator i = p_storage_list.begin(); i != p_storage_list.end(); ++i) {
        SgAsmStringStorage *storage = *i;
        storage->set_strtab(NULL);
        storage->set_offset(SgAsmStoredString::unallocated);
    }
    p_storage_list.clear();
    p_dont_free = NULL; /*FIXME: can't delete for same reason as in SgAsmStoredString destructor. (RPM 2008-09-05) */
}

/* Creates the storage item for the string at the specified offset. If 'shared' is true then attempt to re-use a previous
 * storage object, otherwise always create a new one. Each storage object is considered a separate string, therefore when two
 * strings share the same storage object, changing one string changes the other. */
SgAsmStringStorage *
SgAsmElfStrtab::create_storage(addr_t offset, bool shared)
{
    ROSE_ASSERT(offset!=SgAsmStoredString::unallocated);

    /* Has this string already been created? If so, return previous storage object. However, never share the empty_string at
     * offset zero created when this string table was constructed because the ELF spec says it needs to stay there whether
     * referenced or not. */
    if (shared) {
        for (referenced_t::iterator i=p_storage_list.begin(); i!=p_storage_list.end(); i++) {
            if ((*i)->get_offset()==offset && (*i) != p_dont_free)
                return *i;
        }
    }

    /* Create a new storage object at this offset. */
    const char *s = get_container()->content_str(offset);
    SgAsmStringStorage *storage = new SgAsmStringStorage(this, s, offset);

    /* It's a bad idea to free (e.g., modify) strings before we've identified all the strings in the table. Consider
     * the case where offset 1 is "domain" and offset 3 is "main" (i.e., they overlap). If we modify "main" before knowing
     * about "domain" then we'll end up freeing the last part of "domain" (and possibly replacing it with something else)!
     *
     * The only time we can guarantee this is OK is when the new storage points to the same file location as "dont_free"
     * since the latter is guaranteed to never be freed or shared. This exception is used when creating a new, unallocated
     * string (see SgAsmStoredString(SgAsmGenericStrtab,const std::string&)). */
    if (p_num_freed>0 && (!p_dont_free || offset!=p_dont_free->get_offset())) {
        fprintf(stderr,
                "SgAsmElfStrtab::create_storage(%"PRIu64"): %zu other string%s (of %zu created) in [%d] \"%s\""
                " %s been modified and/or reallocated!\n",
                offset, p_num_freed, 1==p_num_freed?"":"s", p_storage_list.size(),
                get_container()->get_id(), get_container()->get_name()->c_str(),
                1==p_num_freed?"has":"have");
        ROSE_ASSERT(0==p_num_freed);
    }
    
    p_storage_list.push_back(storage);
    return storage;
}

/* Returns the number of bytes required to store the string in the string table. This is the length of the string plus
 * one for the NUL terminator. */
rose_addr_t
SgAsmElfStrtab::get_storage_size(const SgAsmStringStorage *storage) {
    return storage->get_string().size() + 1;
}

/* Write string table back to disk. Free space is zeroed out; holes are left as they are. */
void
SgAsmElfStrtab::unparse(FILE *f)
{
    SgAsmGenericSection *container = get_container();

    /*FIXME: What happens if the reallocation causes the string table to be resized at this point? (RPM 2008-09-03)*/
    addr_t orig_size = container->get_size();
    reallocate();
    ROSE_ASSERT(orig_size==container->get_size());
    
    /* Write strings with NUL termination. Shared strings will be written more than once, but that's OK. */
    for (size_t i=0; i<p_storage_list.size(); i++) {
        SgAsmStringStorage *storage = p_storage_list[i];
        ROSE_ASSERT(storage->get_offset()!=SgAsmStoredString::unallocated);
        addr_t at = container->write(f, storage->get_offset(), storage->get_string());
        container->write(f, at, '\0');
    }
    
    /* Fill free areas with zero */
    for (ExtentMap::const_iterator i=get_freelist().begin(); i!=get_freelist().end(); ++i) {
        container->write(f, i->first, std::string(i->second, '\0'));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Section tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Converts 32-bit disk representation to host representation */
void
SgAsmElfSectionTableEntry::ctor(ByteOrder sex, const Elf32SectionTableEntry_disk *disk) 
{
    p_sh_name      = disk_to_host(sex, disk->sh_name);
    p_sh_type      = disk_to_host(sex, disk->sh_type);
    p_sh_flags     = disk_to_host(sex, disk->sh_flags);
    p_sh_addr      = disk_to_host(sex, disk->sh_addr);
    p_sh_offset    = disk_to_host(sex, disk->sh_offset);
    p_sh_size      = disk_to_host(sex, disk->sh_size);
    p_sh_link      = disk_to_host(sex, disk->sh_link);
    p_sh_info      = disk_to_host(sex, disk->sh_info);
    p_sh_addralign = disk_to_host(sex, disk->sh_addralign);
    p_sh_entsize   = disk_to_host(sex, disk->sh_entsize);
}
    
/* Converts 64-bit disk representation to host representation */
void
SgAsmElfSectionTableEntry::ctor(ByteOrder sex, const Elf64SectionTableEntry_disk *disk) 
{
    p_sh_name      = disk_to_host(sex, disk->sh_name);
    p_sh_type      = disk_to_host(sex, disk->sh_type);
    p_sh_flags     = disk_to_host(sex, disk->sh_flags);
    p_sh_addr      = disk_to_host(sex, disk->sh_addr);
    p_sh_offset    = disk_to_host(sex, disk->sh_offset);
    p_sh_size      = disk_to_host(sex, disk->sh_size);
    p_sh_link      = disk_to_host(sex, disk->sh_link);
    p_sh_info      = disk_to_host(sex, disk->sh_info);
    p_sh_addralign = disk_to_host(sex, disk->sh_addralign);
    p_sh_entsize   = disk_to_host(sex, disk->sh_entsize);
}

/* Encode a section table entry into the disk structure */
void *
SgAsmElfSectionTableEntry::encode(ByteOrder sex, Elf32SectionTableEntry_disk *disk)
{
    host_to_disk(sex, p_sh_name,      &(disk->sh_name));
    host_to_disk(sex, p_sh_type,      &(disk->sh_type));
    host_to_disk(sex, p_sh_flags,     &(disk->sh_flags));
    host_to_disk(sex, p_sh_addr,      &(disk->sh_addr));
    host_to_disk(sex, p_sh_offset,    &(disk->sh_offset));
    host_to_disk(sex, p_sh_size,      &(disk->sh_size));
    host_to_disk(sex, p_sh_link,      &(disk->sh_link));
    host_to_disk(sex, p_sh_info,      &(disk->sh_info));
    host_to_disk(sex, p_sh_addralign, &(disk->sh_addralign));
    host_to_disk(sex, p_sh_entsize,   &(disk->sh_entsize));

    return disk;
}
void *
SgAsmElfSectionTableEntry::encode(ByteOrder sex, Elf64SectionTableEntry_disk *disk)
{
    host_to_disk(sex, p_sh_name,      &(disk->sh_name));
    host_to_disk(sex, p_sh_type,      &(disk->sh_type));
    host_to_disk(sex, p_sh_flags,     &(disk->sh_flags));
    host_to_disk(sex, p_sh_addr,      &(disk->sh_addr));
    host_to_disk(sex, p_sh_offset,    &(disk->sh_offset));
    host_to_disk(sex, p_sh_size,      &(disk->sh_size));
    host_to_disk(sex, p_sh_link,      &(disk->sh_link));
    host_to_disk(sex, p_sh_info,      &(disk->sh_info));
    host_to_disk(sex, p_sh_addralign, &(disk->sh_addralign));
    host_to_disk(sex, p_sh_entsize,   &(disk->sh_entsize));

    return disk;
}

/* Constructor reads the Elf Section Table (i.e., array of section headers) */
void
SgAsmElfSectionTable::ctor()
{
    set_synthesized(true);                              /* the section table isn't really a section itself */
    set_name(new SgAsmBasicString("ELF section table"));
    set_purpose(SP_HEADER);

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();

    if (fhdr->get_e_shnum() > 0) {

        /* Check sizes */
        ROSE_ASSERT(4 == fhdr->get_word_size() || 8 == fhdr->get_word_size());
        size_t struct_size = 4 == fhdr->get_word_size() ? sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk) : 
                                                          sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk);
	addr_t entsize = struct_size + fhdr->get_shextrasz();

        /* Change the section size to include all the entries */
        ROSE_ASSERT(0==get_size());
        extend(fhdr->get_e_shnum() * entsize);

        /* Read all the section headers. We can't just cast this to an array like with other structs because
         * the ELF header specifies the size of each entry. */
        std::vector<SgAsmElfSectionTableEntry*> entries;
        addr_t offset = 0;
        for (size_t i = 0; i < fhdr->get_e_shnum(); i++, offset += entsize) {
            SgAsmElfSectionTableEntry *shdr = NULL;
            if (4 == fhdr->get_word_size()) {
                const SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk *disk =
                    (const SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk*)content(offset, entsize);
                shdr = new SgAsmElfSectionTableEntry(sex, disk);
            } else {
                const SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk *disk =
                    (const SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk*)content(offset, entsize);
                shdr = new SgAsmElfSectionTableEntry(sex, disk);
            }

	    addr_t nextra = fhdr->get_shextrasz();
            if (nextra>0)
                shdr->get_extra() = content_ucl(offset+struct_size, nextra);
            entries.push_back(shdr);
        }

        /* Read the string table section first because we'll need this to initialize section names. */
        SgAsmElfStringSection *strsec = NULL;
        if (fhdr->get_e_shstrndx() > 0) {
            SgAsmElfSectionTableEntry *shdr = entries[fhdr->get_e_shstrndx()];
            strsec = new SgAsmElfStringSection(fhdr, shdr);
            strsec->set_id(fhdr->get_e_shstrndx());
            strsec->set_name(new SgAsmStoredString(strsec->get_strtab(), shdr->get_sh_name()));
            strsec->set_mapped_wperm((shdr->get_sh_flags() & 0x01) == 0x01);
            strsec->set_mapped_xperm((shdr->get_sh_flags() & 0x04) == 0x04);
            strsec->set_file_alignment(shdr->get_sh_addralign());
            strsec->set_mapped_alignment(shdr->get_sh_addralign());
            shdr->set_parent(strsec);
        }

        /* Read all other sections */
        for (size_t i = 0; i<entries.size(); i++) {
            SgAsmElfSectionTableEntry *shdr = entries[i];
            SgAsmElfSection *section = NULL;
            if (i == fhdr->get_e_shstrndx()) continue; /*we already read string table*/
            switch (shdr->get_sh_type()) {
              case SgAsmElfSectionTableEntry::SHT_NULL:
                /* Null entry. We still create the section just to hold the section header. */
                section = new SgAsmElfSection(fhdr, shdr, 0);
                break;
              case SgAsmElfSectionTableEntry::SHT_NOBITS:
                /* These types of sections don't occupy any file space (e.g., BSS) */
                section = new SgAsmElfSection(fhdr, shdr, 0);
                break;
              case SgAsmElfSectionTableEntry::SHT_DYNAMIC:
                section = new SgAsmElfDynamicSection(fhdr, shdr);
                break;
              case SgAsmElfSectionTableEntry::SHT_DYNSYM:
              case SgAsmElfSectionTableEntry::SHT_SYMTAB:
                section = new SgAsmElfSymbolSection(fhdr, shdr);
                break;
              case SgAsmElfSectionTableEntry::SHT_STRTAB:
                section = new SgAsmElfStringSection(fhdr, shdr);
                break;
              default:
                section = new SgAsmElfSection(fhdr, shdr);
                break;
            }
            section->set_id(i);
            if (strsec)
                section->set_name(new SgAsmStoredString(strsec->get_strtab(), shdr->get_sh_name()));
            section->set_mapped_wperm((shdr->get_sh_flags() & 0x01) == 0x01);
            section->set_mapped_xperm((shdr->get_sh_flags() & 0x04) == 0x04);
            section->set_file_alignment(shdr->get_sh_addralign());
            section->set_mapped_alignment(shdr->get_sh_addralign());
            shdr->set_parent(section);
        }

        /* Initialize links between sections */
        for (size_t i = 0; i < entries.size(); i++) {
            SgAsmElfSectionTableEntry *shdr = entries[i];
            if (shdr->get_sh_link() > 0) {
                SgAsmElfSection *source = dynamic_cast<SgAsmElfSection*>(fhdr->get_file()->get_section_by_id(i));
                SgAsmElfSection *target = dynamic_cast<SgAsmElfSection*>(fhdr->get_file()->get_section_by_id(shdr->get_sh_link()));
                source->set_linked_section(target);
            }
        }
    }
}

/* Update this section table entry with newer information from the section */
void
SgAsmElfSectionTableEntry::update_from_section(SgAsmElfSection *section)
{
    p_sh_name = dynamic_cast<SgAsmStoredString*>(section->get_name())->get_offset();

    p_sh_offset = section->get_offset();
    if (p_sh_type==SHT_NOBITS && section->is_mapped()) {
        p_sh_size = section->get_mapped_size();
    } else {
        p_sh_size = section->get_size();
    }

    if (section->is_mapped()) {
        p_sh_addr = section->get_mapped_rva();
        if (section->get_mapped_wperm()) {
            p_sh_flags |= 0x01;
        } else {
            p_sh_flags &= ~0x01;
        }
        if (section->get_mapped_xperm()) {
            p_sh_flags |= 0x04;
        } else {
            p_sh_flags &= ~0x04;
        }
    } else {
        p_sh_addr = 0;
        p_sh_flags &= ~0x05; /*clear write & execute bits*/
    }
    
    SgAsmElfSection *linked_to = section->get_linked_section();
    if (linked_to) {
        ROSE_ASSERT(linked_to->get_id()>0);
        p_sh_link = linked_to->get_id();
    } else {
        p_sh_link = 0;
    }
}

/* Print some debugging info */
void
SgAsmElfSectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSectionTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %u bytes into strtab\n",                      p, w, "sh_name",        p_sh_name);
    fprintf(f, "%s%-*s = %lu\n",                                       p, w, "sh_type",        p_sh_type);
    fprintf(f, "%s%-*s = %lu\n",                                       p, w, "sh_link",        p_sh_link);
    fprintf(f, "%s%-*s = %lu\n",                                       p, w, "sh_info",        p_sh_info);
    fprintf(f, "%s%-*s = 0x%08"PRIx64"\n",                             p, w, "sh_flags",       p_sh_flags);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n",                 p, w, "sh_addr",        p_sh_addr, p_sh_addr);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes into file\n", p, w, "sh_offset",      p_sh_offset, p_sh_offset);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "sh_size",        p_sh_size, p_sh_size);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "sh_addralign",   p_sh_addralign, p_sh_addralign);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "sh_entsize",     p_sh_entsize, p_sh_entsize);
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

/* Write the section table section back to disk */
void
SgAsmElfSectionTable::unparse(FILE *f)
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();

    /* Write the sections first */
    for (size_t i=0; i<sections.size(); i++) {
        if (sections[i]->get_id() >= 0) {
            sections[i]->unparse(f);
        }
    }

    /* Write the section table entries */
    for (size_t i = 0; i < sections.size(); ++i) {
        if (sections[i]->get_id() >= 0) {
            SgAsmElfSection *section = dynamic_cast<SgAsmElfSection*>(sections[i]);
            SgAsmElfSectionTableEntry *shdr = section->get_section_entry();
            ROSE_ASSERT(shdr!=NULL);
            shdr->update_from_section(section);

            SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk disk32;
            SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk disk64;
            void *disk  = NULL;
            size_t size = 0;

            if (4==fhdr->get_word_size()) {
                disk = shdr->encode(sex, &disk32);
                size = sizeof disk32;
            } else if (8==fhdr->get_word_size()) {
                disk = shdr->encode(sex, &disk64);
                size = sizeof disk64;
            } else {
                ROSE_ASSERT(!"invalid word size");
            }

            /* The disk struct */
	    addr_t entsize = size + fhdr->get_shextrasz();
            addr_t extra_offset = write(f, section->get_id()*entsize, size, disk);
            if (shdr->get_extra().size() > 0)
                write(f, extra_offset, shdr->get_extra());
        }
    }

    unparse_holes(f);
}

/* Print some debugging info */
void
SgAsmElfSectionTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSectionTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}
    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Segment tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Converts 32-bit disk representation to host representation */
void
SgAsmElfSegmentTableEntry::ctor(ByteOrder sex, const struct Elf32SegmentTableEntry_disk *disk) 
{
    p_type      = (SegmentType)disk_to_host(sex, disk->p_type);
    p_offset    = disk_to_host(sex, disk->p_offset);
    p_vaddr     = disk_to_host(sex, disk->p_vaddr);
    p_paddr     = disk_to_host(sex, disk->p_paddr);
    p_filesz    = disk_to_host(sex, disk->p_filesz);
    p_memsz     = disk_to_host(sex, disk->p_memsz);
    p_flags     = (SegmentFlags)disk_to_host(sex, disk->p_flags);
    p_align     = disk_to_host(sex, disk->p_align);
}

/* Converts 64-bit disk representation to host representation */
void
SgAsmElfSegmentTableEntry::ctor(ByteOrder sex, const Elf64SegmentTableEntry_disk *disk) 
{
    p_type      = (SegmentType)disk_to_host(sex, disk->p_type);
    p_offset    = disk_to_host(sex, disk->p_offset);
    p_vaddr     = disk_to_host(sex, disk->p_vaddr);
    p_paddr     = disk_to_host(sex, disk->p_paddr);
    p_filesz    = disk_to_host(sex, disk->p_filesz);
    p_memsz     = disk_to_host(sex, disk->p_memsz);
    p_flags     = (SegmentFlags)disk_to_host(sex, disk->p_flags);
    p_align     = disk_to_host(sex, disk->p_align);
}

/* Converts segment table entry back into disk structure */
void *
SgAsmElfSegmentTableEntry::encode(ByteOrder sex, Elf32SegmentTableEntry_disk *disk)
{
    host_to_disk(sex, p_type, &(disk->p_type));
    host_to_disk(sex, p_offset, &(disk->p_offset));
    host_to_disk(sex, p_vaddr, &(disk->p_vaddr));
    host_to_disk(sex, p_paddr, &(disk->p_paddr));
    host_to_disk(sex, p_filesz, &(disk->p_filesz));
    host_to_disk(sex, p_memsz, &(disk->p_memsz));
    host_to_disk(sex, p_flags, &(disk->p_flags));
    host_to_disk(sex, p_align, &(disk->p_align));
    return disk;
}
void *
SgAsmElfSegmentTableEntry::encode(ByteOrder sex, Elf64SegmentTableEntry_disk *disk)
{
    host_to_disk(sex, p_type, &(disk->p_type));
    host_to_disk(sex, p_offset, &(disk->p_offset));
    host_to_disk(sex, p_vaddr, &(disk->p_vaddr));
    host_to_disk(sex, p_paddr, &(disk->p_paddr));
    host_to_disk(sex, p_filesz, &(disk->p_filesz));
    host_to_disk(sex, p_memsz, &(disk->p_memsz));
    host_to_disk(sex, p_flags, &(disk->p_flags));
    host_to_disk(sex, p_align, &(disk->p_align));
    return disk;
}

/* Update this segment table entry with newer information from the section */
void
SgAsmElfSegmentTableEntry::update_from_section(SgAsmElfSection *section)
{
    set_offset(section->get_offset());
    set_filesz(section->get_size());

    set_vaddr(section->get_mapped_va());
    set_memsz(section->get_mapped_size());
    if (section->get_mapped_rperm()) {
        p_flags = (SegmentFlags)(p_flags | PF_RPERM);
    } else {
        p_flags = (SegmentFlags)(p_flags & ~PF_RPERM);
    }
    if (section->get_mapped_wperm()) {
        p_flags = (SegmentFlags)(p_flags | PF_WPERM);
    } else {
        p_flags = (SegmentFlags)(p_flags & ~PF_WPERM);
    }
    if (section->get_mapped_xperm()) {
        p_flags = (SegmentFlags)(p_flags | PF_XPERM);
    } else {
        p_flags = (SegmentFlags)(p_flags & ~PF_XPERM);
    }
}

/* Print some debugging info */
void
SgAsmElfSegmentTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSegmentTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSegmentTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    fprintf(f, "%s%-*s = %zu\n",                             p, w, "index",  p_index);
    fprintf(f, "%s%-*s = 0x%08x = %s\n",                     p, w, "type",   p_type,  stringifyType(p_type).c_str());
    fprintf(f, "%s%-*s = 0x%08x ",                           p, w, "flags",  p_flags);
    fputc(p_flags & PF_RPERM ? 'r' : '-', f);
    fputc(p_flags & PF_WPERM ? 'w' : '-', f);
    fputc(p_flags & PF_XPERM ? 'x' : '-', f);
    if (p_flags & PF_PROC_MASK) fputs(" proc", f);
    if (p_flags & PF_RESERVED) fputs(" *", f);
    fputc('\n', f);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes into file\n", p, w, "offset", p_offset, p_offset);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n",                 p, w, "vaddr",  p_vaddr, p_vaddr);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n",                 p, w, "paddr",  p_paddr, p_paddr);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "filesz", p_filesz, p_filesz);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "memsz",  p_memsz, p_memsz);
    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64") bytes\n",           p, w, "align",  p_align, p_align);
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

// DQ (26/2008): Support output of named enum values
std::string
SgAsmElfSegmentTableEntry::stringifyType(SegmentType kind) const
{
    std::string s;

    switch (kind) {
      case SgAsmElfSegmentTableEntry::PT_NULL:    s = "PT_NULL";    break;
      case SgAsmElfSegmentTableEntry::PT_LOAD:    s = "PT_LOAD";    break;
      case SgAsmElfSegmentTableEntry::PT_DYNAMIC: s = "PT_DYNAMIC"; break;
      case SgAsmElfSegmentTableEntry::PT_INTERP:  s = "PT_INTERP";  break;
      case SgAsmElfSegmentTableEntry::PT_NOTE:    s = "PT_NOTE";    break;
      case SgAsmElfSegmentTableEntry::PT_SHLIB:   s = "PT_SHLIB";   break;
      case SgAsmElfSegmentTableEntry::PT_PHDR:    s = "PT_PHDR";    break;
      case SgAsmElfSegmentTableEntry::PT_LOPROC:  s = "PT_LOPROC";  break;
      case SgAsmElfSegmentTableEntry::PT_HIPROC:  s = "PT_HIPROC";  break;

      default:
      {
          s = "error";

          // DQ (8/29/2008): This case is exercised frequently, I think it warrants only a warning, instead of an error.
          printf ("Warning: default reached for SgAsmElfSegmentTableEntry::stringifyType = 0x%x \n",kind);
      }
    }
    return s;
}

/* Constructor reads the Elf Segment (Program Header) Table */
void
SgAsmElfSegmentTable::ctor()
{
    set_synthesized(true);                              /* the segment table isn't part of any explicit section */
    set_name(new SgAsmBasicString("ELF Segment Table"));
    set_purpose(SP_HEADER);

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();
    
    if (fhdr->get_e_phnum() > 0) {

        /* Check sizes */
        ROSE_ASSERT(4==fhdr->get_word_size() || 8==fhdr->get_word_size());
        size_t struct_size = 4 == fhdr->get_word_size() ? 
                            sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk) : 
                            sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk);
	addr_t entsize = struct_size + fhdr->get_phextrasz();

        /* Extend section to hold entire table */
        ROSE_ASSERT(0==get_size());
        extend(fhdr->get_e_phnum() * entsize);

        addr_t offset=0;                                /* w.r.t. the beginning of this section */
        for (size_t i=0; i<fhdr->get_e_phnum(); i++, offset += entsize) {
            /* Read/decode the segment header */
            SgAsmElfSegmentTableEntry *shdr = NULL;
            if (4==fhdr->get_word_size()) {
                const SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk *disk =
                    (const SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk*)content(offset, struct_size);
                shdr = new SgAsmElfSegmentTableEntry(sex, disk);
            } else {
                const SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk *disk =
                    (const SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk*)content(offset, struct_size);
                shdr = new SgAsmElfSegmentTableEntry(sex, disk);
            }
            shdr->set_index(i);

            /* Save extra bytes */
            addr_t nextra = fhdr->get_phextrasz();
            if (nextra>0)
                shdr->get_extra() = content_ucl(offset+struct_size, nextra);

            /* Null segments are just unused slots in the table; no real section to create */
            if (SgAsmElfSegmentTableEntry::PT_NULL == shdr->get_type())
                continue;

            /* Create SgAsmElfSection objects for each ELF Segment. However, if the ELF Segment Table describes a segment
             * that's the same offset and size as a section from the Elf Section Table (and the memory mappings are
             * consistent) then use the preexisting section instead of creating a new one. */
            SgAsmElfSection *s = NULL;
            SgAsmGenericSectionPtrList possible = fhdr->get_file()->get_sections_by_offset(shdr->get_offset(), shdr->get_filesz());
            for (size_t j = 0; !s && j < possible.size(); j++) {
                if (possible[j]->get_offset()!=shdr->get_offset() || possible[j]->get_size()!=shdr->get_filesz())
                    continue; /*different file extent*/
                if (possible[j]->is_mapped()) {
                    if (possible[j]->get_mapped_rva() != shdr->get_vaddr() || possible[j]->get_mapped_size() != shdr->get_memsz())
                        continue; /*different mapped address or size*/
                    unsigned section_perms = (possible[j]->get_mapped_rperm() ? 0x01 : 0x00) |
                                             (possible[j]->get_mapped_wperm() ? 0x02 : 0x00) |
                                             (possible[j]->get_mapped_xperm() ? 0x04 : 0x00);
                    unsigned segment_perms = (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? 0x01 : 0x00) |
                                             (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? 0x02 : 0x00) |
                                             (shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? 0x04 : 0x00);
                    if (section_perms != segment_perms)
                        continue; /*different mapped permissions*/
                }

                /* Found a match. Set memory mapping params. */
                s = dynamic_cast<SgAsmElfSection*>(possible[j]);
                if (!s) continue; /*potential match was not from the ELF Section or Segment table*/
                if (s->get_segment_entry()) continue; /*potential match is assigned to some other segment table entry*/

                s->set_segment_entry(shdr);
                if (!s->is_mapped()) {
                    s->set_mapped_rva(shdr->get_vaddr());
                    s->set_mapped_size(shdr->get_memsz());
                    s->set_file_alignment(shdr->get_align());
                    s->set_mapped_alignment(shdr->get_align());
                    s->set_mapped_rperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? true : false);
                    s->set_mapped_wperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? true : false);
                    s->set_mapped_xperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? true : false);
                }
            }

            /* Create a new segment if no matching section was found. */
            if (!s)
                s = new SgAsmElfSection(fhdr, shdr);
        }
    }
}

/* Write the segment table to disk. */
void
SgAsmElfSegmentTable::unparse(FILE *f)
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();

    /* Write the segments first */
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmElfSection *section = dynamic_cast<SgAsmElfSection*>(sections[i]);
        if (section && section->get_segment_entry()) {
            section->unparse(f);
        }
    }
    
    /* Write the segment table entries */
    for (size_t i=0; i < sections.size(); ++i) {
        SgAsmElfSection *section = dynamic_cast<SgAsmElfSection*>(sections[i]);
        if (section && section->get_segment_entry()) {
            SgAsmElfSegmentTableEntry *shdr = section->get_segment_entry();
            shdr->update_from_section(section);
            
            SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk disk32;
            SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk disk64;
            void *disk = NULL;
            size_t size = 0;
        
            if (4==fhdr->get_word_size()) {
                disk = shdr->encode(sex, &disk32);
                size = sizeof disk32;
            } else if (8==fhdr->get_word_size()) {
                disk = shdr->encode(sex, &disk64);
                size = sizeof disk64;
            } else {
                ROSE_ASSERT(!"invalid word size");
            }
        
            /* The disk struct */
	    addr_t entsize = size + fhdr->get_phextrasz();
            addr_t extra_offset = write(f, shdr->get_index() * entsize, size, disk);
            if (shdr->get_extra().size() > 0)
                write(f, extra_offset, shdr->get_extra());
        }
    }

    unparse_holes(f);
}

/* Print some debugging info */
void
SgAsmElfSegmentTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSegmentTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSegmentTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Linking
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructors */
void
SgAsmElfDynamicEntry::ctor(ByteOrder sex, const Elf32DynamicEntry_disk *disk)
{
    p_d_tag = (EntryType)disk_to_host(sex, disk->d_tag);
    p_d_val = disk_to_host(sex, disk->d_val);
}

void
SgAsmElfDynamicEntry::ctor(ByteOrder sex, const Elf64DynamicEntry_disk *disk)
{
    p_d_tag = (EntryType)disk_to_host(sex, disk->d_tag);
    p_d_val = disk_to_host(sex, disk->d_val);
}

/* Encode a native entry back into disk format */
void *
SgAsmElfDynamicEntry::encode(ByteOrder sex, Elf32DynamicEntry_disk *disk)
{
    host_to_disk(sex, p_d_tag, &(disk->d_tag));
    host_to_disk(sex, p_d_val.get_rva(), &(disk->d_val));
    return disk;
}
void *
SgAsmElfDynamicEntry::encode(ByteOrder sex, Elf64DynamicEntry_disk *disk)
{
    host_to_disk(sex, p_d_tag, &(disk->d_tag));
    host_to_disk(sex, p_d_val.get_rva(), &(disk->d_val));
    return disk;
}

/* Convert Dynamic Entry Tag to a string */
const char *
SgAsmElfDynamicEntry::stringify_tag(EntryType t)
{
    switch (t) {
      case DT_NULL:             return "DT_NULL";
      case DT_NEEDED:           return "DT_NEEDED";
      case DT_PLTRELSZ:         return "DT_PLTRELSZ";
      case DT_PLTGOT:           return "DT_PLTGOT";
      case DT_HASH:             return "DT_HASH";
      case DT_STRTAB:           return "DT_STRTAB";
      case DT_SYMTAB:           return "DT_SYMTAB";
      case DT_RELA:             return "DT_RELA";
      case DT_RELASZ:           return "DT_RELASZ";
      case DT_RELAENT:          return "DT_RELAENT";
      case DT_STRSZ:            return "DT_STRSZ";
      case DT_SYMENT:           return "DT_SYMENT";
      case DT_INIT:             return "DT_INIT";
      case DT_FINI:             return "DT_FINI";
      case DT_SONAME:           return "DT_SONAME";
      case DT_RPATH:            return "DT_RPATH";
      case DT_SYMBOLIC:         return "DT_SYMBOLIC";
      case DT_REL:              return "DT_REL";
      case DT_RELSZ:            return "DT_RELSZ";
      case DT_RELENT:           return "DT_RELENT";
      case DT_PLTREL:           return "DT_PLTREL";
      case DT_DEBUG:            return "DT_DEBUG";
      case DT_TEXTREL:          return "DT_TEXTREL";
      case DT_JMPREL:           return "DT_JMPREL";
      case DT_BIND_NOW:         return "DT_BIND_NOW";
      case DT_INIT_ARRAY:       return "DT_INIT_ARRAY";
      case DT_FINI_ARRAY:       return "DT_FINI_ARRAY";
      case DT_INIT_ARRAYSZ:     return "DT_INIT_ARRAYSZ";
      case DT_FINI_ARRAYSZ:     return "DT_FINI_ARRAYSZ";
      case DT_RUNPATH:          return "DT_RUNPATH";
      case DT_FLAGS:            return "DT_FLAGS";
      case DT_PREINIT_ARRAY:    return "DT_PREINIT_ARRAY";
      case DT_PREINIT_ARRAYSZ:  return "DT_PREINIT_ARRAYSZ";
      case DT_NUM:              return "DT_NUM";
      case DT_GNU_PRELINKED:    return "DT_GNU_PRELINKED";
      case DT_GNU_CONFLICTSZ:   return "DT_GNU_CONFLICTSZ";
      case DT_GNU_LIBLISTSZ:    return "DT_GNU_LIBLISTSZ";
      case DT_CHECKSUM:         return "DT_CHECKSUM";
      case DT_PLTPADSZ:         return "DT_PLTPADSZ";
      case DT_MOVEENT:          return "DT_MOVEENT";
      case DT_MOVESZ:           return "DT_MOVESZ";
      case DT_FEATURE_1:        return "DT_FEATURE_1";
      case DT_POSFLAG_1:        return "DT_POSFLAG_1";
      case DT_SYMINSZ:          return "DT_SYMINSZ";
      case DT_SYMINENT:         return "DT_SYMINENT";
      case DT_GNU_HASH:         return "DT_GNU_HASH";
      case DT_TLSDESC_PLT:      return "DT_TLSDESC_PLT";
      case DT_TLSDESC_GOT:      return "DT_TLSDESC_GOT";
      case DT_GNU_CONFLICT:     return "DT_GNU_CONFLICT";
      case DT_GNU_LIBLIST:      return "DT_GNU_LIBLIST";
      case DT_CONFIG:           return "DT_CONFIG";
      case DT_DEPAUDIT:         return "DT_DEPAUDIT";
      case DT_AUDIT:            return "DT_AUDIT";
      case DT_PLTPAD:           return "DT_PLTPAD";
      case DT_MOVETAB:          return "DT_MOVETAB";
      case DT_SYMINFO:          return "DT_SYMINFO";
      case DT_VERSYM:           return "DT_VERSYM";
      case DT_RELACOUNT:        return "DT_RELACOUNT";
      case DT_RELCOUNT:         return "DT_RELCOUNT";
      case DT_FLAGS_1:          return "DT_FLAGS_1";
      case DT_VERDEF:           return "DT_VERDEF";
      case DT_VERDEFNUM:        return "DT_VERDEFNUM";
      case DT_VERNEED:          return "DT_VERNEED";
      case DT_VERNEEDNUM:       return "DT_VERNEEDNUM";
      case DT_AUXILIARY:        return "DT_AUXILIARY";
      case DT_FILTER:           return "DT_FILTER";

      default:
        static char s[64];
        sprintf(s, "0x%08lx", (unsigned long)t);
        return s;
    }
}

/* Print some debugging info */
void
SgAsmElfDynamicEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfDynamicEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfDynamicEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    char label[256];
    strcpy(label, stringify_tag(p_d_tag));
    for (char *s=label; *s; s++) *s = tolower(*s);

    fprintf(f, "%s%-*s = 0x%08"PRIx64" (%"PRIu64")\n", p, w, label, p_d_val.get_rva(), p_d_val.get_rva());
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

/* Constructor */
void
SgAsmElfDynamicSection::ctor(SgAsmElfFileHeader *fhdr, SgAsmElfSectionTableEntry *shdr)
{
    p_entries = new SgAsmElfDynamicEntryList;
    p_entries->set_parent(this);

    size_t entry_size, struct_size, extra_size;
    size_t nentries = calculate_sizes(&entry_size, &struct_size, &extra_size);
    ROSE_ASSERT(entry_size==shdr->get_sh_entsize());

    /* Parse each entry; some fields can't be initialized until set_linked_section() is called. */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfDynamicEntry *entry=0;
        if (4==fhdr->get_word_size()) {
            const SgAsmElfDynamicEntry::Elf32DynamicEntry_disk *disk =
                (const SgAsmElfDynamicEntry::Elf32DynamicEntry_disk*)content(i*entry_size, struct_size);
            entry = new SgAsmElfDynamicEntry(fhdr->get_sex(), disk);
        } else if (8==fhdr->get_word_size()) {
            const SgAsmElfDynamicEntry::Elf64DynamicEntry_disk *disk =
                (const SgAsmElfDynamicEntry::Elf64DynamicEntry_disk*)content(i*entry_size, struct_size);
            entry = new SgAsmElfDynamicEntry(fhdr->get_sex(), disk);
        } else {
            throw FormatError("unsupported ELF word size");
        }
        if (extra_size>0)
            entry->get_extra() = content_ucl(i*entry_size+struct_size, extra_size);
        p_entries->get_entries().push_back(entry);
        ROSE_ASSERT(p_entries->get_entries().size()>0);
    }
}

/* Returns info about the size of the entries. Each entry has a required part and an optional part. The return value is the
 * number of entries in the table. The size of the parts are returned through arguments. */
size_t
SgAsmElfDynamicSection::calculate_sizes(size_t *total, size_t *required, size_t *optional)
{
    size_t struct_size = 0;
    size_t extra_size = 0;
    size_t entry_size = 0;
    size_t nentries = 0;
    SgAsmElfFileHeader *fhdr = get_elf_header();

    /* Assume ELF Section Table Entry is correct (for now) for the size of each entry in the table. */
    ROSE_ASSERT(get_section_entry());
    entry_size = get_section_entry()->get_sh_entsize();

    /* Size of required part of each entry */
    if (4==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfDynamicEntry::Elf32DynamicEntry_disk);
    } else if (8==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfDynamicEntry::Elf64DynamicEntry_disk);
    } else {
        throw FormatError("bad ELF word size");
    }

    /* Entire entry should be at least large enough for the required part. This also takes care of the case when the ELF
     * Section Table Entry has a zero-valued sh_entsize */
    entry_size = std::max(entry_size, struct_size);

    /* Size of optional parts. If we've parsed the table then use the largest optional part, otherwise assume the entry from
     * the ELF Section Table is correct. */
    if ((nentries=p_entries->get_entries().size())>0) {
        for (size_t i=0; i<nentries; i++) {
            SgAsmElfDynamicEntry *entry = p_entries->get_entries()[i];
            extra_size = std::max(extra_size, entry->get_extra().size());
        }
    } else {
        extra_size = entry_size - struct_size;
        nentries = get_size() / entry_size;
    }
    
    /* Return values */
    if (total)
        *total = entry_size;
    if (required)
        *required = struct_size;
    if (optional)
        *optional = extra_size;
    return nentries;
}
    
/* Set linked section (the string table) and finish initializing the section entries. */
void
SgAsmElfDynamicSection::set_linked_section(SgAsmElfSection *_strsec) 
{
    SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(_strsec);
    ROSE_ASSERT(strsec!=NULL);
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr!=NULL && fhdr == strsec->get_elf_header());

    /* This method augments the super class */
    SgAsmElfSection::set_linked_section(strsec);

    /* Finalize each entry */
    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfDynamicEntry *entry = p_entries->get_entries()[i];
        switch (entry->get_d_tag()) {
          case SgAsmElfDynamicEntry::DT_NEEDED: {
              /* Offset to NUL-terminated library name in the linked-to (".dynstr") section. */
              ROSE_ASSERT(entry->get_d_val().get_section()==NULL);
              SgAsmStoredString *name = new SgAsmStoredString(strsec->get_strtab(), entry->get_d_val().get_rva());
              fhdr->add_dll(new SgAsmGenericDLL(name));
              break;
          }
          case SgAsmElfDynamicEntry::DT_PLTGOT:
          case SgAsmElfDynamicEntry::DT_HASH:
          case SgAsmElfDynamicEntry::DT_STRTAB:
          case SgAsmElfDynamicEntry::DT_SYMTAB:
          case SgAsmElfDynamicEntry::DT_RELA:
          case SgAsmElfDynamicEntry::DT_INIT:
          case SgAsmElfDynamicEntry::DT_FINI:
          case SgAsmElfDynamicEntry::DT_REL:
          case SgAsmElfDynamicEntry::DT_DEBUG:
          case SgAsmElfDynamicEntry::DT_JMPREL:
          case SgAsmElfDynamicEntry::DT_INIT_ARRAY:
          case SgAsmElfDynamicEntry::DT_FINI_ARRAY:
          case SgAsmElfDynamicEntry::DT_PREINIT_ARRAY:
          case SgAsmElfDynamicEntry::DT_GNU_HASH:
          case SgAsmElfDynamicEntry::DT_TLSDESC_PLT:
          case SgAsmElfDynamicEntry::DT_TLSDESC_GOT:
          case SgAsmElfDynamicEntry::DT_GNU_CONFLICT:
          case SgAsmElfDynamicEntry::DT_GNU_LIBLIST:
          case SgAsmElfDynamicEntry::DT_CONFIG:
          case SgAsmElfDynamicEntry::DT_DEPAUDIT:
          case SgAsmElfDynamicEntry::DT_AUDIT:
          case SgAsmElfDynamicEntry::DT_PLTPAD:
          case SgAsmElfDynamicEntry::DT_MOVETAB:
          case SgAsmElfDynamicEntry::DT_SYMINFO:
          case SgAsmElfDynamicEntry::DT_VERSYM:
          case SgAsmElfDynamicEntry::DT_VERDEF:
          case SgAsmElfDynamicEntry::DT_VERNEED:
          case SgAsmElfDynamicEntry::DT_AUXILIARY:
          case SgAsmElfDynamicEntry::DT_FILTER: {
              /* d_val is relative to a section. Even though this doesn't depend on _strsec we perform this action in this
               * function because we know that all ELF Sections (but perhaps not the ELF Segments) have been created by this
               * time. */
              ROSE_ASSERT(entry->get_d_val().get_section()==NULL);
              SgAsmGenericSectionPtrList containers = fhdr->get_sections_by_rva(entry->get_d_val().get_rva());
              SgAsmGenericSection *best = NULL;
              for (SgAsmGenericSectionPtrList::iterator i=containers.begin(); i!=containers.end(); ++i) {
                  if ((*i)->is_mapped()) {
                      if ((*i)->get_mapped_rva()==entry->get_d_val().get_rva()) {
                          best = *i;
                          break;
                      } else if (!best) {
                          best = *i;
                      } else if ((*i)->get_mapped_size() < best->get_mapped_size()) {
                          best = *i;
                      }
                  }
              }
              if (best)
                  entry->set_d_val(rose_rva_t(entry->get_d_val().get_rva(), best));
              break;
          }
          default:
            break;
        }
    }
}

/* Write the dynamic section back to disk */
void
SgAsmElfDynamicSection::unparse(FILE *f)
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    ByteOrder sex = fhdr->get_sex();

    size_t entry_size, struct_size, extra_size;
    calculate_sizes(&entry_size, &struct_size, &extra_size);
    size_t nentries = p_entries->get_entries().size();

    /* Adjust section size. FIXME: this should be moved to the reallocate() function. (RPM 2008-10-07) */
    if (nentries*entry_size < get_size()) {
        /* Make the section smaller without affecting other sections. This is allowed during unparsing. */
        if (is_mapped()) {
            ROSE_ASSERT(get_mapped_size()==get_size());
            set_mapped_size(nentries*entry_size);
        }
        set_size(nentries*entry_size);
    } else if (nentries*entry_size > get_size()) {
        /* We should have detected this before unparsing! */
        ROSE_ASSERT(!"can't expand dynamic section while unparsing!");
    }
    
    /* Adjust the entry size stored in the ELF Section Table */
    get_section_entry()->set_sh_entsize(entry_size);

    /* Write each entry's required part followed by the optional part */
    for (size_t i = 0; i < p_entries->get_entries().size(); i++) {
        SgAsmElfDynamicEntry::Elf32DynamicEntry_disk disk32;
        SgAsmElfDynamicEntry::Elf64DynamicEntry_disk disk64;
        void *disk  = NULL;

        SgAsmElfDynamicEntry *entry = p_entries->get_entries()[i];

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

/* Print some debugging info */
void
SgAsmElfDynamicSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sDynamicSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sDynamicSection.", prefix);
    }

    SgAsmElfSection::dump(f, p, -1);

    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfDynamicEntry *ent = p_entries->get_entries()[i];
        ent->dump(f, p, i);
        dump_containing_sections(f, std::string(p)+"...", ent->get_d_val(), get_header()->get_sections()->get_sections());
    }

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Symbol Tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructors */
void
SgAsmElfSymbol::ctor(ByteOrder sex, const Elf32SymbolEntry_disk *disk)
{
    set_name(new SgAsmBasicString("")); /*ROSETTA-generated constructor doesn't do this! (RPM 2008-09-12)*/

    p_st_name  = disk_to_host(sex, disk->st_name);
    p_st_info  = disk_to_host(sex, disk->st_info);
    p_st_res1  = disk_to_host(sex, disk->st_res1);
    p_st_shndx = disk_to_host(sex, disk->st_shndx);
    p_st_size  = disk_to_host(sex, disk->st_size);

    p_value    = disk_to_host(sex, disk->st_value);
    p_size     = p_st_size;
    ctor_common();
}
void
SgAsmElfSymbol::ctor(ByteOrder sex, const Elf64SymbolEntry_disk *disk)
{
    set_name(new SgAsmBasicString("")); /*ROSETTA-generated constructor doesn't do this! (RPM 2008-09-12)*/

    p_st_name  = disk_to_host(sex, disk->st_name);
    p_st_info  = disk_to_host(sex, disk->st_info);
    p_st_res1  = disk_to_host(sex, disk->st_res1);
    p_st_shndx = disk_to_host(sex, disk->st_shndx);
    p_st_size  = disk_to_host(sex, disk->st_size);

    p_value    = disk_to_host(sex, disk->st_value);
    p_size     = p_st_size;
    ctor_common();
}
void
SgAsmElfSymbol::ctor_common()
{
    /* Binding */
    switch (get_elf_binding()) {
      case STB_LOCAL:   p_binding = SYM_LOCAL;  break;
      case STB_GLOBAL:  p_binding = SYM_GLOBAL; break;
      case STB_WEAK:    p_binding = SYM_WEAK;   break;
      default:
        fprintf(stderr, "ROBB: unknown elf symbol binding: %u\n", get_elf_binding());
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
      default:
        fprintf(stderr, "ROBB: unknown elf symbol type: %u\n", get_elf_type());
        ROSE_ASSERT(0);
        break;
    }

    /* Definition state */
    if (p_value || p_size) {
        p_def_state = SYM_DEFINED;
    } else if (p_st_name > 0 || get_elf_type()) {
        p_def_state = SYM_TENTATIVE;
    } else {
        p_def_state = SYM_UNDEFINED;
    }
}

/* Encode a symbol into disk format */
void *
SgAsmElfSymbol::encode(ByteOrder sex, Elf32SymbolEntry_disk *disk)
{
    host_to_disk(sex, p_st_name,     &(disk->st_name));
    host_to_disk(sex, p_st_info,     &(disk->st_info));
    host_to_disk(sex, p_st_res1,     &(disk->st_res1));
    host_to_disk(sex, p_st_shndx,    &(disk->st_shndx));
    host_to_disk(sex, p_st_size,     &(disk->st_size));
    host_to_disk(sex, get_value(), &(disk->st_value));
    return disk;
}
void *
SgAsmElfSymbol::encode(ByteOrder sex, Elf64SymbolEntry_disk *disk)
{
    host_to_disk(sex, p_st_name,     &(disk->st_name));
    host_to_disk(sex, p_st_info,     &(disk->st_info));
    host_to_disk(sex, p_st_res1,     &(disk->st_res1));
    host_to_disk(sex, p_st_shndx,    &(disk->st_shndx));
    host_to_disk(sex, p_st_size,     &(disk->st_size));
    host_to_disk(sex, get_value(), &(disk->st_value));
    return disk;
}

/* Print some debugging info. The 'section' is an optional section pointer for the st_shndx member. */
void
SgAsmElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmGenericSection *section)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymbol[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymbol.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    const char *s;
    char sbuf[256];

    SgAsmGenericSymbol::dump(f, p, -1);

    fprintf(f, "%s%-*s = %"PRIu64" offset into strtab\n", p, w, "st_name",  p_st_name);

    fprintf(f, "%s%-*s = %u (",          p, w, "st_info",  p_st_info);
    switch (get_elf_binding()) {
      case STB_LOCAL:  s = "local";  break;
      case STB_GLOBAL: s = "global"; break;
      case STB_WEAK:   s = "weak";   break;
      default:
        sprintf(sbuf, "binding-%d", get_elf_binding());
        s = sbuf;
        break;
    }
    fputs(s, f);
    switch (get_elf_type()) {
      case STT_NOTYPE:  s = " no-type";   break;
      case STT_OBJECT:  s = " object";    break;
      case STT_FUNC:    s = " function";  break;
      case STT_SECTION: s = " section";   break;
      case STT_FILE:    s = " file";      break;
      default:
        sprintf(sbuf, " type-%d", get_elf_type());
        s = sbuf;
        break;
    }
    fputs(s, f);
    fputs(")\n", f);

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

/* Constructor */
void
SgAsmElfSymbolSection::ctor(SgAsmElfSectionTableEntry *shdr)
{
    p_symbols = new SgAsmElfSymbolList;
    p_symbols->set_parent(this);

    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr!=NULL);

    size_t entry_size, struct_size, extra_size;
    size_t nentries = calculate_sizes(&entry_size, &struct_size, &extra_size);
    ROSE_ASSERT(entry_size==shdr->get_sh_entsize());
    
    /* Parse each entry; some fields can't be initialized until set_linked_section() is called. */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfSymbol *entry=0;
        if (4==fhdr->get_word_size()) {
            const SgAsmElfSymbol::Elf32SymbolEntry_disk *disk =
                (const SgAsmElfSymbol::Elf32SymbolEntry_disk*)content(i*entry_size, struct_size);
            entry = new SgAsmElfSymbol(fhdr->get_sex(), disk);
        } else if (8==fhdr->get_word_size()) {
            const SgAsmElfSymbol::Elf64SymbolEntry_disk *disk =
                (const SgAsmElfSymbol::Elf64SymbolEntry_disk*)content(i*entry_size, struct_size);
            entry = new SgAsmElfSymbol(fhdr->get_sex(), disk);
        } else {
            throw FormatError("unsupported ELF word size");
        }
        if (extra_size>0)
            entry->get_extra() = content_ucl(i*entry_size+struct_size, extra_size);
        p_symbols->get_symbols().push_back(entry);
        ROSE_ASSERT(p_symbols->get_symbols().size()>0);
    }
}

/* Returns info about the size of the entries. Each entry has a required part and an optional part. The return value is the
 * number of entries in the table. The size of the parts are returned through arguments. */
size_t
SgAsmElfSymbolSection::calculate_sizes(size_t *total, size_t *required, size_t *optional)
{
    size_t struct_size = 0;
    size_t extra_size = 0;
    size_t entry_size = 0;
    size_t nentries = 0;
    SgAsmElfFileHeader *fhdr = get_elf_header();

    /* Assume ELF Section Table Entry is correct (for now) for the size of each entry in the table. */
    ROSE_ASSERT(get_section_entry());
    entry_size = get_section_entry()->get_sh_entsize();

    /* Size of required part of each entry */
    if (4==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSymbol::Elf32SymbolEntry_disk);
    } else if (8==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSymbol::Elf64SymbolEntry_disk);
    } else {
        throw FormatError("bad ELF word size");
    }

    /* Entire entry should be at least large enough for the required part. This also takes care of the case when the ELF
     * Section Table Entry has a zero-valued sh_entsize */
    entry_size = std::max(entry_size, struct_size);

    /* Size of optional parts. If we've parsed the table then use the largest optional part, otherwise assume the entry from
     * the ELF Section Table is correct. */
    if ((nentries=p_symbols->get_symbols().size())>0) {
        for (size_t i=0; i<nentries; i++) {
            SgAsmElfSymbol *entry = p_symbols->get_symbols()[i];
            extra_size = std::max(extra_size, entry->get_extra().size());
        }
    } else {
        extra_size = entry_size - struct_size;
        nentries = get_size() / entry_size;
    }
    
    /* Return values */
    if (total)
        *total = entry_size;
    if (required)
        *required = struct_size;
    if (optional)
        *optional = extra_size;
    return nentries;
}

/* Symbol table sections link to their string tables. Updating the string table should cause the symbol names to be updated.
 * Also update section pointers for locally-bound symbols since we know that the section table has been read and all
 * non-synthesized sections have been created.
 * 
 * The st_shndx is the index (ID) of the section to which the symbol is bound. Special values are:
 *   0x0000        no section (section table entry zero should be all zeros anyway)
 *   0xff00-0xffff reserved values, not an index
 *   0xff00-0xff1f processor specific values
 *   0xfff1        symbol has absolute value not affected by relocation
 *   0xfff2        symbol is fortran common or unallocated C extern */
void
SgAsmElfSymbolSection::set_linked_section(SgAsmElfSection *_strsec)
{
    SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(_strsec);
    ROSE_ASSERT(strsec!=NULL);

    SgAsmElfSection::set_linked_section(strsec);
    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmElfSymbol *symbol = p_symbols->get_symbols()[i];

        /* Get symbol name */
        SgAsmStoredString *name = new SgAsmStoredString(strsec->get_strtab(), symbol->get_st_name());
        symbol->set_name(name);

        /* Get bound section ptr */
        if (symbol->get_st_shndx() > 0 && symbol->get_st_shndx() < 0xff00) {
            SgAsmGenericSection *bound = get_file()->get_section_by_id(symbol->get_st_shndx());
            ROSE_ASSERT(bound != NULL);
            symbol->set_bound(bound);
        }

        /* Section symbols may need names and sizes */
        if (symbol->get_type() == SgAsmElfSymbol::SYM_SECTION && symbol->get_bound()) {
            if (symbol->get_name()->get_string().size() == 0)
                symbol->set_name(symbol->get_bound()->get_name());
            if (symbol->get_size() == 0)
                symbol->set_size(symbol->get_bound()->get_size());
        }
    }
}

/* Write symbol table sections back to disk */
void
SgAsmElfSymbolSection::unparse(FILE *f)
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    ByteOrder sex = fhdr->get_sex();

    size_t entry_size, struct_size, extra_size;
    calculate_sizes(&entry_size, &struct_size, &extra_size);
    size_t nentries = p_symbols->get_symbols().size();
    
    /* Adjust section size. FIXME: this should be moved to the reallocate() function. (RPM 2008-10-07) */
    if (nentries*entry_size < get_size()) {
        /* Make the section smaller without affecting other sections. This is allowed during unparsing. */
        if (is_mapped()) {
            ROSE_ASSERT(get_mapped_size()==get_size());
            set_mapped_size(nentries*entry_size);
        }
        set_size(nentries*entry_size);
    } else if (nentries*entry_size > get_size()) {
        /* We should have detected this before unparsing! */
        ROSE_ASSERT(!"can't expand symbol section while unparsing!");
    }

    /* Adjust the entry size stored in the ELF Section Table */
    get_section_entry()->set_sh_entsize(entry_size);

    /* Write each entry's required part followed by the optional part */
    for (size_t i=0; i < p_symbols->get_symbols().size(); i++) {
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

/* Print some debugging info */
void
SgAsmElfSymbolSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymbolSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymbolSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmElfSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu symbols\n", p, w, "ElfSymbol.size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmGenericSection *section = get_file()->get_section_by_id(p_symbols->get_symbols()[i]->get_st_shndx());
        p_symbols->get_symbols()[i]->dump(f, p, i, section);
    }

    hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Returns true if a cursory look at the file indicates that it could be an ELF file. */
bool
SgAsmElfFileHeader::is_ELF(SgAsmGenericFile *f)
{
    SgAsmElfFileHeader *hdr = NULL;

    bool retval = false;

    ROSE_ASSERT(f != NULL);
    
    try {
        hdr = new SgAsmElfFileHeader(f, 0);
        retval = true;
    } catch (...) {
        /* cleanup is below */
    }

    delete hdr;
    return retval;
}

/* Parses the structure of an ELF file and adds the info to the ExecFile */
SgAsmElfFileHeader *
SgAsmElfFileHeader::parse(SgAsmGenericFile *ef)
{
    ROSE_ASSERT(ef);
    
    SgAsmElfFileHeader *fhdr = new SgAsmElfFileHeader(ef, 0);
    ROSE_ASSERT(fhdr != NULL);

    /* Read the optional section and segment tables and the sections to which they point. */
    if (fhdr->get_e_shnum())
        fhdr->set_section_table( new SgAsmElfSectionTable(fhdr) );
    if (fhdr->get_e_phnum())
        fhdr->set_segment_table( new SgAsmElfSegmentTable(fhdr) );

    /* Associate the entry point with a particular section. */
    ROSE_ASSERT(fhdr->get_entry_rvas().size()==1);
    addr_t entry_va = fhdr->get_entry_rvas()[0].get_rva() + fhdr->get_base_va();
    SgAsmGenericSection *secbind = fhdr->get_best_section_by_va(entry_va);
    fhdr->get_entry_rvas()[0].set_section(secbind);
    ROSE_ASSERT(fhdr->get_entry_rvas()[0].get_section()==secbind);

    /* Use symbols from either ".symtab" or ".dynsym" */
    SgAsmElfSymbolSection *symtab = dynamic_cast<SgAsmElfSymbolSection*>(ef->get_section_by_name(".symtab"));
    if (!symtab)
        symtab = dynamic_cast<SgAsmElfSymbolSection*>(ef->get_section_by_name(".dynsym"));
    if (symtab) {
        std::vector<SgAsmElfSymbol*> & symbols = symtab->get_symbols()->get_symbols();
        for (size_t i=0; i<symbols.size(); i++)
            fhdr->add_symbol(symbols[i]);
    }

    return fhdr;
}
