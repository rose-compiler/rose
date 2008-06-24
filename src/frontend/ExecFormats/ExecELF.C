/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* Truncate an address, ADDR, to be a multiple of the alignment, ALMNT, where ALMNT is a power of two and of the same
 * unsigned datatype as the address. */
#define ALIGN(ADDR,ALMNT) ((ADDR) & ~((ALMNT)-1))

namespace Exec {
namespace ELF {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File headers
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor reads and decodes the ELF header, whether it's 32- or 64-bit.  The 'offset' argument is normally zero since
 * ELF headers are at the beginning of the file. As mentioned in the header file, the section size is initialized as if we had
 * 32-bit words and if necessary we extend the section for 64-bit words herein. */
void
ElfFileHeader::ctor(ExecFile *f, addr_t offset)
{
    const Elf32FileHeader_disk *disk32 = (const Elf32FileHeader_disk*)content(0, sizeof(Elf32FileHeader_disk));
    const Elf64FileHeader_disk *disk64 = NULL;

    set_name("ELF File Header");
    set_synthesized(true);
    set_purpose(SP_HEADER);

    /* File byte order */
    if (1!=disk32->e_ident_data_encoding && 2!=disk32->e_ident_data_encoding)
        throw FormatError("invalid ELF header data encoding");

    ByteOrder sex = 1 == disk32->e_ident_data_encoding ? ORDER_LSB : ORDER_MSB;

    /* Switch to 64-bit view of header if necessary */
    if (1!=disk32->e_ident_file_class && 2!=disk32->e_ident_file_class)
        throw FormatError("invalid ELF header file class");
    if (2==disk32->e_ident_file_class) {
        extend(sizeof(Elf64FileHeader_disk)-sizeof(Elf32FileHeader_disk));
        disk64 = (const Elf64FileHeader_disk*)content(0, sizeof(Elf64FileHeader_disk));
        disk32 = NULL;
    }

    /* Decode members. Both alternatives are identical except one uses disk32 and the other uses disk64 */
    if (disk32) {
        ROSE_ASSERT(sizeof(e_ident_padding)==sizeof(disk32->e_ident_padding));
        memcpy(e_ident_padding, disk32->e_ident_padding, sizeof(e_ident_padding));
        e_ident_file_class    = disk_to_host(sex, disk32->e_ident_file_class);
        e_ident_data_encoding = disk_to_host(sex, disk32->e_ident_data_encoding);
        e_ident_file_version  = disk_to_host(sex, disk32->e_ident_file_version);
        e_type                = disk_to_host(sex, disk32->e_type);
        e_machine             = disk_to_host(sex, disk32->e_machine);
        e_version             = disk_to_host(sex, disk32->e_version);
        e_entry               = disk_to_host(sex, disk32->e_entry);
        e_phoff               = disk_to_host(sex, disk32->e_phoff);
        e_shoff               = disk_to_host(sex, disk32->e_shoff);
        e_flags               = disk_to_host(sex, disk32->e_flags);
        e_ehsize              = disk_to_host(sex, disk32->e_ehsize);
        e_phentsize           = disk_to_host(sex, disk32->e_phentsize);
        e_phnum               = disk_to_host(sex, disk32->e_phnum);
        e_shentsize           = disk_to_host(sex, disk32->e_shentsize);
        e_shnum               = disk_to_host(sex, disk32->e_shnum);
        e_shstrndx            = disk_to_host(sex, disk32->e_shstrndx);
    } else {
        ROSE_ASSERT(sizeof(e_ident_padding)==sizeof(disk64->e_ident_padding));
        memcpy(e_ident_padding, disk64->e_ident_padding, sizeof(e_ident_padding));
        e_ident_file_class    = disk_to_host(sex, disk64->e_ident_file_class);
        e_ident_data_encoding = disk_to_host(sex, disk64->e_ident_data_encoding);
        e_ident_file_version  = disk_to_host(sex, disk64->e_ident_file_version);
        e_type                = disk_to_host(sex, disk64->e_type);
        e_machine             = disk_to_host(sex, disk64->e_machine);
        e_version             = disk_to_host(sex, disk64->e_version);
        e_entry               = disk_to_host(sex, disk64->e_entry);
        e_phoff               = disk_to_host(sex, disk64->e_phoff);
        e_shoff               = disk_to_host(sex, disk64->e_shoff);
        e_flags               = disk_to_host(sex, disk64->e_flags);
        e_ehsize              = disk_to_host(sex, disk64->e_ehsize);
        e_phentsize           = disk_to_host(sex, disk64->e_phentsize);
        e_phnum               = disk_to_host(sex, disk64->e_phnum);
        e_shentsize           = disk_to_host(sex, disk64->e_shentsize);
        e_shnum               = disk_to_host(sex, disk64->e_shnum);
        e_shstrndx            = disk_to_host(sex, disk64->e_shstrndx);
   }

    /* Magic number */
    ROSE_ASSERT(sizeof(disk32->e_ident_magic)==sizeof(disk64->e_ident_magic));
    for (size_t i=0; i<sizeof(disk32->e_ident_magic); i++)
        magic.push_back(disk32?disk32->e_ident_magic[i]:disk64->e_ident_magic[i]);
    
    /* File format */
    exec_format.family = FAMILY_ELF;
    switch (e_type) {
      case 0:
        exec_format.purpose = PURPOSE_UNSPECIFIED;
        break;
      case 1:
      case 3:
        exec_format.purpose = PURPOSE_LIBRARY;
        break;
      case 2:
        exec_format.purpose = PURPOSE_EXECUTABLE;
        break;
      case 4:
        exec_format.purpose = PURPOSE_CORE_DUMP;
        break;
      default:
        if (e_type>=0xff00 && e_type<=0xffff) {
            exec_format.purpose = PURPOSE_PROC_SPECIFIC;
        } else {
            exec_format.purpose = PURPOSE_OTHER;
        }
        break;
    }
    exec_format.sex = sex;
    exec_format.version = e_version;
    exec_format.is_current_version = (1==e_version);
    exec_format.abi = ABI_UNSPECIFIED;                   /* ELF specifies a target architecture rather than an ABI */
    exec_format.abi_version = 0;
    exec_format.word_size = disk32 ? 4 : 8;

    /* Target architecture */
    switch (e_machine) {                                /* These come from the Portable Formats Specification v1.1 */
      case 0:
        target.set_isa(ISA_UNSPECIFIED);
        break;
      case 1:
        target.set_isa(ISA_ATT_WE_32100);
        break;
      case 2:
        target.set_isa(ISA_SPARC_Family);
        break;
      case 3:
        target.set_isa(ISA_IA32_386);
        break;
      case 4:
        target.set_isa(ISA_M68K_Family);
        break;
      case 5:
        target.set_isa(ISA_M88K_Family);
        break;
      case 7:
        target.set_isa(ISA_I860_Family);
        break;
      case 8:
        target.set_isa(ISA_MIPS_Family);
        break;
      default:
        /*FIXME: There's a whole lot more. See Dan's Elf reader. */
        target.set_isa(ISA_OTHER, e_machine);
        break;
    }

    /* Target architecture */
    /*FIXME*/

    /* Entry point */
    base_va = 0;
    entry_rva = e_entry;
}

/* Maximum page size according to the ABI. This is used by the loader when calculating the program base address. Since parts
 * of the file are mapped into the process address space those parts must be aligned (both in the file and in memory) on the
 * largest possible page boundary so that any smaller page boundary will also work correctly. */
uint64_t
ElfFileHeader::max_page_size()
{
    /* FIXME:
     *    System V max page size is 4k.
     *    IA32 is 4k
     *    x86_64 is 2MB
     * Other systems may vary! */
    return 4*1024;
}

/* Encode Elf header disk structure */
void *
ElfFileHeader::encode(ByteOrder sex, Elf32FileHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_ident_magic); i++)
        disk->e_ident_magic[i] = magic[i];
    host_to_disk(sex, e_ident_file_class,    &(disk->e_ident_file_class));
    host_to_disk(sex, e_ident_data_encoding, &(disk->e_ident_data_encoding));
    host_to_disk(sex, e_ident_file_version,  &(disk->e_ident_file_version));
    for (size_t i=0; i<NELMTS(e_ident_padding); i++)
        disk->e_ident_padding[i] = e_ident_padding[i];
    host_to_disk(sex, e_type,                &(disk->e_type));
    host_to_disk(sex, e_machine,             &(disk->e_machine));
    host_to_disk(sex, e_version,             &(disk->e_version));
    host_to_disk(sex, e_entry,               &(disk->e_entry));
    host_to_disk(sex, e_phoff,               &(disk->e_phoff));
    host_to_disk(sex, e_shoff,               &(disk->e_shoff));
    host_to_disk(sex, e_flags,               &(disk->e_flags));
    host_to_disk(sex, e_ehsize,              &(disk->e_ehsize));
    host_to_disk(sex, e_phentsize,           &(disk->e_phentsize));
    host_to_disk(sex, e_phnum,               &(disk->e_phnum));
    host_to_disk(sex, e_shentsize,           &(disk->e_shentsize));
    host_to_disk(sex, e_shnum,               &(disk->e_shnum));
    host_to_disk(sex, e_shstrndx,            &(disk->e_shstrndx));
    return disk;
}
void *
ElfFileHeader::encode(ByteOrder sex, Elf64FileHeader_disk *disk)
{
    for (size_t i=0; i<NELMTS(disk->e_ident_magic); i++)
        disk->e_ident_magic[i] = magic[i];
    host_to_disk(sex, e_ident_file_class,    &(disk->e_ident_file_class));
    host_to_disk(sex, e_ident_data_encoding, &(disk->e_ident_data_encoding));
    host_to_disk(sex, e_ident_file_version,  &(disk->e_ident_file_version));
    for (size_t i=0; i<NELMTS(e_ident_padding); i++)
        disk->e_ident_padding[i] = e_ident_padding[i];
    host_to_disk(sex, e_type,                &(disk->e_type));
    host_to_disk(sex, e_machine,             &(disk->e_machine));
    host_to_disk(sex, e_version,             &(disk->e_version));
    host_to_disk(sex, e_entry,               &(disk->e_entry));
    host_to_disk(sex, e_phoff,               &(disk->e_phoff));
    host_to_disk(sex, e_shoff,               &(disk->e_shoff));
    host_to_disk(sex, e_flags,               &(disk->e_flags));
    host_to_disk(sex, e_ehsize,              &(disk->e_ehsize));
    host_to_disk(sex, e_phentsize,           &(disk->e_phentsize));
    host_to_disk(sex, e_phnum,               &(disk->e_phnum));
    host_to_disk(sex, e_shentsize,           &(disk->e_shentsize));
    host_to_disk(sex, e_shnum,               &(disk->e_shnum));
    host_to_disk(sex, e_shstrndx,            &(disk->e_shstrndx));
    return disk;
}

    

/* Write the ELF header back to a file by encoding its contents rather than simply writing the section contents */
void
ElfFileHeader::unparse(FILE *f)
{
    Elf32FileHeader_disk disk32;
    Elf64FileHeader_disk disk64;
    void *disk=NULL;
    size_t size=0;
    
    if (4==get_word_size()) {
        disk = encode(get_sex(), &disk32);
        size = sizeof(disk32);
    } else if (8==get_word_size()) {
        disk = encode(get_sex(), &disk64);
        size = sizeof(disk64);
    } else {
        ROSE_ASSERT(!"unsupported word size");
    }

    int status = fseek(f, offset, SEEK_SET);
    ROSE_ASSERT(status>=0);
    size_t nwrite = fwrite(disk, size, 1, f);
    ROSE_ASSERT(1==nwrite);
}

/* Print some debugging info */
void
ElfFileHeader::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfFileHeader[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfFileHeader.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecHeader::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_file_class",     e_ident_file_class);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_data_encoding",  e_ident_data_encoding);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_ident_file_version",   e_ident_file_version);
    for (size_t i=0; i<NELMTS(e_ident_padding); i++)
        fprintf(f, "%s%-*s = [%zu] %u\n",                   p, w, "e_ident_padding",     i, e_ident_padding[i]);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_type",                 e_type);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_machine",              e_machine);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_version",              e_version);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",                p, w, "e_entry",                e_entry);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes into file\n",    p, w, "e_phoff",                e_phoff);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes into file\n",    p, w, "e_shoff",                e_shoff);
    fprintf(f, "%s%-*s = 0x%08x\n",                         p, w, "e_flags",                e_flags);
    fprintf(f, "%s%-*s = %u bytes\n",                       p, w, "e_ehsize",               e_ehsize);
    fprintf(f, "%s%-*s = %u bytes\n",                       p, w, "e_phentsize",            e_phentsize);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_phnum",                e_phnum);
    fprintf(f, "%s%-*s = %u bytes\n",                       p, w, "e_shentsize",            e_shentsize);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_shnum",                e_shnum);
    fprintf(f, "%s%-*s = %u\n",                             p, w, "e_shstrndx",             e_shstrndx);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sections
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructor */
void
ElfSection::ctor(ElfFileHeader *fhdr, ElfSectionTableEntry *shdr)
{
    set_synthesized(false);

    /* Section purpose */
    switch (shdr->sh_type) {
      case SHT_PROGBITS:
        set_purpose(SP_PROGRAM);
        break;
      case SHT_STRTAB:
        set_purpose(SP_HEADER);
        break;
      case SHT_DYNSYM:
      case SHT_SYMTAB:
        set_purpose(SP_SYMTAB);
        break;
      default:
        set_purpose(SP_OTHER);
        break;
    }

    /* Section mapping */
    if (shdr->sh_addr>0)
        set_mapped(true, shdr->sh_addr);
}

/* Print some debugging info */
void
ElfSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSection.", prefix);
    }
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    ExecSection::dump(f, p, -1);
    st_entry->dump(f, p, -1);
    if (linked_section) {
        fprintf(f, "%s%-*s = [%d] \"%s\" @%" PRIu64 ", %" PRIu64 " bytes\n", p, w, "linked_to",
                linked_section->get_id(), linked_section->get_name().c_str(),
                linked_section->get_offset(), linked_section->get_size());
    } else {
        fprintf(f, "%s%-*s = NULL\n",    p, w, "linked_to");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Section tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Converts 32-bit disk representation to host representation */
void
ElfSectionTableEntry::ctor(ByteOrder sex, const Elf32SectionTableEntry_disk *disk) 
{
    sh_name      = disk_to_host(sex, disk->sh_name);
    sh_type      = disk_to_host(sex, disk->sh_type);
    sh_flags     = disk_to_host(sex, disk->sh_flags);
    sh_addr      = disk_to_host(sex, disk->sh_addr);
    sh_offset    = disk_to_host(sex, disk->sh_offset);
    sh_size      = disk_to_host(sex, disk->sh_size);
    sh_link      = disk_to_host(sex, disk->sh_link);
    sh_info      = disk_to_host(sex, disk->sh_info);
    sh_addralign = disk_to_host(sex, disk->sh_addralign);
    sh_entsize   = disk_to_host(sex, disk->sh_entsize);
}
    
/* Converts 64-bit disk representation to host representation */
void
ElfSectionTableEntry::ctor(ByteOrder sex, const Elf64SectionTableEntry_disk *disk) 
{
    sh_name      = disk_to_host(sex, disk->sh_name);
    sh_type      = disk_to_host(sex, disk->sh_type);
    sh_flags     = disk_to_host(sex, disk->sh_flags);
    sh_addr      = disk_to_host(sex, disk->sh_addr);
    sh_offset    = disk_to_host(sex, disk->sh_offset);
    sh_size      = disk_to_host(sex, disk->sh_size);
    sh_link      = disk_to_host(sex, disk->sh_link);
    sh_info      = disk_to_host(sex, disk->sh_info);
    sh_addralign = disk_to_host(sex, disk->sh_addralign);
    sh_entsize   = disk_to_host(sex, disk->sh_entsize);
}

/* Constructor reads the Elf Section Table (i.e., array of section headers) */
void
ElfSectionTable::ctor(ElfFileHeader *fhdr)
{
    set_synthesized(true);                              /* the section table isn't really a section itself */
    set_name("ELF section table");
    set_purpose(SP_HEADER);

    ByteOrder sex = fhdr->get_sex();

    if (fhdr->e_shnum>0) {

        /* Check sizes */
        ROSE_ASSERT(4==fhdr->get_word_size() || 8==fhdr->get_word_size());
        size_t struct_size = 4==fhdr->get_word_size() ?
                             sizeof(Elf32SectionTableEntry_disk) : sizeof(Elf64SectionTableEntry_disk);
        if (fhdr->e_shentsize < struct_size)
            throw FormatError("ELF header shentsize is too small");
        
        /* Read all the section headers. We can't just cast this to an array like with other structs because
         * the ELF header specifies the size of each entry. */
        std::vector<ElfSectionTableEntry*> entries;
        addr_t offset=0;
        for (size_t i=0; i<fhdr->e_shnum; i++, offset+=fhdr->e_shentsize) {
            ElfSectionTableEntry *shdr = NULL;
            if (4==fhdr->get_word_size()) {
                const Elf32SectionTableEntry_disk *disk = (const Elf32SectionTableEntry_disk*)content(offset, fhdr->e_shentsize);
                shdr = new ElfSectionTableEntry(sex, disk);
            } else {
                const Elf64SectionTableEntry_disk *disk = (const Elf64SectionTableEntry_disk*)content(offset, fhdr->e_shentsize);
                shdr = new ElfSectionTableEntry(sex, disk);
            }
            shdr->nextra = fhdr->e_shentsize - struct_size;
            if (shdr->nextra>0)
                shdr->extra = content(offset+struct_size, shdr->nextra);
            entries.push_back(shdr);
        }

        /* Read the string table section first because we'll need this to initialize section names. */
        ElfSection *strtab = NULL;
        if (fhdr->e_shstrndx>0) {
            ElfSectionTableEntry *shdr = entries[fhdr->e_shstrndx];
            strtab = new ElfSection(fhdr, shdr);
            strtab->set_id(fhdr->e_shstrndx);
            strtab->set_st_entry(shdr);
            strtab->set_name(strtab->content_str(shdr->sh_name));
        }
        
        /* Read all other sections */
        for (size_t i=0; i<entries.size(); i++) {
            ElfSectionTableEntry *shdr = entries[i];
            ElfSection *section=NULL;
            if (i==fhdr->e_shstrndx) continue; /*we already read string table*/
            if (SHT_NULL==shdr->sh_type || SHT_NOBITS==shdr->sh_type) continue; /*no disk representation*/
            switch (shdr->sh_type) {
              case SHT_DYNSYM:
              case SHT_SYMTAB:
                section = new ElfSymbolSection(fhdr, shdr);
                break;
              default:
                section = new ElfSection(fhdr, shdr);
                break;
            }
            section->set_id(i);
            section->set_st_entry(shdr);
            if (strtab)
                section->set_name(strtab->content_str(shdr->sh_name));
        }

        /* Initialize links between sections */
        for (size_t i=0; i<entries.size(); i++) {
            ElfSectionTableEntry *shdr = entries[i];
            if (shdr->sh_link>0) {
                ElfSection *source = dynamic_cast<ElfSection*>(fhdr->get_file()->get_section_by_id(i));
                ElfSection *target = dynamic_cast<ElfSection*>(fhdr->get_file()->get_section_by_id(shdr->sh_link));
                source->set_linked_section(target);
            }
        }
    }
}

/* Print some debugging info */
void
ElfSectionTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSectionTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSectionTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %u bytes into strtab\n",          p, w, "sh_name",        sh_name);
    fprintf(f, "%s%-*s = %u\n",                            p, w, "sh_type",        sh_type);
    fprintf(f, "%s%-*s = %u\n",                            p, w, "sh_link",        sh_link);
    fprintf(f, "%s%-*s = %u\n",                            p, w, "sh_info",        sh_info);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",               p, w, "sh_flags",       sh_flags);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",               p, w, "sh_addr",        sh_addr);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes info file\n",   p, w, "sh_offset",      sh_offset);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",             p, w, "sh_size",        sh_size);
    fprintf(f, "%s%-*s = %" PRIu64 "\n",                   p, w, "sh_addralign",   sh_addralign);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",             p, w, "sh_entsize",     sh_entsize);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",             p, w, "nextra",         nextra);
    if (nextra>0)
        fprintf(f, "%s%-*s = %s\n",                        p, w, "extra",          "<FIXME>");
}

/* Print some debugging info */
void
ElfSectionTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSectionTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSectionTable.", prefix);
    }

    ExecSection::dump(f, p, -1);
}
    

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Segment tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Converts 32-bit disk representation to host representation */
void
ElfSegmentTableEntry::ctor(ByteOrder sex, const Elf32SegmentTableEntry_disk *disk) 
{
    p_type      = disk_to_host(sex, disk->p_type);
    p_offset    = disk_to_host(sex, disk->p_offset);
    p_vaddr     = disk_to_host(sex, disk->p_vaddr);
    p_paddr     = disk_to_host(sex, disk->p_paddr);
    p_filesz    = disk_to_host(sex, disk->p_filesz);
    p_memsz     = disk_to_host(sex, disk->p_memsz);
    p_flags     = disk_to_host(sex, disk->p_flags);
    p_align     = disk_to_host(sex, disk->p_align);
}

/* Converts 64-bit disk representation to host representation */
void
ElfSegmentTableEntry::ctor(ByteOrder sex, const Elf64SegmentTableEntry_disk *disk) 
{
    p_type      = disk_to_host(sex, disk->p_type);
    p_offset    = disk_to_host(sex, disk->p_offset);
    p_vaddr     = disk_to_host(sex, disk->p_vaddr);
    p_paddr     = disk_to_host(sex, disk->p_paddr);
    p_filesz    = disk_to_host(sex, disk->p_filesz);
    p_memsz     = disk_to_host(sex, disk->p_memsz);
    p_flags     = disk_to_host(sex, disk->p_flags);
    p_align     = disk_to_host(sex, disk->p_align);
}

/* Print some debugging info */
void
ElfSegmentTableEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSegmentTableEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSegmentTableEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    fprintf(f, "%s%-*s = %u\n",                              p, w, "p_type",         p_type);
    fprintf(f, "%s%-*s = 0x%08x\n",                          p, w, "p_flags",        p_flags);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 " bytes into file\n", p, w, "p_offset",       p_offset);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",                 p, w, "p_vaddr",        p_vaddr);
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",                 p, w, "p_paddr",        p_paddr);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",               p, w, "p_filesz",       p_filesz);
    fprintf(f, "%s%-*s = %" PRIu64 " bytes\n",               p, w, "p_memsz",        p_memsz);
    fprintf(f, "%s%-*s = %" PRIu64 " byte boundary\n",       p, w, "p_align",        p_align);
    if (nextra>0)
        fprintf(f, "%s%-*s = %s\n",                           p, w, "extra",          "<FIXME>");
}

/* Constructor reads the Elf Segment (Program Header) Table */
void
ElfSegmentTable::ctor(ElfFileHeader *fhdr)
{
    
    set_synthesized(true);                              /* the segment table isn't part of any explicit section */
    set_name("ELF Segment Table");
    set_purpose(SP_HEADER);
    
    ByteOrder sex = fhdr->get_sex();
    
    if (fhdr->e_phnum>0) {

        /* Check sizes */
        ROSE_ASSERT(4==fhdr->get_word_size() || 8==fhdr->get_word_size());
        size_t struct_size = 4==fhdr->get_word_size() ?
                             sizeof(Elf32SegmentTableEntry_disk) : sizeof(Elf64SegmentTableEntry_disk);
        if (fhdr->e_phentsize < struct_size)
            throw FormatError("ELF header phentsize is too small");

        /* Read all segment headers. We can't just cast this to an array like with other structs because the ELF header
         * specifies the size of each entry. */
        addr_t offset=0;                                /* w.r.t. the beginning of this section */
        for (size_t i=0; i<fhdr->e_phnum; i++, offset+=fhdr->e_phentsize) {
            ElfSegmentTableEntry *shdr=NULL;
            if (4==fhdr->get_word_size()) {
                const Elf32SegmentTableEntry_disk *disk = (const Elf32SegmentTableEntry_disk*)content(offset, struct_size);
                shdr = new ElfSegmentTableEntry(sex, disk);
            } else {
                const Elf64SegmentTableEntry_disk *disk = (const Elf64SegmentTableEntry_disk*)content(offset, struct_size);
                shdr = new ElfSegmentTableEntry(sex, disk);
            }
            shdr->nextra = fhdr->e_phentsize - struct_size;
            if (shdr->nextra>0)
                shdr->extra = content(offset+struct_size, shdr->nextra);
            entries.push_back(shdr);
        }
    }

    /* Read the segments */
    for (size_t i=0; i<entries.size(); i++) {
        ElfSegmentTableEntry *shdr = entries[i];

        /* Get (or synthesize) the section in which this segment lives. */
        std::vector<ExecSection*> found = fhdr->get_file()->get_sections_by_offset(shdr->p_offset, shdr->p_filesz);
        ExecSection *section = NULL;
        if (found.size()>0) {
            section = found[0];
        } else {
            section = new ExecSection(fhdr->get_file(), shdr->p_offset, shdr->p_filesz);
            section->set_synthesized(true);
            section->set_name("synthesized segment container");
            section->set_purpose(SP_UNSPECIFIED);
        }

        /* Calculate offsets and sizes */
        addr_t offset_wrt_section = shdr->p_offset - section->get_offset();
        ROSE_ASSERT(shdr->p_vaddr >= fhdr->get_base_va());
        addr_t rva = shdr->p_vaddr - fhdr->get_base_va();

        /* Read the segment */
        ExecSegment *segment = NULL;
        if (PT_DYNAMIC==shdr->p_type) {
            ElfSection *elf_section = dynamic_cast<ElfSection*>(section); /*Dynamic segments always live in real sections!*/
            segment = new ElfDynamicSegment(fhdr, elf_section, offset_wrt_section, shdr->p_filesz, rva, shdr->p_memsz);
        } else {
            segment = new ExecSegment(section, offset_wrt_section, shdr->p_filesz, rva, shdr->p_memsz);
            switch (shdr->p_type) {
              case PT_NULL:    segment->set_name("NULL");    break;
              case PT_LOAD:    segment->set_name("LOAD");    break;
              case PT_DYNAMIC: segment->set_name("DYNAMIC"); break;
              case PT_INTERP:  segment->set_name("INPERP");  break;
              case PT_NOTE:    segment->set_name("NOTE");    break;
              case PT_SHLIB:   segment->set_name("SHLIB");   break;
              case PT_PHDR:    segment->set_name("PHDR");    break;
              default:
                char s[128];
                sprintf(s, "p_type=0x%08x", shdr->p_type);
                segment->set_name(s);
                break;
            }
        }
        segment->set_executable(shdr->p_flags & 0x1 ? true : false);
        segment->set_writable  (shdr->p_flags & 0x2 ? true : false);
    }
}
    
/* Print some debugging info */
void
ElfSegmentTable::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sSegmentTable[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sSegmentTable.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zd entries\n", p, w, "size", entries.size());
    for (size_t i=0; i<entries.size(); i++) {
        entries[i]->dump(f, p, i);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Linking
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructors */
void
ElfDynamicEntry::ctor(ByteOrder sex, const Elf32DynamicEntry_disk *disk)
{
    d_tag = disk_to_host(sex, disk->d_tag);
    d_val = disk_to_host(sex, disk->d_val);
}
void
ElfDynamicEntry::ctor(ByteOrder sex, const Elf64DynamicEntry_disk *disk)
{
    d_tag = disk_to_host(sex, disk->d_tag);
    d_val = disk_to_host(sex, disk->d_val);
}

/* Print some debugging info */
void
ElfDynamicEntry::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfDynamicEntry[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfDynamicEntry.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    
    if (d_tag>=34) {
        fprintf(f, "%s%-*s = 0x%08u\n",          p, w, "d_tag",      d_tag);
    } else {
        fprintf(f, "%s%-*s = %u\n",              p, w, "d_tag",      d_tag);
    }
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n",     p, w, "d_val_addr", d_val);
}

/* Constructor */
void
ElfDynamicSegment::ctor(ElfFileHeader *fhdr, ElfSection *section,
                        addr_t offset_wrt_section, addr_t file_size, addr_t rva, addr_t mem_size)
{
    set_name("DYNAMIC");
    ElfSection *dynamic_strtab = section->get_linked_section();

    std::vector<ElfDynamicEntry> entries;
    size_t entry_size=0, nentries=0;
    if (4==fhdr->get_word_size()) {
        const Elf32DynamicEntry_disk *disk = (const Elf32DynamicEntry_disk*)section->content(offset_wrt_section, file_size);
        entry_size = sizeof(Elf32DynamicEntry_disk);
        nentries = file_size / entry_size;
        for (size_t i=0; i<nentries; i++) {
            entries.push_back(ElfDynamicEntry(fhdr->get_sex(), disk+i));
        }
    } else if (8==fhdr->get_word_size()) {
        const Elf64DynamicEntry_disk *disk = (const Elf64DynamicEntry_disk*)section->content(offset_wrt_section, file_size);
        entry_size = sizeof(Elf64DynamicEntry_disk);
        nentries = file_size / entry_size;
        for (size_t i=0; i<nentries; i++) {
            entries.push_back(ElfDynamicEntry(fhdr->get_sex(), disk+i));
        }
    } else {
        throw FormatError("bad ELF word size");
    }

    for (size_t i=0; i<nentries; i++) {
        switch (entries[i].d_tag) {
          case 0:
            /* DT_NULL: unused entry */
            break;
          case 1:
            /* DT_NEEDED: offset to NUL-terminated library name in the linked-to (".dynstr") section. */
            if (dynamic_strtab) {
                const char *name = (const char*)dynamic_strtab->content_str(entries[i].d_val);
                fhdr->add_dll(new ExecDLL(name));
            } else {
                throw FormatError("DYNAMIC segment is not linked to a string table");
            }
            break;
          case 2:
            dt_pltrelsz = entries[i].d_val;
            break;
          case 3:
            dt_pltgot = entries[i].d_val;
            break;
          case 4:
            dt_hash = entries[i].d_val;
            break;
          case 5:
            dt_strtab = entries[i].d_val;
            break;
          case 6:
            dt_symtab = entries[i].d_val;
            break;
          case 7:
            dt_rela = entries[i].d_val;
            break;
          case 8:
            dt_relasz = entries[i].d_val;
            break;
          case 9:
            dt_relaent = entries[i].d_val;
            break;
          case 10:
            dt_strsz = entries[i].d_val;
            break;
          case 11:
            dt_symentsz = entries[i].d_val;
            break;
          case 12:
            dt_init = entries[i].d_val;
            break;
          case 13:
            dt_fini = entries[i].d_val;
            break;
          case 20:
            dt_pltrel = entries[i].d_val;
            break;
          case 23:
            dt_jmprel = entries[i].d_val;
            break;
          case 0x6fffffff:
            dt_verneednum = entries[i].d_val;
            break;
          case 0x6ffffffe:
            dt_verneed = entries[i].d_val;
            break;
          case 0x6ffffff0:
            dt_versym = entries[i].d_val;
            break;
          default:
            other.push_back(entries[i]);
            break;
        }
    }
}

/* Helper for ElfDynamicSegment::dump */
static void
dump_section_rva(FILE *f, const char *p, int w, const char *name, addr_t addr, ExecFile *ef)
{
    fprintf(f, "%s%-*s = 0x%08" PRIx64 "\n", p, w, name, addr);
    std::vector<ExecSection*> sections = ef->get_sections_by_rva(addr);
    for (size_t i=0; i<sections.size(); i++) {
        fprintf(f, "%s%-*s     [%d] \"%s\"", p, w, "...", sections[i]->get_id(), sections[i]->get_name().c_str());
        addr_t offset = addr - sections[i]->get_mapped();
        if (offset>0) {
            addr_t nbytes = sections[i]->get_size() - offset;
            fprintf(f, " @(0x%08"PRIx64"+%"PRIu64") %"PRIu64" bytes", sections[i]->get_mapped(), offset, nbytes);
        } else {
            fprintf(f, " @0x%08"PRIx64" %"PRIu64" bytes" , sections[i]->get_mapped(), sections[i]->get_size());
        }
        fprintf(f, "\n");
    }
}

/* Print some debugging info */
void
ElfDynamicSegment::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sDynamicSegment[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sDynamicSegment.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));
    ExecFile *ef = get_section()->get_file();

    ExecSegment::dump(f, p, -1);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "pltrelsz",   dt_pltrelsz);
    dump_section_rva(f, p, w, "pltgot",  dt_pltgot, ef);
    dump_section_rva(f, p, w, "hash",    dt_hash,   ef);
    dump_section_rva(f, p, w, "strtab",  dt_strtab, ef);
    dump_section_rva(f, p, w, "symtab",  dt_symtab, ef);
    dump_section_rva(f, p, w, "rela",    dt_rela,   ef);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "relasz",     dt_relasz);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "relaent",    dt_relaent);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "strsz",      dt_strsz);
    fprintf(f, "%s%-*s = %u bytes\n",        p, w, "symentsz",   dt_symentsz);
    dump_section_rva(f, p, w, "init",    dt_init,   ef);
    dump_section_rva(f, p, w, "fini",    dt_fini,   ef);
    fprintf(f, "%s%-*s = %u\n",              p, w, "pltrel",     dt_pltrel);
    dump_section_rva(f, p, w, "jmprel",  dt_jmprel, ef);
    fprintf(f, "%s%-*s = %u\n",              p, w, "verneednum", dt_verneednum);
    dump_section_rva(f, p, w, "verneed", dt_verneed, ef);
    dump_section_rva(f, p, w, "versym",  dt_versym,  ef);
    
    for (size_t i=0; i<other.size(); i++) {
        other[i].dump(f, p, i);
    }
}
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Symbol Tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Constructors */
void
ElfSymbol::ctor(ByteOrder sex, const Elf32SymbolEntry_disk *disk)
{
    st_name =  disk_to_host(sex, disk->st_name);
    st_info =  disk_to_host(sex, disk->st_info);
    st_res1 =  disk_to_host(sex, disk->st_res1);
    st_shndx = disk_to_host(sex, disk->st_shndx);

    value = disk_to_host(sex, disk->st_value);
    size =  disk_to_host(sex, disk->st_size);
    ctor_common();
}
void
ElfSymbol::ctor(ByteOrder sex, const Elf64SymbolEntry_disk *disk)
{
    st_name =  disk_to_host(sex, disk->st_name);
    st_info =  disk_to_host(sex, disk->st_info);
    st_res1 =  disk_to_host(sex, disk->st_res1);
    st_shndx = disk_to_host(sex, disk->st_shndx);

    value = disk_to_host(sex, disk->st_value);
    size =  disk_to_host(sex, disk->st_size);
    ctor_common();
}
void
ElfSymbol::ctor_common()
{
    /* Binding */
    switch (get_elf_binding()) {
      case STB_LOCAL:   binding = SYM_LOCAL;  break;
      case STB_GLOBAL:  binding = SYM_GLOBAL; break;
      case STB_WEAK:    binding = SYM_WEAK;   break;
      default:
        fprintf(stderr, "ROBB: unknown elf symbol binding: %u\n", get_elf_binding());
        ROSE_ASSERT(0);
        break;
    }

    /* Type */
    switch (get_elf_type()) {
      case STT_NOTYPE:  type = SYM_NO_TYPE; break;
      case STT_OBJECT:  type = SYM_DATA;    break;
      case STT_FUNC:    type = SYM_FUNC;    break;
      case STT_SECTION: type = SYM_SECTION; break;
      case STT_FILE:    type = SYM_FILE;    break;
      default:
        fprintf(stderr, "ROBB: unknown elf symbol type: %u\n", get_elf_type());
        ROSE_ASSERT(0);
        break;
    }

    /* Definition state */
    if (value || size) {
        def_state = SYM_DEFINED;
    } else if (st_name>0 || get_elf_type()) {
        def_state = SYM_TENTATIVE;
    } else {
        def_state = SYM_UNDEFINED;
    }
}

/* Print some debugging info. The 'section' is an optional section pointer for the st_shndx member. */
void
ElfSymbol::dump(FILE *f, const char *prefix, ssize_t idx, ExecSection *section)
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

    ExecSymbol::dump(f, p, -1);

    fprintf(f, "%s%-*s = %"PRIu64" offset into strtab\n", p, w, "st_name",  st_name);

    fprintf(f, "%s%-*s = %u (",          p, w, "st_info",  st_info);
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

    fprintf(f, "%s%-*s = %u\n",         p, w, "st_res1",  st_res1);

    if (section && section->get_id()==(int)st_shndx) {
        fprintf(f, "%s%-*s = [%d] \"%s\" @%"PRIu64", %"PRIu64" bytes\n", p, w, "st_shndx",
                section->get_id(), section->get_name().c_str(), section->get_offset(), section->get_size());
    } else {
        fprintf(f, "%s%-*s = %u\n",         p, w, "st_shndx", st_shndx);        
    }
}

/* Constructor */
void
ElfSymbolSection::ctor(ElfFileHeader *fhdr, ElfSectionTableEntry *shdr)
{
    if (4==fhdr->get_word_size()) {
        const Elf32SymbolEntry_disk *disk = (const Elf32SymbolEntry_disk*)content(0, get_size());
        size_t nentries = get_size() / sizeof(Elf32SymbolEntry_disk);
        for (size_t i=0; i<nentries; i++) {
            symbols.push_back(new ElfSymbol(fhdr->get_sex(), disk+i));
        }
    } else {
        const Elf64SymbolEntry_disk *disk = (const Elf64SymbolEntry_disk*)content(0, get_size());
        size_t nentries = get_size() / sizeof(Elf64SymbolEntry_disk);
        for (size_t i=0; i<nentries; i++) {
            symbols.push_back(new ElfSymbol(fhdr->get_sex(), disk+i));
        }
    }
}

/* Symbol table sections link to their string tables. Updating the string table should cause the symbol names to be updated. */
void
ElfSymbolSection::set_linked_section(ElfSection *strtab)
{
    ElfSection::set_linked_section(strtab);
    for (size_t i=0; i<symbols.size(); i++) {
        symbols[i]->set_name(strtab->content_str(symbols[i]->st_name));
    }
}

/* Print some debugging info */
void
ElfSymbolSection::dump(FILE *f, const char *prefix, ssize_t idx)
{
    char p[4096];
    if (idx>=0) {
        sprintf(p, "%sElfSymbolSection[%zd].", prefix, idx);
    } else {
        sprintf(p, "%sElfSymbolSection.", prefix);
    }
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ElfSection::dump(f, p, -1);
    fprintf(f, "%s%-*s = %zu symbols\n", p, w, "size", symbols.size());
    for (size_t i=0; i<symbols.size(); i++) {
        ExecSection *section = get_file()->get_section_by_id(symbols[i]->st_shndx);
        symbols[i]->dump(f, p, i, section);
    }
}
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Returns true if a cursory look at the file indicates that it could be an ELF file. */
bool
is_ELF(ExecFile *f)
{
    ElfFileHeader *hdr = NULL;
    
    try {
        hdr = new ElfFileHeader(f, 0);
    } catch (...) {
        return false;
    }

    bool ret = (hdr->get_magic().size()==4 &&
                hdr->get_magic()[0]==0x7f && hdr->get_magic()[1]=='E' &&
                hdr->get_magic()[2]=='L'  && hdr->get_magic()[3]=='F');
    delete hdr;
    return ret;
}

/* Parses the structure of an ELF file and adds the information to the asmFile */
void
parseBinaryFormat(ExecFile *f, SgAsmFile* asmFile)
   {
     ROSE_ASSERT(f);
    
     ElfFileHeader *fhdr = new ElfFileHeader(f, 0);

     ROSE_ASSERT(fhdr != NULL);
     SgAsmElfHeader* roseElfHeader = new SgAsmElfHeader(fhdr);
     ROSE_ASSERT(roseElfHeader != NULL);
     asmFile->set_header(roseElfHeader);

  /* Read the optional section and segment tables and the sections/segments to which they point. */
     if (fhdr->e_shnum)
          fhdr->section_table = new ElfSectionTable(fhdr);

     if (fhdr->e_phnum)
          fhdr->segment_table = new ElfSegmentTable(fhdr);

  /* Identify parts of the file that we haven't encountered during parsing */
     f->fill_holes();
   }

/* Parses the structure of an ELF file and adds the info to the ExecFile */
ElfFileHeader *
parse(ExecFile *ef)
{
    ROSE_ASSERT(ef);
    
    ElfFileHeader *fhdr = new ElfFileHeader(ef, 0);
    ROSE_ASSERT(fhdr != NULL);

    /* Read the optional section and segment tables and the sections/segments to which they point. */
    if (fhdr->e_shnum)
        fhdr->section_table = new ElfSectionTable(fhdr);

    if (fhdr->e_phnum)
        fhdr->segment_table = new ElfSegmentTable(fhdr);

    /* Use symbols from either ".symtab" or ".dynsym" */
    ElfSymbolSection *symtab = dynamic_cast<ElfSymbolSection*>(ef->get_section_by_name(".symtab"));
    if (!symtab)
        symtab = dynamic_cast<ElfSymbolSection*>(ef->get_section_by_name(".dynsym"));
    if (symtab) {
        std::vector<ElfSymbol*> &symbols = symtab->get_symbols();
        for (size_t i=0; i<symbols.size(); i++)
            fhdr->add_symbol(symbols[i]);
    }

    /* Identify parts of the file that we haven't encountered during parsing */
    ef->fill_holes();
    return fhdr;
}
    
    
}; //namespace ELF
}; //namespace Exec
