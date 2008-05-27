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
    fileFormat.family = FAMILY_ELF;
    switch (e_type) {
      case 0:
        fileFormat.purpose = PURPOSE_UNSPECIFIED;
        break;
      case 1:
      case 3:
        fileFormat.purpose = PURPOSE_LIBRARY;
        break;
      case 2:
        fileFormat.purpose = PURPOSE_EXECUTABLE;
        break;
      case 4:
        fileFormat.purpose = PURPOSE_CORE_DUMP;
        break;
      default:
        if (e_type>=0xff00 && e_type<=0xffff) {
            fileFormat.purpose = PURPOSE_PROC_SPECIFIC;
        } else {
            fileFormat.purpose = PURPOSE_OTHER;
        }
        break;
    }
    fileFormat.sex = sex;
    fileFormat.version = e_version;
    fileFormat.is_current_version = (1==e_version);
    fileFormat.abi = ABI_UNSPECIFIED;                   /* ELF specifies a target architecture rather than an ABI */
    fileFormat.abi_version = 0;
    fileFormat.word_size = disk32 ? 4 : 8;

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

/* Print some debugging info */
void
ElfFileHeader::dump(FILE *f, const char *prefix)
{
    char p[4096];
    sprintf(p, "%sElfFileHeader.", prefix);
    int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecHeader::dump(f, p);
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

    ByteOrder sex = fhdr->fileFormat.sex;

    if (fhdr->e_shnum>0) {
        ROSE_ASSERT(4==fhdr->fileFormat.word_size || 8==fhdr->fileFormat.word_size);
        if ((4==fhdr->fileFormat.word_size && fhdr->e_shentsize < sizeof(Elf32SectionTableEntry_disk)) ||
            (8==fhdr->fileFormat.word_size && fhdr->e_shentsize < sizeof(Elf64SectionTableEntry_disk)))
            throw FormatError("ELF header shentsize is too small");

        ElfSectionTableEntry *strtab = NULL;            /* Section containing section names */
        addr_t offset=0;                                /* w.r.t. the beginning of this section */
        for (size_t i=0; i<fhdr->e_shnum; i++, offset+=fhdr->e_shentsize) {
            /* Read the section header */
            ElfSectionTableEntry *shdr=NULL;
            size_t struct_size=0;
            if (4==fhdr->fileFormat.word_size) {
                struct_size = sizeof(Elf32SectionTableEntry_disk);
                const Elf32SectionTableEntry_disk *disk = (const Elf32SectionTableEntry_disk*)content(offset, struct_size);
                shdr = new ElfSectionTableEntry(sex, disk);
            } else if (8==fhdr->fileFormat.word_size) {
                struct_size = sizeof(Elf64SectionTableEntry_disk);
                const Elf64SectionTableEntry_disk *disk = (const Elf64SectionTableEntry_disk*)content(offset, struct_size);
                shdr = new ElfSectionTableEntry(sex, disk);
            } else {
                ROSE_ASSERT(!"invalid word size");
            }

            /* If the disk struct is smaller than the section header size as advertised in the file header then claim the
             * extra data as part of the section header. We already know that fhdr->shentsize is larger than struct_size. */
            shdr->nextra = fhdr->e_shentsize - struct_size;
            if (shdr->nextra>0)
                shdr->extra = content(offset+struct_size, shdr->nextra);

            /* Read the section itself */
            if (shdr->sh_type!=SHT_NOBITS && shdr->sh_type!=SHT_NULL) {
                shdr->section = new ExecSection(fhdr->get_file(), shdr->sh_offset, shdr->sh_size);
                if (i==fhdr->e_shstrndx) strtab = shdr;
                shdr->section->set_synthesized(false);
                shdr->section->set_id(i);
                switch (shdr->sh_type) {
                  case SHT_PROGBITS:
                    shdr->section->set_purpose(SP_PROGRAM);
                    break;
                  case SHT_STRTAB:
                    shdr->section->set_purpose(SP_HEADER);
                    break;
                  default:
                    shdr->section->set_purpose(SP_OTHER);
                    break;
                }
            }

            /* The section table entry is a member of the section table */
            entries.push_back(shdr);
        }

        /* After we've read all the sections, go back and initialize the names. We have to do this last because the names
         * are stored in one of the section (the "section name table" section). The name table should have the a NUL as the
         * first byte. All remaining strings are NUL terminated and we check that here.
         *
         * FIXME: It's possible to hide stuff in the name table since (1) section names might point into the middle of a
         *        string (such as the a section named "foo" pointing into "malwarefoo\0" in the table) and (2) not all
         *        strings need to be explicitly referenced from elsewhere. */
        if (strtab && strtab->section) {
            for (std::vector<ElfSectionTableEntry*>::iterator i=entries.begin(); i!=entries.end(); i++) {
                addr_t string_offset = (*i)->sh_name;
                if (string_offset >= strtab->section->get_size())
                    throw FormatError("ELF section string offset is beyond end of string table");
                addr_t remaining_bytes = strtab->section->get_size() - string_offset;
                const unsigned char *name = strtab->section->content(string_offset, remaining_bytes);
                for (addr_t j=0; j<=remaining_bytes; j++) {
                    if (j>=remaining_bytes)
                        throw FormatError("ELF string table entry is not NUL terminated");
                    if (!name[j]) break;
                }
                if ((*i)->section) {
                    (*i)->section->set_name(name);
                }
            }
        }
    }
}

/* Print some debugging info */
void
ElfSectionTableEntry::dump(FILE *f, const char *prefix)
{
    const char *p = prefix; /* Don't append to prefix since caller (ElfSectionTable::dump) is adding array indices. */
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

/* Look up a section based on some range of file addresses specified with an offset and size. Look only at the sections
 * defined in the Section Table and return only the first match found. */
ElfSectionTableEntry *
ElfSectionTable::lookup(addr_t offset, addr_t size)
{
    for (std::vector<ElfSectionTableEntry*>::iterator si=entries.begin(); si!=entries.end(); si++) {
        ElfSectionTableEntry *shdr = *si;
        if (offset >= shdr->sh_offset) {
            addr_t offset_wrt_section = offset - shdr->sh_offset;
            if (offset_wrt_section + size <= shdr->sh_size) {
                return shdr;
            }
        }
    }
    return NULL;
}

/* Print some debugging info */
void
ElfSectionTable::dump(FILE *f, const char *prefix)
{
    char p[4096];
    sprintf(p, "%sSectionTable.", prefix);
    const int w = std::max(1, DUMP_FIELD_WIDTH-(int)strlen(p));

    ExecSection::dump(f, p);
    fprintf(f, "%s%-*s = %zu entries\n", p, w, "size", entries.size());
    for (size_t i=0; i<entries.size(); i++) {
        sprintf(p, "%sSectionTable.entries[%zu].", prefix, i);
        entries[i]->dump(f, p);
    }
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

/* Constructor reads the Elf Segment (Program Header) Table */
void
ElfSegmentTable::ctor(ElfFileHeader *fhdr)
{
    
    set_synthesized(true);                              /* the segment table isn't part of any explicit section */
    set_name("ELF Segment Table");
    set_purpose(SP_HEADER);
    
    ByteOrder sex = fhdr->fileFormat.sex;
    
    if (fhdr->e_phnum>0) {
        ROSE_ASSERT(4==fhdr->fileFormat.word_size || 8==fhdr->fileFormat.word_size);
        if ((4==fhdr->fileFormat.word_size && fhdr->e_phentsize < sizeof(Elf32SegmentTableEntry_disk)) ||
            (8==fhdr->fileFormat.word_size && fhdr->e_phentsize < sizeof(Elf64SegmentTableEntry_disk)))
            throw FormatError("ELF header phentsize is too small");

        addr_t offset=0;                                /* w.r.t. the beginning of this section */
        for (size_t i=0; i<fhdr->e_phnum; i++, offset+=fhdr->e_phentsize) {
            /* Read the segment header */
            ElfSegmentTableEntry *shdr=NULL;
            size_t struct_size=0;
            if (4==fhdr->fileFormat.word_size) {
                struct_size = sizeof(Elf32SegmentTableEntry_disk);
                const Elf32SegmentTableEntry_disk *disk = (const Elf32SegmentTableEntry_disk*)content(offset, struct_size);
                shdr = new ElfSegmentTableEntry(sex, disk);
            } else if (8==fhdr->fileFormat.word_size) {
                struct_size = sizeof(Elf64SegmentTableEntry_disk);
                const Elf64SegmentTableEntry_disk *disk = (const Elf64SegmentTableEntry_disk*)content(offset, struct_size);
                shdr = new ElfSegmentTableEntry(sex, disk);
            } else {
                ROSE_ASSERT(!"invalid word size");
            }
            
            /* If the disk struct is smaller than the segment header size as advertised in the file header then claim the extra
             * data as part of the segment header. We already know that fhdr->phentsize is larger than struct_size. */
            shdr->nextra = fhdr->e_phentsize - struct_size;
            if (shdr->nextra>0)
                shdr->extra = content(offset+struct_size, shdr->nextra);
            
            /* Read the segment itself */
            //FIXME

            /* The segment table entry is a member of the segment table */
            entries.push_back(shdr);
        }
    }

    /* The ElfFileHeader's base address (base_va) is the lowest virtual address associated with the memory image of the
     * programs and can be used to relocate the memory image of the program during dynamic linking. It is calculated here
     * (and in the program loader) since it's not stored explicitly in the file. */
    ROSE_ASSERT(fhdr->e_phnum>0); /*FIXME: no base_va if no segments?*/
    ROSE_ASSERT(0==fhdr->base_va); /*shouldn't be set yet since elf has no explicit base address*/
    addr_t lowest_vaddr=~(addr_t)0;
    for (std::vector<ElfSegmentTableEntry*>::iterator si=entries.begin(); si!=entries.end(); si++) {
        if ((*si)->p_type != PT_LOAD && 
            (*si)->p_vaddr < lowest_vaddr) {
            lowest_vaddr = (*si)->p_vaddr;
            fhdr->base_va = ALIGN((*si)->p_vaddr, fhdr->max_page_size());
        }
    }

    /* Load the segments */
    for (std::vector<ElfSegmentTableEntry*>::iterator si=entries.begin(); si!=entries.end(); si++) {
        ElfSegmentTableEntry *shdr = *si;

        /* Get (or synthesize) the section in which this segment lives. */
        ExecSection *section=NULL;
        ElfSectionTableEntry *section_hdr = fhdr->section_table ? 
                                            fhdr->section_table->lookup(shdr->p_offset, shdr->p_filesz) :
                                            NULL;
        if (section_hdr && section_hdr->section) {
            section = section_hdr->section;
        } else {
            section = new ExecSection(fhdr->get_file(), shdr->p_offset, shdr->p_filesz);
            section->set_synthesized(true);
            section->set_name("synthesized segment container");
            section->set_purpose(SP_UNSPECIFIED);
        }

        addr_t offset_wrt_section = shdr->p_offset - section->get_offset();
        ROSE_ASSERT(shdr->p_vaddr >= fhdr->base_va); /*from calculations for the base_va above*/
        addr_t rva = shdr->p_vaddr - fhdr->base_va; /*relative virtual address of segment*/
        ExecSegment *segment = new ExecSegment(section, offset_wrt_section, shdr->p_filesz, rva, shdr->p_memsz);
        segment->set_executable(shdr->p_flags & 0x1 ? true : false);
        segment->set_writable  (shdr->p_flags & 0x2 ? true : false);

        /* Segment name */
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

    bool ret = hdr->magic.size()==4 && hdr->magic[0]==0x7f && hdr->magic[1]=='E' && hdr->magic[2]=='L' && hdr->magic[3]=='F';
    delete hdr;
    return ret;
}

/* Parses the structure of an ELF file and adds the information to the ExecFile */
void
parse(ExecFile *f)
{
    ROSE_ASSERT(f);
    
    ElfFileHeader *fhdr = new ElfFileHeader(f, 0);
    ExecSection *strtab = NULL; /*section containing strings for section names*/

    /* Read the optional section and segment tables and the sections/segments to which they point. */
    if (fhdr->e_shnum) fhdr->section_table = new ElfSectionTable(fhdr);
    if (fhdr->e_phnum) fhdr->segment_table = new ElfSegmentTable(fhdr);

    /* Identify parts of the file that we haven't encountered during parsing */
    f->find_holes();
}
    
}; //namespace ELF
}; //namespace Exec
