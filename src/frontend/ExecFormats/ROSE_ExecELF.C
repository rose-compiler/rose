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

/** Construct a new ELF File Header with default values. The new section is placed at file offset zero and the size is
 *  initially one byte (calling parse() will extend it as necessary). Setting the initial size of non-parsed sections to a
 *  positive value works better when adding sections to the end-of-file since the sections will all have different starting
 *  offsets and therefore SgAsmGenericFile::shift_extend will know what order the sections should be in when they are
 *  eventually resized. */
void
SgAsmElfFileHeader::ctor()
{
    ROSE_ASSERT(get_file()!=NULL);
    ROSE_ASSERT(get_size()>0);

    set_name(new SgAsmBasicString("ELF File Header"));
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* Magic number */
    p_magic.clear();
    p_magic.push_back(0x7f);
    p_magic.push_back('E');
    p_magic.push_back('L');
    p_magic.push_back('F');

    /* Executable Format */
    ROSE_ASSERT(p_exec_format!=NULL);
    p_exec_format->set_family(FAMILY_ELF);
    p_exec_format->set_purpose(PURPOSE_EXECUTABLE);
    p_exec_format->set_sex(ORDER_LSB);
    p_exec_format->set_word_size(4);
    p_exec_format->set_version(1);
    p_exec_format->set_is_current_version(true);
    p_exec_format->set_abi(ABI_UNSPECIFIED);
    p_exec_format->set_abi_version(0);

    p_isa = ISA_IA32_386;
    p_e_ident_data_encoding = 1;  /*LSB*/
    p_e_ident_padding = SgUnsignedCharList(9, '\0');
}

/** Return true if the file looks like it might be an ELF file according to the magic number. */
bool
SgAsmElfFileHeader::is_ELF(SgAsmGenericFile *ef)
{
    SgAsmElfFileHeader *fhdr = NULL;
    bool retval = false;
    try {
        fhdr = new SgAsmElfFileHeader(ef);
        fhdr->grab_content();
        fhdr->extend(4);
        unsigned char magic[4];
        fhdr->content(0, 4, magic);
        retval = 0x7f==magic[0] && 'E'==magic[1] && 'L'==magic[2] && 'F'==magic[3];
    } catch (...) {
        /* cleanup is below */
    }

    delete fhdr;
    return retval;
}

/** Initialize this header with information parsed from the file and construct and parse everything that's reachable from the
 *  header. Since the size of the ELF File Header is determined by the contents of the ELF File Header as stored in the file,
 *  the size of the ELF File Header will be adjusted upward if necessary. The ELF File Header should have been constructed
 *  such that SgAsmElfFileHeader::ctor() was called. */
SgAsmElfFileHeader*
SgAsmElfFileHeader::parse()
{
    SgAsmGenericHeader::parse();

    /* Read 32-bit header for now. Might need to re-read as 64-bit later. */
    Elf32FileHeader_disk disk32;
    if (sizeof(disk32)>get_size())
        extend(sizeof(disk32)-get_size());
    content(0, sizeof(disk32), &disk32);

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
    p_e_ident_data_encoding = disk32.e_ident_data_encoding; /*save original value*/

    /* Decode header to native format */
    rva_t entry_rva, sectab_rva, segtab_rva;
    if (1 == disk32.e_ident_file_class) {
        p_exec_format->set_word_size(4);

	p_e_ident_padding.clear();
        for (size_t i=0; i<sizeof(disk32.e_ident_padding); i++)
             p_e_ident_padding.push_back(disk32.e_ident_padding[i]);

        p_e_ident_file_class    = disk_to_host(sex, disk32.e_ident_file_class);
        p_e_ident_file_version  = disk_to_host(sex, disk32.e_ident_file_version);
        p_e_type                = disk_to_host(sex, disk32.e_type);
        p_e_machine             = disk_to_host(sex, disk32.e_machine);
	p_exec_format->set_version(disk_to_host(sex, disk32.e_version));
        entry_rva               = disk_to_host(sex, disk32.e_entry);
        segtab_rva              = disk_to_host(sex, disk32.e_phoff);
        sectab_rva              = disk_to_host(sex, disk32.e_shoff);
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
        if (sizeof(disk64)>get_size())
            extend(sizeof(disk64)-get_size());
        content(0, sizeof disk64, &disk64);

	p_e_ident_padding.clear();
        for (size_t i=0; i<sizeof(disk64.e_ident_padding); i++)
             p_e_ident_padding.push_back(disk64.e_ident_padding[i]);

        p_e_ident_file_class    = disk_to_host(sex, disk64.e_ident_file_class);
        p_e_ident_file_version  = disk_to_host(sex, disk64.e_ident_file_version);
        p_e_type                = disk_to_host(sex, disk64.e_type);
        p_e_machine             = disk_to_host(sex, disk64.e_machine);
	p_exec_format->set_version(disk_to_host(sex, disk64.e_version));
        entry_rva               = disk_to_host(sex, disk64.e_entry);
        segtab_rva              = disk_to_host(sex, disk64.e_phoff);
        sectab_rva              = disk_to_host(sex, disk64.e_shoff);
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
    p_magic.clear();
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
      case 20:
        // Note that PowerPC has: p_e_machine = 20 = 0x14, using both gcc on BGL and xlc on BGL.
        // However, these don't seem like correct values for PowerPC.
        set_isa(ISA_PowerPC);
        break;
      case 40:
        set_isa(ISA_ARM_Family);
        break;
      case 62:
        set_isa(ISA_X8664_Family);
        break;
      default:
        /*FIXME: There's a whole lot more. See Dan's Elf reader. */
        // DQ (10/12/2008): Need more information to address PowerPC support.
        fprintf(stderr, "Warning: SgAsmElfFileHeader::parse::p_e_machine = 0x%lx (%lu)\n", p_e_machine, p_e_machine);
        set_isa(ISA_OTHER);
        break;
    }

    /* Target architecture */
    /*FIXME*/

    /* Read the optional section and segment tables and the sections to which they point. An empty section or segment table is
     * treated as if it doesn't exist. This seems to be compatible with the loader since the 45-bit "tiny" ELF executable
     * stores a zero in the e_shnum member and a completely invalid value in the e_shoff member. */
    if (sectab_rva>0 && get_e_shnum()>0) {
        SgAsmElfSectionTable *tab = new SgAsmElfSectionTable(this);
        tab->set_offset(sectab_rva.get_rva());
        tab->parse();
    }
    if (segtab_rva>0 && get_e_phnum()>0) {
        SgAsmElfSegmentTable *tab = new SgAsmElfSegmentTable(this);
        tab->set_offset(segtab_rva.get_rva());
        tab->parse();
    }
    
    /* Associate the entry point with a particular section. */
    entry_rva.bind(this);
    add_entry_rva(entry_rva);

    /* Use symbols from either ".symtab" or ".dynsym" */
    SgAsmElfSymbolSection *symtab = dynamic_cast<SgAsmElfSymbolSection*>(get_section_by_name(".symtab"));
    if (!symtab)
        symtab = dynamic_cast<SgAsmElfSymbolSection*>(get_section_by_name(".dynsym"));
    if (symtab) {
        std::vector<SgAsmElfSymbol*> & symbols = symtab->get_symbols()->get_symbols();
        for (size_t i=0; i<symbols.size(); i++)
            add_symbol(symbols[i]);
    }
    
    return this;
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

/* Get the list of sections defined in the ELF Section Table */
SgAsmGenericSectionPtrList
SgAsmElfFileHeader::get_sectab_sections()
{
    SgAsmGenericSectionPtrList retval;
    SgAsmGenericSectionPtrList sections = get_sections()->get_sections();
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmElfSection *elfsec = dynamic_cast<SgAsmElfSection*>(sections[i]);
        if (elfsec && elfsec->get_section_entry()!=NULL)
            retval.push_back(elfsec);
    }
    return retval;
}

/* Get the list of sections defined in the ELF Segment Table */
SgAsmGenericSectionPtrList
SgAsmElfFileHeader::get_segtab_sections()
{
    SgAsmGenericSectionPtrList retval;
    SgAsmGenericSectionPtrList sections = get_sections()->get_sections();
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmElfSection *elfsec = dynamic_cast<SgAsmElfSection*>(sections[i]);
        if (elfsec && elfsec->get_segment_entry()!=NULL)
            retval.push_back(elfsec);
    }
    return retval;
}

/* Encode Elf header disk structure */
void *
SgAsmElfFileHeader::encode(ByteOrder sex, Elf32FileHeader_disk *disk) const
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

    /* Byte order. According to the spec, valid values are 1 (little-endian) and 2 (big-endian). However, we've seen cases
     * where a value of zero is used to indicate "native" order (loader assumes words are in the order of the machine on which
     * the loader is running, and the ROSE ELF parser determines the order by looking at other fields in the header). Any
     * original value other than 1 or 2 will be written to the new output; otherwise we choose 1 or 2 based on the currently
     * defined byte order. */
    unsigned data_encoding;
    if (p_e_ident_data_encoding==1 || p_e_ident_data_encoding==2) {
        data_encoding = ORDER_LSB==get_sex() ? 1 : 2;
    } else {
        data_encoding = p_e_ident_data_encoding;
    }
    host_to_disk(sex, data_encoding, &(disk->e_ident_data_encoding));

    host_to_disk(sex, p_e_ident_file_version,  &(disk->e_ident_file_version));
    ROSE_ASSERT(p_e_ident_padding.size() == NELMTS(disk->e_ident_padding));
    for (size_t i=0; i<NELMTS(disk->e_ident_padding); i++)
        disk->e_ident_padding[i] = p_e_ident_padding[i];
    host_to_disk(sex, p_e_type,                &(disk->e_type));
    host_to_disk(sex, p_e_machine,             &(disk->e_machine));
    host_to_disk(sex, p_exec_format->get_version(), &(disk->e_version));
    host_to_disk(sex, get_entry_rva(),         &(disk->e_entry));
    if (get_segment_table()) {
        host_to_disk(sex, get_segment_table()->get_offset(), &(disk->e_phoff));
    } else {
        host_to_disk(sex, 0, &(disk->e_phoff));
    }
    if (get_section_table()) {
        host_to_disk(sex, get_section_table()->get_offset(), &(disk->e_shoff));
    } else {
        host_to_disk(sex, 0, &(disk->e_shoff));
    }
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
SgAsmElfFileHeader::encode(ByteOrder sex, Elf64FileHeader_disk *disk) const
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
    if (get_segment_table()) {
        host_to_disk(sex, get_segment_table()->get_offset(), &(disk->e_phoff));
    } else {
        host_to_disk(sex, 0, &(disk->e_phoff));
    }
    if (get_section_table()) {
        host_to_disk(sex, get_section_table()->get_offset(), &(disk->e_shoff));
    } else {
        host_to_disk(sex, 0, &(disk->e_shoff));
    }
    host_to_disk(sex, p_e_flags,               &(disk->e_flags));
    host_to_disk(sex, p_e_ehsize,              &(disk->e_ehsize));
    host_to_disk(sex, p_phextrasz+sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk), &(disk->e_phentsize));
    host_to_disk(sex, p_e_phnum,               &(disk->e_phnum));
    host_to_disk(sex, p_shextrasz+sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk), &(disk->e_shentsize));
    host_to_disk(sex, p_e_shnum,               &(disk->e_shnum));
    host_to_disk(sex, p_e_shstrndx,            &(disk->e_shstrndx));

    return disk;
}

/* Update prior to parsing */
bool
SgAsmElfFileHeader::reallocate()
{

    /* Reallocate superclass. This also calls reallocate() for all the sections associated with this ELF File Header. */
    bool reallocated = SgAsmGenericHeader::reallocate();

    /* Resize header based on current word size */
    addr_t need;
    if (4==get_word_size()) {
        need = sizeof(Elf32FileHeader_disk);
    } else if (8==get_word_size()) {
        need = sizeof(Elf64FileHeader_disk);
    } else {
        throw FormatError("unsupported ELF word size");
    }
    if (need < get_size()) {
        if (is_mapped()) {
            ROSE_ASSERT(get_mapped_size()==get_size());
            set_mapped_size(need);
        }
        set_size(need);
        reallocated = true;
    } else if (need > get_size()) {
        get_file()->shift_extend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    return reallocated;
}

/* Write ELF contents back to a file. */
void
SgAsmElfFileHeader::unparse(std::ostream &f) const
{
    /* Write unreferenced areas back to the file before anything else. */
    unparse_holes(f);

    /* Write the ELF segment table and segments first since they generally overlap with more specific things which may have
     * been modified when walking the AST. (We generally don't modify segments, just the more specific sections.) */
    if (p_segment_table) {
        ROSE_ASSERT(p_segment_table->get_header()==this);
        p_segment_table->unparse(f);
    }

    /* Write the ELF section table and, indirectly, the sections themselves. */
    if (p_section_table) {
        ROSE_ASSERT(p_section_table->get_header()==this);
        p_section_table->unparse(f);
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
SgAsmElfFileHeader::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfFileHeader.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmGenericHeader::dump(f, p, -1);
    const char *class_s = 1==p_e_ident_file_class ? " (32-bit)" :
                          2==p_e_ident_file_class ? " (64-bit)" : "";
    fprintf(f, "%s%-*s = %u%s\n",                           p, w, "e_ident_file_class",     p_e_ident_file_class, class_s);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_file_version",   p_e_ident_file_version);
    for (size_t i=0; i < p_e_ident_padding.size(); i++)
        fprintf(f, "%s%-*s = [%zu] %u\n",                   p, w, "e_ident_padding",     i, p_e_ident_padding[i]);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_type",                 p_e_type);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_machine",              p_e_machine);
    fprintf(f, "%s%-*s = 0x%08lx\n",                        p, w, "e_flags",                p_e_flags);
    fprintf(f, "%s%-*s = 0x%08lx (%lu) bytes\n",            p, w, "e_ehsize",               p_e_ehsize, p_e_ehsize);
    fprintf(f, "%s%-*s = 0x%08lx (%lu) bytes\n",            p, w, "phextrasz",              p_phextrasz, p_phextrasz);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_phnum",                p_e_phnum);
    fprintf(f, "%s%-*s = 0x%08lx (%lu) bytes\n",            p, w, "shextrasz",              p_shextrasz, p_shextrasz);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_shnum",                p_e_shnum);
    fprintf(f, "%s%-*s = %lu\n",                            p, w, "e_shstrndx",             p_e_shstrndx);
    if (p_section_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n",                p, w, "section_table",
                p_section_table->get_id(), p_section_table->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",                       p, w, "section_table");
    }
    if (p_segment_table) {
        fprintf(f, "%s%-*s = [%d] \"%s\"\n",                p, w, "segment_table",
                p_segment_table->get_id(), p_segment_table->get_name()->c_str());
    } else {
        fprintf(f, "%s%-*s = none\n",                       p, w, "segment_table");
    }

    if (variantT() == V_SgAsmElfFileHeader) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sections
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor for sections that are in neither the ELF Section Table nor the ELF Segment Table yet (but eventually will be) */
void
SgAsmElfSection::ctor()
{
    set_synthesized(false);
    set_purpose(SP_UNSPECIFIED);
}

/** Initializes the section from data parsed from the ELF Section Table. This includes the section name, offset, size, memory
 *  mapping, and alignments. The @p id is the index into the section table. This function complements
 *  SgAsmElfSectionTable::add_section(): this function initializes this section from the section table while add_section()
 *  initializes the section table from the section. */
SgAsmElfSection *
SgAsmElfSection::init_from_section_table(SgAsmElfSectionTableEntry *shdr, SgAsmElfStringSection *strsec, int id)
{
    ROSE_ASSERT(shdr);
    ROSE_ASSERT(strsec);
    ROSE_ASSERT(id>=0);

    /* Purpose */
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

    /* File mapping */
    set_offset(shdr->get_sh_offset());
    if (SgAsmElfSectionTableEntry::SHT_NULL==shdr->get_sh_type() ||
        SgAsmElfSectionTableEntry::SHT_NOBITS==shdr->get_sh_type()) {
        set_size(0);
    } else {
        set_size(shdr->get_sh_size());
    }
    set_file_alignment(shdr->get_sh_addralign());
    p_data = get_file()->content(get_offset(), get_size());

    /* Memory mapping */
    if (shdr->get_sh_addr() > 0) {
        set_mapped_rva(shdr->get_sh_addr());
        set_mapped_size(shdr->get_sh_size());
        set_mapped_rperm(true);
        set_mapped_wperm((shdr->get_sh_flags() & 0x01) == 0x01);
        set_mapped_xperm((shdr->get_sh_flags() & 0x04) == 0x04);
        set_mapped_alignment(shdr->get_sh_addralign());
    } else {
        set_mapped_rva(0);
        set_mapped_size(0);
        set_mapped_rperm(false);
        set_mapped_wperm(false);
        set_mapped_xperm(false);
        set_mapped_alignment(shdr->get_sh_addralign());
    }

    /* Name. This has to be near the end because possibly strsec==this, in which case we have to call set_size() to extend the
     * section to be large enough before we can try to look up the name. */
    set_id(id);
    set_name(new SgAsmStoredString(strsec->get_strtab(), shdr->get_sh_name()));

    /* Add section table entry to section */
    set_section_entry(shdr);
    shdr->set_parent(this);

    return this;
}

/** Initializes the section from data parse from the ELF Segment Table similar to init_from_section_table() */
SgAsmElfSection *
SgAsmElfSection::init_from_segment_table(SgAsmElfSegmentTableEntry *shdr, bool mmap_only)
{
    if (!mmap_only) {
        /* Purpose */
        set_purpose(SP_HEADER);

        /* File mapping */
        set_offset(shdr->get_offset());
        set_size(shdr->get_filesz());
        set_file_alignment(shdr->get_align());
        p_data = get_file()->content(get_offset(), get_size());
    
        /* Name */
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
    }
    
    /* Memory mapping */
    set_mapped_rva(shdr->get_vaddr());
    set_mapped_size(shdr->get_memsz());
    set_mapped_alignment(shdr->get_align());
    set_mapped_rperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_RPERM ? true : false);
    set_mapped_wperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_WPERM ? true : false);
    set_mapped_xperm(shdr->get_flags() & SgAsmElfSegmentTableEntry::PF_XPERM ? true : false);

    /* Add segment table entry to section */
    set_segment_entry(shdr);
    shdr->set_parent(this);

    return this;
}

/* Just a convenience function so we don't need to constantly cast the return value from get_header() */
SgAsmElfFileHeader*
SgAsmElfSection::get_elf_header() const
{
    return dynamic_cast<SgAsmElfFileHeader*>(get_header());
}

/* Returns info about the size of the entries based on information already available. Any or all arguments may be null
 * pointers if the caller is not interested in the value. Return values are:
 *
 *   entsize  - size of each entry, sum of required and optional parts. This comes from the sh_entsize member of this
 *              section's ELF Section Table Entry, adjusted upward to be large enough to hold the required part of each
 *              entry (see "required").
 *
 *   required - size of the required (leading) part of each entry. The size of the required part is based on the ELF word size.
 *
 *   optional - size of the optional (trailing) part of each entry. If the section has been parsed then the optional size will
 *              be calculated from the entry with the largest "extra" (aka, optional) data. Otherwise this is calculated as the
 *              difference between the "entsize" and the "required" sizes.
 *   
 *   entcount - total number of entries in this section. If the section has been parsed then this is the actual number of
 *              parsed entries, otherwise its the section size divided by the "entsize".
 *
 * Return value is the total size needed for the section. In all cases, it is entsize*entcount.
 */
rose_addr_t
SgAsmElfSection::calculate_sizes(size_t r32size, size_t r64size,       /*size of required parts*/
                                 const std::vector<size_t> &optsizes,  /*size of optional parts and number of parts parsed*/
                                 size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    size_t struct_size = 0;
    size_t extra_size = 0;
    size_t entry_size = 0;
    size_t nentries = 0;
    SgAsmElfFileHeader *fhdr = get_elf_header();

    /* Assume ELF Section Table Entry is correct for now for the size of each entry in the table. */
    ROSE_ASSERT(get_section_entry()!=NULL);
    entry_size = get_section_entry()->get_sh_entsize();

    /* Size of required part of each entry */
    if (0==r32size && 0==r64size) {
        /* Probably called by four-argument SgAsmElfSection::calculate_sizes and we don't know the sizes of the required parts
         * because there isn't a parser for this type of section, or the section doesn't contain a table. In the latter case
         * the ELF Section Table has a zero sh_entsize and we'll treat the section as if it were a table with one huge entry.
         * Otherwise we'll assume that the struct size is the same as the sh_entsize and there's no optional data. */
        struct_size = entry_size>0 ? entry_size : get_size();
    } else if (4==fhdr->get_word_size()) {
        struct_size = r32size;
    } else if (8==fhdr->get_word_size()) {
        struct_size = r64size;
    } else {
        throw FormatError("bad ELF word size");
    }

    /* Entire entry should be at least large enough for the required part. This also takes care of the case when the ELF
     * Section Table Entry has a zero-valued sh_entsize */
    entry_size = std::max(entry_size, struct_size);

    /* Size of optional parts. If we've parsed the table then use the largest optional part, otherwise assume the entry from
     * the ELF Section Table is correct. */
    nentries = optsizes.size();
    if (nentries>0) {
        for (size_t i=0; i<nentries; i++) {
            extra_size = std::max(extra_size, optsizes[i]);
        }
        entry_size = std::min(entry_size, struct_size+extra_size);
    } else {
        extra_size = entry_size - struct_size;
        nentries = entry_size>0 ? get_size() / entry_size : 0;
    }

    /* Return values */
    if (entsize)
        *entsize = entry_size;
    if (required)
        *required = struct_size;
    if (optional)
        *optional = extra_size;
    if (entcount)
        *entcount = nentries;
    return entry_size * nentries;
}

/* Most subclasses will override this virtual function in order to return more useful values. This implementation returns the
 * following values:
 *   entsize  -- size stored in the ELF Section Table's sh_entsize member, or size of entire section if not a table.
 *   required -- same as entsize
 *   optional -- zero
 *   entcount -- number of entries, each of size entsize, that can fit in the section.
 * The return size is entsize*entcount, which, if this section is a table (nonzero sh_entsize), could be smaller than the
 * total size of the section. */
rose_addr_t
SgAsmElfSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    return calculate_sizes(0, 0, std::vector<size_t>(), entsize, required, optional, entcount);
}

/* Called prior to unparse to make things consistent. */
bool
SgAsmElfSection::reallocate()
{
    bool reallocated = false;
    SgAsmElfSectionTableEntry *sechdr = get_section_entry();
    SgAsmElfSegmentTableEntry *seghdr = get_segment_entry();

    /* Change section size if this section was defined in the ELF Section Table */
    if (sechdr!=NULL) {
        addr_t need = calculate_sizes(NULL, NULL, NULL, NULL);
        if (need < get_size()) {
            if (is_mapped()) {
                ROSE_ASSERT(get_mapped_size()==get_size());
                set_mapped_size(need);
            }
            set_size(need);
            reallocated = true;
        } else if (need > get_size()) {
            get_file()->shift_extend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
            reallocated = true;
        }
    }

    /* Update entry in the ELF Section Table and/or ELF Segment Table */
    if (sechdr)
        sechdr->update_from_section(this);
    if (seghdr)
        seghdr->update_from_section(this);
    
    return reallocated;
}

/* Print some debugging info */
void
SgAsmElfSection::dump(FILE *f, const char *prefix, ssize_t idx) const
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

    if (variantT() == V_SgAsmElfSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Table Sections
//
//    SgAsmElfStringSection is derived from SgAsmElfSection, which is derived in turn from SgAsmGenericSection. An ELF String
//    Table Section points to the ELF String Table (SgAsmElfStrtab) that is contained in the section.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Non-parsing constructor. The ELF String Table is constructed to contain one NUL byte. */
void
SgAsmElfStringSection::ctor()
{
    get_name()->set_string("ELF String Table");
    if (get_size()==0)
        set_size(1);
    p_strtab = new SgAsmElfStrtab(this);
}

/* Parse the file content to initialize the string table */
SgAsmElfStringSection *
SgAsmElfStringSection::parse()
{
    SgAsmElfSection::parse();
    ROSE_ASSERT(p_strtab);
    p_strtab->get_freelist().clear(); /*because set_size() during construction added to the free list*/
    p_strtab->parse();
    return this;
}

/* Reallocate space for the string section if necessary. Note that reallocation is lazy here -- we don't shrink the section,
 * we only enlarge it (if you want the section to shrink then call SgAsmGenericStrtab::reallocate(bool) with a true value
 * rather than calling this function. SgAsmElfStringSection::reallocate is called in response to unparsing a file and gives
 * the string table a chance to extend its container section if it needs to allocate more space for strings. */
bool
SgAsmElfStringSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();
    if (get_strtab()->reallocate(false))
        reallocated = true;
    return reallocated;
}

/* Unparse an ElfStringSection by unparsing the ElfStrtab */
void
SgAsmElfStringSection::unparse(std::ostream &f) const
{
    get_strtab()->unparse(f);
    unparse_holes(f);
}

/** Augments superclass to make sure free list and such are adjusted properly. Any time the ELF String Section size is changed
 *  we adjust the free list in the ELF String Table contained in this section. */
void
SgAsmElfStringSection::set_size(addr_t newsize)
{
    addr_t orig_size = get_size();
    SgAsmElfSection::set_size(newsize);
    SgAsmGenericStrtab *strtab = get_strtab();

    if (strtab) {
        if (get_size() > orig_size) {
            /* Add new address space to string table free list */
            addr_t n = get_size() - orig_size;
            strtab->get_freelist().insert(orig_size, n);
        } else if (get_size() < orig_size) {
            /* Remove deleted address space from string table free list */
            addr_t n = orig_size - get_size();
            strtab->get_freelist().erase(get_size(), n);
        }
    }
}

/* Print some debugging info */
void
SgAsmElfStringSection::dump(FILE *f, const char *prefix, ssize_t idx) const
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

    if (variantT() == V_SgAsmElfStringSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// String Tables
//
//    An SgAsmElfStrtab is an ELF String Table, inheriting from SgAsmGenericStrtab. String tables point to the
//    SgAsmGenericSection that contains them. In the case of SgAsmElfStrtab it points to an SgAsmElfStringSection.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Non-parsing constructor. The table is created to be at least one byte long and having a NUL character as the first byte. */
void
SgAsmElfStrtab::ctor()
{
    ROSE_ASSERT(get_container());
    if (get_container()->get_size()==0)
        get_container()->set_size(1);
    p_dont_free = create_storage(0, false);
}

/* Parses the string table. All that actually happens at this point is we look to see if the table begins with an empty
 * string. */
SgAsmElfStrtab *
SgAsmElfStrtab::parse()
{
    SgAsmGenericStrtab::parse();
    ROSE_ASSERT(get_container());
    if (get_container()->get_size()>0) {
        if (get_container()->content(0, 1)[0]=='\0') {
            if (p_dont_free) {
                ROSE_ASSERT(0==p_dont_free->get_offset());
            } else {
                p_dont_free = create_storage(0, false);
            }
        } else if (p_dont_free) {
            p_dont_free = NULL;
        }
    }
    return this;
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
        storage->set_offset(SgAsmGenericString::unallocated);
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
    ROSE_ASSERT(offset!=SgAsmGenericString::unallocated);

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
    SgAsmStringStorage *storage = NULL;
    if (0==offset && 0==get_container()->get_data().size()) {
        ROSE_ASSERT(get_container()->get_size()>=1);
        storage = new SgAsmStringStorage(this, "", 0);
    } else {
        std::string s = get_container()->content_str(offset);
        storage = new SgAsmStringStorage(this, s, offset);
    }

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
    set_isModified(true);
    return storage;
}

/* Returns the number of bytes required to store the string in the string table. This is the length of the string plus
 * one for the NUL terminator. */
rose_addr_t
SgAsmElfStrtab::get_storage_size(const SgAsmStringStorage *storage) {
    return storage->get_string().size() + 1;
}

/* Tries to find a suitable offset for a string such that it overlaps with some other string already allocated. If the new
 * string is the same as the end of some other string (new="main", existing="domain") then we just use an offset into that
 * string since the space is already allocated for the existing string. If the new string ends with an existing string
 * (new="domain", existing="main") and there's enough free space before the existing string (two bytes in this case) then
 * we allocate some of that free space and use a suitable offset. In any case, upon return storage->get_offset() will return
 * the allocated offset if successful, or SgAsmGenericString::unallocated if we couldn't find an overlap. */
void
SgAsmElfStrtab::allocate_overlap(SgAsmStringStorage *storage)
{
    ROSE_ASSERT(storage->get_offset()==SgAsmGenericString::unallocated);
    size_t need = storage->get_string().size();
    for (size_t i=0; i<p_storage_list.size(); i++) {
        SgAsmStringStorage *existing = p_storage_list[i];
        if (existing->get_offset()!=SgAsmGenericString::unallocated) {
            size_t have = existing->get_string().size();
            if (need<=have && 0==existing->get_string().compare(have-need, need, storage->get_string())) {
                /* An existing string ends with the new string. */
                storage->set_offset(existing->get_offset() + (have-need));
                return;
            } else if (need>have && existing->get_offset()>=need-have &&
                       0==storage->get_string().compare(need-have, have, existing->get_string())) {
                /* New string ends with an existing string. Check for, and allocate, free space. */
                addr_t offset = existing->get_offset() - (need-have); /* positive diffs checked above */
                if (get_freelist().subtract_from(offset, need-have).size()==0) {
                    get_freelist().allocate_at(offset, need-have);
                    storage->set_offset(offset);
                    return;
                }
            }
        }
    }
}

/* Write string table back to disk. Free space is zeroed out; holes are left as they are. */
void
SgAsmElfStrtab::unparse(std::ostream &f) const
{
    SgAsmGenericSection *container = get_container();
    
    /* Write strings with NUL termination. Shared strings will be written more than once, but that's OK. */
    for (size_t i=0; i<p_storage_list.size(); i++) {
        SgAsmStringStorage *storage = p_storage_list[i];
        ROSE_ASSERT(storage->get_offset()!=SgAsmGenericString::unallocated);
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
    p_sh_type      = (SectionType)disk_to_host(sex, disk->sh_type);
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
    p_sh_type      = (SectionType)disk_to_host(sex, disk->sh_type);
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
SgAsmElfSectionTableEntry::encode(ByteOrder sex, Elf32SectionTableEntry_disk *disk) const
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
SgAsmElfSectionTableEntry::encode(ByteOrder sex, Elf64SectionTableEntry_disk *disk) const
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

/** Non-parsing constructor for an ELF Section Table */
void
SgAsmElfSectionTable::ctor()
{
    /* There can be only one ELF Section Table */
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr);
    ROSE_ASSERT(fhdr->get_section_table()==NULL);

    set_synthesized(true);                              /* the section table isn't really a section itself */
    set_name(new SgAsmBasicString("ELF Section Table"));
    set_purpose(SP_HEADER);

    /* Every section table has a first entry that's all zeros. We don't declare that section here (see parse()) but we do set
     * the section count in the header in order to reserve that first slot. */
    if (fhdr->get_e_shnum()<1)
        fhdr->set_e_shnum(1);

    fhdr->set_section_table(this);
}
    
/** Parses an ELF Section Table and constructs and parses all sections reachable from the table. The section is extended as
 *  necessary based on the number of entries and the size of each entry. */
SgAsmElfSectionTable *
SgAsmElfSectionTable::parse()
{
    SgAsmGenericSection::parse();

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();

    size_t ent_size, struct_size, opt_size, nentries;
    calculate_sizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(opt_size==fhdr->get_shextrasz() && nentries==fhdr->get_e_shnum());

    /* If the current size is very small (0 or 1 byte) then we're coming straight from the constructor and the parsing should
     * also extend this section to hold all the entries. Otherwise the caller must have assigned a specific size for a good
     * reason and we should leave that alone, reading zeros if the entries extend beyond the defined size. */
    if (get_size()<=1 && get_size()<nentries*ent_size)
        extend(nentries*ent_size - get_size());

    /* Read all the section headers. */
    std::vector<SgAsmElfSectionTableEntry*> entries;
    addr_t offset = 0;
    for (size_t i=0; i<nentries; i++, offset+=ent_size) {
        SgAsmElfSectionTableEntry *shdr = NULL;
        if (4 == fhdr->get_word_size()) {
            SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk disk;
            content(offset, struct_size, &disk);
            shdr = new SgAsmElfSectionTableEntry(sex, &disk);
        } else {
            SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk disk;
            content(offset, struct_size, &disk);
            shdr = new SgAsmElfSectionTableEntry(sex, &disk);
        }
        if (opt_size>0)
            shdr->get_extra() = content_ucl(offset+struct_size, opt_size);
        entries.push_back(shdr);
    }

    /* Read the string table section first because we'll need this to initialize section names.  */
    SgAsmElfStringSection *strsec = NULL;
    if (fhdr->get_e_shstrndx() > 0) {
        SgAsmElfSectionTableEntry *shdr = entries[fhdr->get_e_shstrndx()];
        strsec = new SgAsmElfStringSection(fhdr);
        strsec->init_from_section_table(shdr, strsec, fhdr->get_e_shstrndx());
        strsec->parse();
    }

    /* Read all other sections */
    for (size_t i = 0; i<entries.size(); i++) {
        SgAsmElfSectionTableEntry *shdr = entries[i];
        SgAsmElfSection *section = NULL;
        if (i == fhdr->get_e_shstrndx()) continue; /*we already read string table*/
        switch (shdr->get_sh_type()) {
          case SgAsmElfSectionTableEntry::SHT_NULL:
            /* Null entry. We still create the section just to hold the section header. */
            section = new SgAsmElfSection(fhdr);
            break;
          case SgAsmElfSectionTableEntry::SHT_NOBITS:
            /* These types of sections don't occupy any file space (e.g., BSS) */
            section = new SgAsmElfSection(fhdr);
            break;
          case SgAsmElfSectionTableEntry::SHT_DYNAMIC:
            section = new SgAsmElfDynamicSection(fhdr);
            break;
          case SgAsmElfSectionTableEntry::SHT_DYNSYM:
          case SgAsmElfSectionTableEntry::SHT_SYMTAB:
            section = new SgAsmElfSymbolSection(fhdr);
            break;
          case SgAsmElfSectionTableEntry::SHT_STRTAB:
            section = (new SgAsmElfStringSection(fhdr));
            break;
          case SgAsmElfSectionTableEntry::SHT_REL: {
              SgAsmElfRelocSection *relocsec;
              section = relocsec = new SgAsmElfRelocSection(fhdr);
              relocsec->set_uses_addend(false);
              break;
          }
          case SgAsmElfSectionTableEntry::SHT_RELA: {
              SgAsmElfRelocSection *relocsec;
              section = relocsec = new SgAsmElfRelocSection(fhdr);
              relocsec->set_uses_addend(true);
              break;
          }
          default:
            section = new SgAsmElfSection(fhdr);
            break;
        }
        section->init_from_section_table(shdr, strsec, i);
        section->parse();
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
    return this;
}

/** Attaches a previously unattached ELF Section to the section table. If @p section is an  ELF String Section
 *  (SgAsmElfStringSection) that contains an ELF String Table (SgAsmElfStringTable) and the ELF Section Table has no
 *  associated string table then the @p section will be used as the string table to hold the section names.
 *
 *  This method complements SgAsmElfSection::init_from_section_table. This method initializes the section table from the
 *  section while init_from_section_table() initializes the section from the section table. */
void
SgAsmElfSectionTable::add_section(SgAsmElfSection *section)
{
    ROSE_ASSERT(section!=NULL);
    ROSE_ASSERT(section->get_file()==get_file());
    ROSE_ASSERT(section->get_header()==get_header());
    ROSE_ASSERT(section->get_section_entry()==NULL);            /* must not be in the section table yet */
    
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    /* Assign an ID if there isn't one yet */
    if (section->get_id()<0) {
        int id = fhdr->get_e_shnum();
        fhdr->set_e_shnum(id+1);
        section->set_id(id);
    }

    /* If the supplied section is a string table and the ELF Section Table doesn't have a string table associated with it yet,
     * then use the supplied section as the string table to hold the names of the sections. */
    SgAsmElfStringSection *strsec = NULL;
    if (fhdr->get_e_shstrndx()==0) {
        strsec = dynamic_cast<SgAsmElfStringSection*>(section);
        if (strsec) {
            fhdr->set_e_shstrndx(section->get_id());
        } else {
            throw FormatError("ELF Section Table must have an ELF String Section to store section names");
        }
    } else {
        strsec = dynamic_cast<SgAsmElfStringSection*>(fhdr->get_section_by_id(fhdr->get_e_shstrndx()));
        ROSE_ASSERT(strsec!=NULL);
    }

    /* Make sure the name is in the correct string table */
    std::string name;
    if (section->get_name()) {
        name = section->get_name()->get_string();
        section->get_name()->set_string(""); /*frees old string if stored*/
    }
    SgAsmStoredString *stored_name = new SgAsmStoredString(strsec->get_strtab(), 0);
    stored_name->set_string(name);
    section->set_name(stored_name);

    /* Create a new section table entry. */
    SgAsmElfSectionTableEntry *shdr = new SgAsmElfSectionTableEntry;
    shdr->update_from_section(section);
    section->set_section_entry(shdr);
}

/* Returns info about the size of the entries based on information already available. Any or all arguments may be null
 * pointers if the caller is not interested in the value. */
rose_addr_t
SgAsmElfSectionTable::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    size_t struct_size = 0;
    size_t extra_size = fhdr->get_shextrasz();
    size_t entry_size = 0;
    size_t nentries = 0;

    /* Size of required part of each entry */
    if (4==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk);
    } else if (8==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk);
    } else {
        throw FormatError("bad ELF word size");
    }

    /* Entire entry should be at least large enough for the required part. */
    entry_size = struct_size;

    /* Size of optional parts. If we've parsed the table then use the largest optional part, otherwise assume the entry from
     * the ELF File Header is correct. */
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmElfSection *elfsec = dynamic_cast<SgAsmElfSection*>(sections[i]);
        if (elfsec && elfsec->get_section_entry()) {
            ROSE_ASSERT(elfsec->get_id()>=0);
            nentries = std::max(nentries, (size_t)elfsec->get_id()+1);
            extra_size = std::max(extra_size, elfsec->get_section_entry()->get_extra().size());
        }
    }

    /* Total number of entries. Either we haven't parsed the section table yet (nor created the sections it defines) or we
     * have. In the former case we use the setting from the ELF File Header. Otherwise the table has to be large enough to
     * store the section with the largest ID (ID also serves as the index into the ELF Section Table). */
    if (0==nentries)
        nentries = fhdr->get_e_shnum();

    /* Return values */
    if (entsize)
        *entsize = entry_size;
    if (required)
        *required = struct_size;
    if (optional)
        *optional = extra_size;
    if (entcount)
        *entcount = nentries;
    return entry_size * nentries;
}

/** Update this section table entry with newer information from the section */
void
SgAsmElfSectionTableEntry::update_from_section(SgAsmElfSection *section)
{
    p_sh_name = dynamic_cast<SgAsmStoredString*>(section->get_name())->get_offset();

    set_sh_offset(section->get_offset());
    if (get_sh_type()==SHT_NOBITS && section->is_mapped()) {
        set_sh_size(section->get_mapped_size());
    } else {
        set_sh_size(section->get_size());
    }

    if (section->is_mapped()) {
        set_sh_addr(section->get_mapped_rva());
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
        set_sh_addr(0);
        p_sh_flags &= ~0x05; /*clear write & execute bits*/
    }
    
    SgAsmElfSection *linked_to = section->get_linked_section();
    if (linked_to) {
        ROSE_ASSERT(linked_to->get_id()>0);
        set_sh_link(linked_to->get_id());
    } else {
        set_sh_link(0);
    }
}

/* Change symbol to string */
const char *
SgAsmElfSectionTableEntry::to_string(SectionType t)
{
    switch (t) {
      case SHT_NULL:     return "SHT_NULL";
      case SHT_PROGBITS: return "SHT_PROGBITS";
      case SHT_SYMTAB:   return "SHT_SYMTAB";
      case SHT_STRTAB:   return "SHT_STRTAB";
      case SHT_RELA:     return "SHT_RELA";
      case SHT_HASH:     return "SHT_HASH";
      case SHT_DYNAMIC:  return "SHT_DYNAMIC";
      case SHT_NOTE:     return "SHT_NOTE";
      case SHT_NOBITS:   return "SHT_NOBITS";
      case SHT_REL:      return "SHT_REL";
      case SHT_SHLIB:    return "SHT_SHLIB";
      case SHT_DYNSYM:   return "SHT_DYNSYM";
      default:
        if (t>=SHT_LOPROC && t<=SHT_HIPROC) {
            return "processor-specific";
        } else if (t>=SHT_LOUSER && t<=SHT_HIUSER) {
            return "application-specific";
        } else {
            return "unknown";
        }
    }
}

/* Print some debugging info */
void
SgAsmElfSectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSectionTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %u bytes into strtab\n",                      p, w, "sh_name",        p_sh_name);
    fprintf(f, "%s%-*s = 0x%x (%d) %s\n",                              p, w, "sh_type", 
            p_sh_type, p_sh_type, to_string(p_sh_type));
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

/* Pre-unparsing updates */
bool
SgAsmElfSectionTable::reallocate()
{
    bool reallocated = false;

    /* Resize based on word size from ELF File Header */
    size_t opt_size, nentries;
    addr_t need = calculate_sizes(NULL, NULL, &opt_size, &nentries);
    if (need < get_size()) {
        if (is_mapped()) {
            ROSE_ASSERT(get_mapped_size()==get_size());
            set_mapped_size(need);
        }
        set_size(need);
        reallocated = true;
        
    } else if (need > get_size()) {
        get_file()->shift_extend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    /* Update data members in the ELF File Header. No need to return true for these changes. */
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    fhdr->set_shextrasz(opt_size);
    fhdr->set_e_shnum(nentries);

    return reallocated;
}

/* Write the section table section back to disk */
void
SgAsmElfSectionTable::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();
    SgAsmGenericSectionPtrList sections = fhdr->get_sectab_sections();

    /* Write the sections first */
    for (size_t i=0; i<sections.size(); i++)
        sections[i]->unparse(f);
    unparse_holes(f);

    /* Calculate sizes. The ELF File Header should have been updated in reallocate() prior to unparsing. */
    size_t ent_size, struct_size, opt_size, nentries;
    calculate_sizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(fhdr->get_shextrasz()==opt_size);
    ROSE_ASSERT(fhdr->get_e_shnum()==nentries);
    
    /* Write the section table entries */
    for (size_t i=0; i<sections.size(); ++i) {
        SgAsmElfSection *section = dynamic_cast<SgAsmElfSection*>(sections[i]);
        ROSE_ASSERT(section!=NULL);
        SgAsmElfSectionTableEntry *shdr = section->get_section_entry();
        ROSE_ASSERT(shdr!=NULL);
        ROSE_ASSERT(shdr->get_sh_offset()==section->get_offset()); /*section table entry should have been updated in reallocate()*/

        int id = section->get_id();
        ROSE_ASSERT(id>=0 && (size_t)id<nentries);

        SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk disk32;
        SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk disk64;
        void *disk  = NULL;

        if (4==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk32);
        } else if (8==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk64);
        } else {
            ROSE_ASSERT(!"invalid word size");
        }

        /* The disk struct */
        addr_t spos = write(f, id*ent_size, struct_size, disk);
        if (shdr->get_extra().size() > 0) {
            ROSE_ASSERT(shdr->get_extra().size()<=opt_size);
            write(f, spos, shdr->get_extra());
        }
    }
}

/* Print some debugging info */
void
SgAsmElfSectionTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSectionTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    if (variantT() == V_SgAsmElfSectionTable) //unless a base class
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
SgAsmElfSegmentTableEntry::encode(ByteOrder sex, Elf32SegmentTableEntry_disk *disk) const
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
SgAsmElfSegmentTableEntry::encode(ByteOrder sex, Elf64SegmentTableEntry_disk *disk) const
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

/** Update this segment table entry with newer information from the section */
void
SgAsmElfSegmentTableEntry::update_from_section(SgAsmElfSection *section)
{
    set_offset(section->get_offset());
    set_filesz(section->get_size());

    set_vaddr(section->get_mapped_va());
    set_memsz(section->get_mapped_size());
    if (section->get_mapped_rperm()) {
        set_flags((SegmentFlags)(p_flags | PF_RPERM));
    } else {
        set_flags((SegmentFlags)(p_flags & ~PF_RPERM));
    }
    if (section->get_mapped_wperm()) {
        set_flags((SegmentFlags)(p_flags | PF_WPERM));
    } else {
        set_flags((SegmentFlags)(p_flags & ~PF_WPERM));
    }
    if (section->get_mapped_xperm()) {
        set_flags((SegmentFlags)(p_flags | PF_XPERM));
    } else {
        set_flags((SegmentFlags)(p_flags & ~PF_XPERM));
    }
}

/* Print some debugging info */
void
SgAsmElfSegmentTableEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
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
      case SgAsmElfSegmentTableEntry::PT_NULL:    s = "PT_NULL";    break; // 0
      case SgAsmElfSegmentTableEntry::PT_LOAD:    s = "PT_LOAD";    break; // 1
      case SgAsmElfSegmentTableEntry::PT_DYNAMIC: s = "PT_DYNAMIC"; break; // 2
      case SgAsmElfSegmentTableEntry::PT_INTERP:  s = "PT_INTERP";  break; // 3
      case SgAsmElfSegmentTableEntry::PT_NOTE:    s = "PT_NOTE";    break; // 4
      case SgAsmElfSegmentTableEntry::PT_SHLIB:   s = "PT_SHLIB";   break; // 5
      case SgAsmElfSegmentTableEntry::PT_PHDR:    s = "PT_PHDR";    break; // 6

      // DQ (10/31/2008): Added mising enum values to prevent run-time warnings
      /* OS- and Processor-specific ranges */
      case SgAsmElfSegmentTableEntry::PT_LOOS: s = "PT_LOOS";  break; // 0x60000000, Values reserved for OS-specific semantics
      case SgAsmElfSegmentTableEntry::PT_HIOS: s = "PT_HIOS";  break; // 0x6fffffff,

      /* Values reserved for processor-specific semantics */
      case SgAsmElfSegmentTableEntry::PT_LOPROC: s = "PT_LOPROC";  break;
      case SgAsmElfSegmentTableEntry::PT_HIPROC: s = "PT_HIPROC";  break;

      /* OS-specific values for GNU/Linux */
      case SgAsmElfSegmentTableEntry::PT_GNU_EH_FRAME: s = "PT_GNU_EH_FRAME"; break; // 0x6474e550 GCC .eh_frame_hdr segment
      case SgAsmElfSegmentTableEntry::PT_GNU_STACK:    s = "PT_GNU_STACK";    break; // 0x6474e551 Indicates stack executability */
      case SgAsmElfSegmentTableEntry::PT_GNU_RELRO:    s = "PT_GNU_RELRO";    break; // 0x6474e552 Read-only after relocation */
      case SgAsmElfSegmentTableEntry::PT_PAX_FLAGS:    s = "PT_PAX_FLAGS";    break; // 0x65041580 Indicates PaX flag markings */

      /* OS-specific values for Sun */
      case SgAsmElfSegmentTableEntry::PT_SUNWBSS:      s = "PT_SUNWBSS";   break; // 0x6ffffffa Sun Specific segment */
      case SgAsmElfSegmentTableEntry::PT_SUNWSTACK:    s = "PT_SUNWSTACK"; break; // 0x6ffffffb Stack segment */

      default:
      {
          s = "error";

          // DQ (8/29/2008): This case is exercised frequently, I think it warrants only a warning, instead of an error.
          printf ("Warning: default reached for SgAsmElfSegmentTableEntry::stringifyType = 0x%x \n",kind);
      }
    }
    return s;
}

/** Non-parsing constructor for an ELF Segment (Program Header) Table */
void
SgAsmElfSegmentTable::ctor()
{
    /* There can be only one ELF Segment Table */
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr);
    ROSE_ASSERT(fhdr->get_segment_table()==NULL);
    fhdr->set_segment_table(this);
    
    set_synthesized(true);                              /* the segment table isn't part of any explicit section */
    set_name(new SgAsmBasicString("ELF Segment Table"));
    set_purpose(SP_HEADER);

    fhdr->set_segment_table(this);
}

/** Parses an ELF Segment (Program Header) Table and constructs and parses all segments reachable from the table. The section
 *  is extended as necessary based on the number of entries and teh size of each entry. */
SgAsmElfSegmentTable *
SgAsmElfSegmentTable::parse()
{
    SgAsmGenericSection::parse();

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();

    size_t ent_size, struct_size, opt_size, nentries;
    calculate_sizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(opt_size==fhdr->get_phextrasz() && nentries==fhdr->get_e_phnum());

    /* If the current size is very small (0 or 1 byte) then we're coming straight from the constructor and the parsing should
     * also extend this section to hold all the entries. Otherwise the caller must have assigned a specific size for a good
     * reason and we should leave that alone, reading zeros if the entries extend beyond the defined size. */
    if (get_size()<=1 && get_size()<nentries*ent_size)
        extend(nentries*ent_size - get_size());
    
    addr_t offset=0;                                /* w.r.t. the beginning of this section */
    for (size_t i=0; i<nentries; i++, offset+=ent_size) {
        /* Read/decode the segment header */
        SgAsmElfSegmentTableEntry *shdr = NULL;
        if (4==fhdr->get_word_size()) {
            SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk disk;
            content(offset, struct_size, &disk);
            shdr = new SgAsmElfSegmentTableEntry(sex, &disk);
        } else {
            SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk disk;
            content(offset, struct_size, &disk);
            shdr = new SgAsmElfSegmentTableEntry(sex, &disk);
        }
        shdr->set_index(i);
        if (opt_size>0)
            shdr->get_extra() = content_ucl(offset+struct_size, opt_size);

        /* Null segments are just unused slots in the table; no real section to create */
        if (SgAsmElfSegmentTableEntry::PT_NULL == shdr->get_type())
            continue;

        /* Create SgAsmElfSection objects for each ELF Segment. However, if the ELF Segment Table describes a segment
         * that's the same offset and size as a section from the Elf Section Table (and the memory mappings are
         * consistent) then use the preexisting section instead of creating a new one. */
        SgAsmElfSection *s = NULL;
        SgAsmGenericSectionPtrList possible = fhdr->get_file()->get_sections_by_offset(shdr->get_offset(), shdr->get_filesz());
        for (size_t j=0; !s && j<possible.size(); j++) {
            if (possible[j]->get_offset()!=shdr->get_offset() || possible[j]->get_size()!=shdr->get_filesz())
                continue; /*different file extent*/
            if (possible[j]->is_mapped()) {
                if (possible[j]->get_mapped_rva()!=shdr->get_vaddr() || possible[j]->get_mapped_size()!=shdr->get_memsz())
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

            /* Found a match. Set memory mapping params only. */
            s = dynamic_cast<SgAsmElfSection*>(possible[j]);
            if (!s) continue; /*potential match was not from the ELF Section or Segment table*/
            if (s->get_segment_entry()) continue; /*potential match is assigned to some other segment table entry*/
            s->init_from_segment_table(shdr, true); /*true=>set memory mapping params only*/
        }

        /* Create a new segment if no matching section was found. */
        if (!s) {
            s = new SgAsmElfSection(fhdr);
            s->init_from_segment_table(shdr);
        }
    }
    return this;
}

/** Attaches a previously unattached ELF Segment (SgAsmElfSection) to the ELF Segment Table (SgAsmElfSegmentTable). This
 *  method complements SgAsmElfSection::init_from_segment_table. This method initializes the segment table from the segment
 *  while init_from_segment_table() initializes the segment from the segment table.
 *  
 *  ELF Segments are represented by SgAsmElfSection objects since ELF Segments and ELF Sections overlap very much in their
 *  features and thus should share an interface. An SgAsmElfSection can appear in the ELF Section Table and/or the ELF Segment
 *  Table and you can determine where it was located by calling get_section_entry() and get_segment_entry(). */
void
SgAsmElfSegmentTable::add_section(SgAsmElfSection *section)
{
    ROSE_ASSERT(section!=NULL);
    ROSE_ASSERT(section->get_file()==get_file());
    ROSE_ASSERT(section->get_header()==get_header());
    ROSE_ASSERT(section->get_segment_entry()==NULL);            /* must not be in the segment table yet */

    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr);
    
    /* Assign a slot in the segment table */
    int idx = fhdr->get_e_phnum();
    fhdr->set_e_phnum(idx+1);

    /* Create a new segment table entry */
    SgAsmElfSegmentTableEntry *shdr = new SgAsmElfSegmentTableEntry;
    shdr->update_from_section(section);
    section->set_segment_entry(shdr);
}

/* Returns info about the size of the entries based on information already available. Any or all arguments may be null
 * pointers if the caller is not interested in the value. */
rose_addr_t
SgAsmElfSegmentTable::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);

    size_t struct_size = 0;
    size_t extra_size = fhdr->get_phextrasz();
    size_t entry_size = 0;
    size_t nentries = 0;

    /* Size of required part of each entry */
    if (4==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk);
    } else if (8==fhdr->get_word_size()) {
        struct_size = sizeof(SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk);
    } else {
        throw FormatError("bad ELF word size");
    }

    /* Entire entry should be at least large enough for the required part. */
    entry_size = struct_size;

    /* Size of optional parts. If we've parsed the table then use the largest optional part, otherwise assume the entry from
     * the ELF File Header is correct. */
    SgAsmGenericSectionPtrList sections = fhdr->get_sections()->get_sections();
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmElfSection *elfsec = dynamic_cast<SgAsmElfSection*>(sections[i]);
        if (elfsec && elfsec->get_segment_entry()) {
            nentries++;
            extra_size = std::max(extra_size, elfsec->get_segment_entry()->get_extra().size());
        }
    }

    /* Total number of entries. Either we haven't parsed the segment table yet (nor created the segments it defines) or we
     * have. In the former case we use the setting from the ELF File Header, otherwise we just count the number of segments
     * that have associated segment table entry pointers. */
    if (0==nentries)
        nentries = fhdr->get_e_phnum();

    /* Return values */
    if (entsize)
        *entsize = entry_size;
    if (required)
        *required = struct_size;
    if (optional)
        *optional = extra_size;
    if (entcount)
        *entcount = nentries;
    return entry_size * nentries;
}

/* Pre-unparsing updates */
bool
SgAsmElfSegmentTable::reallocate()
{
    bool reallocated = false;

    /* Resize based on word size from ELF File Header */
    size_t opt_size, nentries;
    addr_t need = calculate_sizes(NULL, NULL, &opt_size, &nentries);
    if (need < get_size()) {
        if (is_mapped()) {
            ROSE_ASSERT(get_mapped_size()==get_size());
            set_mapped_size(need);
        }
        set_size(need);
        reallocated = true;
    } else if (need > get_size()) {
        get_file()->shift_extend(this, 0, need-get_size(), SgAsmGenericFile::ADDRSP_ALL, SgAsmGenericFile::ELASTIC_HOLE);
        reallocated = true;
    }

    /* Update data members in the ELF File Header. No need to return true for these changes. */
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    fhdr->set_phextrasz(opt_size);
    fhdr->set_e_phnum(nentries);

    return reallocated;
}

/* Write the segment table to disk. */
void
SgAsmElfSegmentTable::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = dynamic_cast<SgAsmElfFileHeader*>(get_header());
    ROSE_ASSERT(fhdr!=NULL);
    ByteOrder sex = fhdr->get_sex();
    SgAsmGenericSectionPtrList sections = fhdr->get_segtab_sections();

    /* Write the segments first */
    for (size_t i=0; i<sections.size(); i++)
        sections[i]->unparse(f);
    unparse_holes(f);

    /* Calculate sizes. The ELF File Header should have been updated in reallocate() prior to unparsing. */
    size_t ent_size, struct_size, opt_size, nentries;
    calculate_sizes(&ent_size, &struct_size, &opt_size, &nentries);
    ROSE_ASSERT(fhdr->get_phextrasz()==opt_size);
    ROSE_ASSERT(fhdr->get_e_phnum()==nentries);
    
    /* Write the segment table entries */
    for (size_t i=0; i < sections.size(); ++i) {
        SgAsmElfSection *section = dynamic_cast<SgAsmElfSection*>(sections[i]);
        ROSE_ASSERT(section!=NULL);
        SgAsmElfSegmentTableEntry *shdr = section->get_segment_entry();
        ROSE_ASSERT(shdr!=NULL);
        ROSE_ASSERT(shdr->get_offset()==section->get_offset()); /*segment table entry should have been updated in reallocate()*/

        int id = shdr->get_index();
        ROSE_ASSERT(id>=0 && (size_t)id<nentries);
            
        SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk disk32;
        SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk disk64;
        void *disk = NULL;
        
        if (4==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk32);
        } else if (8==fhdr->get_word_size()) {
            disk = shdr->encode(sex, &disk64);
        } else {
            ROSE_ASSERT(!"invalid word size");
        }
        
        /* The disk struct */
        addr_t spos = write(f, id*ent_size, struct_size, disk);
        if (shdr->get_extra().size() > 0)
            write(f, spos, shdr->get_extra());
    }
}

/* Print some debugging info */
void
SgAsmElfSegmentTable::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSegmentTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSegmentTable.", prefix);
    }

    SgAsmGenericSection::dump(f, p, -1);

    if (variantT() == V_SgAsmElfSegmentTable) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Relocation (Rel and Rela)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructors */
void
SgAsmElfRelocEntry::ctor(ByteOrder sex, const Elf32RelaEntry_disk *disk)
{
    p_r_offset    = disk_to_host(sex, disk->r_offset);
    p_r_addend    = disk_to_host(sex, disk->r_addend);
    uint64_t info = disk_to_host(sex, disk->r_info);
    p_sym = info >> 8;
    p_type = info & 0xff;
}
void
SgAsmElfRelocEntry::ctor(ByteOrder sex, const Elf64RelaEntry_disk *disk)
{
    p_r_offset    = disk_to_host(sex, disk->r_offset);
    p_r_addend    = disk_to_host(sex, disk->r_addend);
    uint64_t info = disk_to_host(sex, disk->r_info);
    p_sym = info >> 32;
    p_type = info & 0xffffffff;
}
void
SgAsmElfRelocEntry::ctor(ByteOrder sex, const Elf32RelEntry_disk *disk)
{
    p_r_offset    = disk_to_host(sex, disk->r_offset);
    p_r_addend    = 0;
    uint64_t info = disk_to_host(sex, disk->r_info);
    p_sym = info >> 8;
    p_type = info & 0xff;
}
void
SgAsmElfRelocEntry::ctor(ByteOrder sex, const Elf64RelEntry_disk *disk)
{
    p_r_offset    = disk_to_host(sex, disk->r_offset);
    p_r_addend    = 0;
    uint64_t info = disk_to_host(sex, disk->r_info);
    p_sym = info >> 32;
    p_type = info & 0xffffffff;
}

/* Encode a native entry back into disk format */
void *
SgAsmElfRelocEntry::encode(ByteOrder sex, Elf32RelaEntry_disk *disk) const
{
    host_to_disk(sex, p_r_offset, &(disk->r_offset));
    host_to_disk(sex, p_r_addend, &(disk->r_addend));
    uint64_t info = (p_sym<<8) | (p_type & 0xff);
    host_to_disk(sex, info, &(disk->r_info));
    return disk;
}
void *
SgAsmElfRelocEntry::encode(ByteOrder sex, Elf64RelaEntry_disk *disk) const
{
    host_to_disk(sex, p_r_offset, &(disk->r_offset));
    host_to_disk(sex, p_r_addend, &(disk->r_addend));
    uint64_t info = ((uint64_t)p_sym<<32) | (p_type & 0xffffffff);
    host_to_disk(sex, info, &(disk->r_info));
    return disk;
}
void *
SgAsmElfRelocEntry::encode(ByteOrder sex, Elf32RelEntry_disk *disk) const
{
    host_to_disk(sex, p_r_offset, &(disk->r_offset));
    ROSE_ASSERT(0==p_r_addend);
    uint64_t info = (p_sym<<8) | (p_type & 0xff);
    host_to_disk(sex, info, &(disk->r_info));
    return disk;
}
void *
SgAsmElfRelocEntry::encode(ByteOrder sex, Elf64RelEntry_disk *disk) const
{
    host_to_disk(sex, p_r_offset, &(disk->r_offset));
    ROSE_ASSERT(0==p_r_addend);
    uint64_t info = ((uint64_t)p_sym<<32) | (p_type & 0xffffffff);
    host_to_disk(sex, info, &(disk->r_info));
    return disk;
}

/* Print some debugging info */
void
SgAsmElfRelocEntry::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmElfSymbolSection *symtab) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfRelocEntry[%zd]", prefix, idx);
    } else {
        sprintf(p, "%sElfRelocEntry", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    /* compact one-line-per-reloc format */
    if (0==idx)
        fprintf(f, "%s%-*s   %-10s %-4s %-10s %4s %-10s Name + Addend\n", p, w, "", "Offset", "Type", "Addend", "Sym", "Value");
    fprintf(f, "%s%-*s = 0x%08"PRIx64" 0x%02lx 0x%08"PRIx64" %4lu", p, w, "", p_r_offset, p_type, p_r_addend, p_sym);
    if (!symtab) {
        fprintf(f, " 0x%08x <no-symtab>", 0);
    } else if (p_sym>=symtab->get_symbols()->get_symbols().size()) {
        fprintf(f, " 0x%08x <range>", 0);
    } else {
        SgAsmGenericSymbol *sym = symtab->get_symbols()->get_symbols()[p_sym];
        fprintf(f, " 0x%08"PRIx64" %s", sym->get_value(), sym->get_name()->c_str());
    }
    fprintf(f, " + %"PRIu64"\n", p_r_addend);
    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, ".extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

/* Non-parsing constructor */
void
SgAsmElfRelocSection::ctor()
{
    p_entries = new SgAsmElfRelocEntryList;
    p_entries->set_parent(this);
}

/* Parse an existing ELF Rela Section */
SgAsmElfRelocSection *
SgAsmElfRelocSection::parse()
{
    SgAsmElfSection::parse();

    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);

    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);
    ROSE_ASSERT(extra_size==0);
    
    /* Parse each entry */
    for (size_t i=0; i<nentries; i++) {
        SgAsmElfRelocEntry *entry = 0;
        if (4==fhdr->get_word_size()) {
            if (p_uses_addend) {
                SgAsmElfRelocEntry::Elf32RelaEntry_disk disk;
                content(i*entry_size, struct_size, &disk);
                entry = new SgAsmElfRelocEntry(fhdr->get_sex(), &disk);
            } else {
                SgAsmElfRelocEntry::Elf32RelEntry_disk disk;
                content(i*entry_size, struct_size, &disk);
                entry = new SgAsmElfRelocEntry(fhdr->get_sex(), &disk);
            }
        } else if (8==fhdr->get_word_size()) {
            if (p_uses_addend) {
                SgAsmElfRelocEntry::Elf64RelaEntry_disk disk;
                content(i*entry_size, struct_size, &disk);
                entry = new SgAsmElfRelocEntry(fhdr->get_sex(), &disk);
            } else {
                SgAsmElfRelocEntry::Elf64RelEntry_disk disk;
                content(i*entry_size, struct_size, &disk);
                entry = new SgAsmElfRelocEntry(fhdr->get_sex(), &disk);
            }
        } else {
            throw FormatError("unsupported ELF word size");
        }
        if (extra_size>0)
            entry->get_extra() = content_ucl(i*entry_size+struct_size, extra_size);
        p_entries->get_entries().push_back(entry);
        ROSE_ASSERT(p_entries->get_entries().size()>0);
    }
    return this;
}

/* Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. */
rose_addr_t
SgAsmElfRelocSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    rose_addr_t retval=0;
    std::vector<size_t> extra_sizes;
    for (size_t i=0; i<p_entries->get_entries().size(); i++)
        extra_sizes.push_back(p_entries->get_entries()[i]->get_extra().size());
    if (p_uses_addend) {
        retval =  calculate_sizes(sizeof(SgAsmElfRelocEntry::Elf32RelaEntry_disk), sizeof(SgAsmElfRelocEntry::Elf64RelaEntry_disk),
                                  extra_sizes, entsize, required, optional, entcount);
    } else {
        retval =  calculate_sizes(sizeof(SgAsmElfRelocEntry::Elf32RelEntry_disk),  sizeof(SgAsmElfRelocEntry::Elf64RelEntry_disk),
                                  extra_sizes, entsize, required, optional, entcount);
    }
    return retval;
}

/* Write section back to disk */
void
SgAsmElfRelocSection::unparse(std::ostream &f) const
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
        SgAsmElfRelocEntry::Elf32RelaEntry_disk diska32;
        SgAsmElfRelocEntry::Elf64RelaEntry_disk diska64;
        SgAsmElfRelocEntry::Elf32RelEntry_disk  disk32;
        SgAsmElfRelocEntry::Elf64RelEntry_disk  disk64;
        void *disk  = NULL;

        SgAsmElfRelocEntry *entry = p_entries->get_entries()[i];

        if (4==fhdr->get_word_size()) {
            if (p_uses_addend) {
                disk = entry->encode(sex, &diska32);
            } else {
                disk = entry->encode(sex, &disk32);
            }
        } else if (8==fhdr->get_word_size()) {
            if (p_uses_addend) {
                disk = entry->encode(sex, &diska64);
            } else {
                disk = entry->encode(sex, &disk64);
            }
        } else {
            ROSE_ASSERT(!"unsupported word size");
        }

        addr_t spos = i * entry_size;
        spos = write(f, spos, struct_size, disk);
#if 0 /*FIXME: padding not supported here yet (RPM 2008-10-13)*/
        if (entry->get_extra().size()>0) {
            ROSE_ASSERT(entry->get_extra().size()<=extra_size);
            write(f, spos, entry->get_extra());
        }
#endif
    }

    unparse_holes(f);
}

/* Print some debugging info */
void
SgAsmElfRelocSection::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sRelocSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sRelocSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    SgAsmElfSection::dump(f, p, -1);
    SgAsmElfSymbolSection *symtab = dynamic_cast<SgAsmElfSymbolSection*>(get_linked_section());
    fprintf(f, "%s%-*s = %s\n", p, w, "uses_addend", p_uses_addend ? "yes" : "no");

    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfRelocEntry *ent = p_entries->get_entries()[i];
        ent->dump(f, p, i, symtab);
    }

    if (variantT() == V_SgAsmElfRelocSection) //unless a base class
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
SgAsmElfDynamicEntry::encode(ByteOrder sex, Elf32DynamicEntry_disk *disk) const
{
    host_to_disk(sex, p_d_tag, &(disk->d_tag));
    host_to_disk(sex, p_d_val.get_rva(), &(disk->d_val));
    return disk;
}
void *
SgAsmElfDynamicEntry::encode(ByteOrder sex, Elf64DynamicEntry_disk *disk) const
{
    host_to_disk(sex, p_d_tag, &(disk->d_tag));
    host_to_disk(sex, p_d_val.get_rva(), &(disk->d_val));
    return disk;
}

/* Convert Dynamic Entry Tag to a string */
const char *
SgAsmElfDynamicEntry::stringify_tag(EntryType t) const
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

/* Set name and adjust parent */
void
SgAsmElfDynamicEntry::set_name(SgAsmGenericString *name)
{
    if (p_name)
        p_name->set_parent(NULL);
    p_name = name;
    if (p_name)
        p_name->set_parent(this);
}

/* Print some debugging info */
void
SgAsmElfDynamicEntry::dump(FILE *f, const char *prefix, ssize_t idx) const
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

    fprintf(f, "%s%-*s = %s", p, w, label, p_d_val.to_string().c_str());
    if (p_name)
        fprintf(f, " \"%s\"", p_name->c_str());
    fputc('\n', f);

    if (p_extra.size()>0) {
        fprintf(f, "%s%-*s = %zu bytes\n", p, w, "extra", p_extra.size());
        hexdump(f, 0, std::string(p)+"extra at ", p_extra);
    }
}

/* Non-parsing constructor */
void
SgAsmElfDynamicSection::ctor()
{
    p_entries = new SgAsmElfDynamicEntryList;
    p_entries->set_parent(this);
}

/** Parse an existing section of a file in order to initialize this ELF Dynamic Section. */
SgAsmElfDynamicSection *
SgAsmElfDynamicSection::parse()
{
    SgAsmElfSection::parse();

    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    SgAsmElfSectionTableEntry *shdr = get_section_entry();
    ROSE_ASSERT(shdr);

    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);
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
    return this;
}

/* Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. */
rose_addr_t
SgAsmElfDynamicSection::calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const
{
    std::vector<size_t> extra_sizes;
    for (size_t i=0; i<p_entries->get_entries().size(); i++)
        extra_sizes.push_back(p_entries->get_entries()[i]->get_extra().size());
    return calculate_sizes(sizeof(SgAsmElfDynamicEntry::Elf32DynamicEntry_disk),
                           sizeof(SgAsmElfDynamicEntry::Elf64DynamicEntry_disk),
                           extra_sizes,
                           entsize, required, optional, entcount);
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
              entry->set_name(new SgAsmStoredString(strsec->get_strtab(), entry->get_d_val().get_rva()));
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

/* Called prior to unparse to make things consistent. */
bool
SgAsmElfDynamicSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();

    for (size_t i=0; i<p_entries->get_entries().size(); i++) {
        SgAsmElfDynamicEntry *entry = p_entries->get_entries()[i];
        SgAsmGenericString *name = entry->get_name();
        if (name)
            entry->set_d_val(name->get_offset());
    }
    
    return reallocated;
}

/* Write the dynamic section back to disk */
void
SgAsmElfDynamicSection::unparse(std::ostream &f) const
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
SgAsmElfDynamicSection::dump(FILE *f, const char *prefix, ssize_t idx) const
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

    if (variantT() == V_SgAsmElfDynamicSection) //unless a base class
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
      case STT_COMMON:  p_type = SYM_COMMON;  break;
      case STT_TLS:     p_type = SYM_TLS;     break;
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

void
SgAsmElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx) const
{
    dump(f, prefix, idx, NULL);
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

/* Called before unparsing. Updates the symbol table entry to a consistent state. */
void
SgAsmElfSymbol::reallocate(SgAsmGenericStrtab *strtab)
{
    SgAsmStoredString *s = dynamic_cast<SgAsmStoredString*>(get_name());
    if (s && s->get_strtab()==strtab) {
        p_st_name = get_name()->get_offset();
    } else {
        p_st_name = 0;
    }
}

/* Encode a symbol into disk format */
void *
SgAsmElfSymbol::encode(ByteOrder sex, Elf32SymbolEntry_disk *disk, SgAsmGenericStrtab *strtab) const
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
SgAsmElfSymbol::encode(ByteOrder sex, Elf64SymbolEntry_disk *disk, SgAsmGenericStrtab *strtab) const
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
SgAsmElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx, SgAsmGenericSection *section) const
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

    fprintf(f, "%s%-*s = %"PRIu64" (in linked string table)\n", p, w, "st_name",  p_st_name);

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

/* Non-parsing constructor */
void
SgAsmElfSymbolSection::ctor()
{
    p_symbols = new SgAsmElfSymbolList;
    p_symbols->set_parent(this);
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

    size_t entry_size, struct_size, extra_size, nentries;
    calculate_sizes(&entry_size, &struct_size, &extra_size, &nentries);
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
    return this;
}

/* Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. */
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

/* Called prior to unparsing. Updates symbol entries with name offsets */
bool
SgAsmElfSymbolSection::reallocate()
{
    bool reallocated = SgAsmElfSection::reallocate();
    SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(get_linked_section());
    SgAsmGenericStrtab *strtab = strsec ? strsec->get_strtab() : NULL;
    for (size_t i=0; i<p_symbols->get_symbols().size(); i++) {
        SgAsmElfSymbol *entry = p_symbols->get_symbols()[i];
        entry->reallocate(strtab);
    }
    return reallocated;
}

/* Write symbol table sections back to disk */
void
SgAsmElfSymbolSection::unparse(std::ostream &f) const
{
    SgAsmElfFileHeader *fhdr = get_elf_header();
    ROSE_ASSERT(fhdr);
    ByteOrder sex = fhdr->get_sex();

    /* We need to know what string table is associated with this symbol table. */
    SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(get_linked_section());
    SgAsmGenericStrtab *strtab = strsec ? strsec->get_strtab() : NULL;

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
            disk = entry->encode(sex, &disk32, strtab);
        } else if (8==fhdr->get_word_size()) {
            disk = entry->encode(sex, &disk64, strtab);
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
    fprintf(f, "%s%-*s = %zu symbols\n", p, w, "ElfSymbol.size", p_symbols->get_symbols().size());
    for (size_t i = 0; i < p_symbols->get_symbols().size(); i++) {
        SgAsmGenericSection *section = get_file()->get_section_by_id(p_symbols->get_symbols()[i]->get_st_shndx());
        p_symbols->get_symbols()[i]->dump(f, p, i, section);
    }

    if (variantT() == V_SgAsmElfSymbolSection) //unless a base class
        hexdump(f, 0, std::string(p)+"data at ", p_data);
}
