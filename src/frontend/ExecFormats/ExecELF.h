/* Copyright 2008 Lawrence Livermore National Security, LLC */
#ifndef Exec_ExecELF_h
#define Exec_ExecELF_h

#include "ExecGeneric.h"

namespace Exec {
namespace ELF {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ELF File Headers
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* File format of an ELF header. Byte order of members depends on e_ident value. This code comes directly from "System V
 * Application Binary Interface, Edition 4.1" and the FreeBSD elf(5) man page, and the "Executable and Linkable Format (ELF)
 * Portable Formats Specifications, Version 1.2" and not from any header file. */
struct Elf32FileHeader_disk {
    unsigned char       e_ident_magic[4];       /* 0x7f, 'E', 'L', 'F' */
    unsigned char       e_ident_file_class;     /* 1=>32-bit; 2=>64-bit; other is error */
    unsigned char       e_ident_data_encoding;  /* 1=>LSB; 2=>MSB; other is error */
    unsigned char       e_ident_file_version;   /* Format version number (same as 'version' member); must be one. */
    unsigned char       e_ident_padding[9];     /* padding to align next member at byte offset 16; must be zero */
    uint16_t            e_type;                 /* object file type: relocatable, executable, shared object, core, etc. */
    uint16_t            e_machine;              /* required architecture for an individual file */
    uint32_t            e_version;              /* object file version, currently zero or one */
    uint32_t            e_entry;                /* entry virtual address or zero if none */
    uint32_t            e_phoff;                /* file offset of program header (segment) table or zero if none */
    uint32_t            e_shoff;                /* file offset of section header table or zero if none */
    uint32_t            e_flags;                /* processor-specific flags (EF_* constants in documentation) */
    uint16_t            e_ehsize;               /* size of ELF header in bytes */
    uint16_t            e_phentsize;            /* size in bytes of each entry in the program header table */
    uint16_t            e_phnum;                /* number of entries in the program header (segment) table, or PN_XNUM, or zero */
    uint16_t            e_shentsize;            /* size in bytes of each entry in the section header table */
    uint16_t            e_shnum;                /* number of entries in the section header table, or zero for extended entries */
    uint16_t            e_shstrndx;             /* index of section containing section name strings, or SHN_UNDEF, or SHN_XINDEX */
};
struct Elf64FileHeader_disk {
    unsigned char       e_ident_magic[4];
    unsigned char       e_ident_file_class;
    unsigned char       e_ident_data_encoding;
    unsigned char       e_ident_file_version;
    unsigned char       e_ident_padding[9];
    uint16_t            e_type;
    uint16_t            e_machine;
    uint32_t            e_version;
    uint64_t            e_entry;
    uint64_t            e_phoff;
    uint64_t            e_shoff;
    uint32_t            e_flags;
    uint16_t            e_ehsize;
    uint16_t            e_phentsize;
    uint16_t            e_phnum;
    uint16_t            e_shentsize;
    uint16_t            e_shnum;
    uint16_t            e_shstrndx;
};

class ElfFileHeader : public ExecHeader {
  public:
    ElfFileHeader(ExecFile *f, addr_t offset)   /* assume 32-bit for now and fix inside ctor() if necessary */
        : ExecHeader(f, offset, sizeof(Elf32FileHeader_disk)) {ctor(f, offset);}
    uint64_t max_page_size();
    virtual void dump(FILE*, const char *prefix);

    /* These are the native-format versions of the same members described in Elf*FileHeader_disk */
    unsigned char       e_ident_file_class, e_ident_data_encoding, e_ident_file_version, e_ident_padding[9];
    unsigned            e_type, e_machine, e_version;
    addr_t              e_entry, e_phoff, e_shoff;
    unsigned            e_flags, e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx;

    /* Additional members not trivially present in the file header but logically part of an Elf header */
    class ElfSectionTable *section_table;
    class ElfSegmentTable *segment_table;

  private:
    void ctor(ExecFile *f, addr_t offset);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ELF Section Table Entries (i.e., section headers) and the table itself.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Section types (host order). All other values are reserved. */
enum SectionType {
    SHT_NULL            = 0,                    /* Section header is inactive */
    SHT_PROGBITS        = 1,                    /* Information defined by the program; format and meaning determined by prog */
    SHT_SYMTAB          = 2,                    /* Complete symbol table */
    SHT_STRTAB          = 3,                    /* String table */
    SHT_RELA            = 4,                    /* Relocation entries with explicit addends (e.g., Elf32_Rela types) */
    SHT_HASH            = 5,                    /* Symbol hash table (used by dynamic linking) */
    SHT_DYNAMIC         = 6,                    /* Information for dynamic linking */
    SHT_NOTE            = 7,                    /* Information that marks the file in some way */
    SHT_NOBITS          = 8,                    /* Like SHT_PROGBITS but occupies no file space */
    SHT_REL             = 9,                    /* Relocation entries without explicit addends (e.g., Elf32_Rel types) */
    SHT_SHLIB           = 10,                   /* Reserved, unspecified semantics; Present only in non-conforming files */
    SHT_DYNSYM          = 11,                   /* Minimal set of dynamic linking symbols */
    SHT_LOPROC          = 0x70000000,           /* Processor specific semantics */
    SHT_HIPROC          = 0x7fffffff,
    SHT_LOUSER          = 0x80000000,           /* Application specific semantics */
    SHT_HIUSER          = 0xffffffff
};

/* File format of an ELF Section header. Byte order of members depends on e_ident value in file header. This code comes
 * directly from "Executable and Linkable Format (ELF)", Portable Formats Specification, Version 1.1, Tool Interface
 * Standards (TIS) and not from any header file. The 64-bit structure is gleaned from the Linux elf(5) man page. */
struct Elf32SectionTableEntry_disk {
    uint32_t            sh_name;                /* Section name; index into section header string table */
    uint32_t            sh_type;                /* Section contents and semantics (see SectionType enum) */
    uint32_t            sh_flags;               /* Bit flags */
    uint32_t            sh_addr;                /* Desired mapped address */
    uint32_t            sh_offset;              /* Section location in file unless sh_type==SHT_NOBITS */
    uint32_t            sh_size;                /* Section size in bytes */
    uint32_t            sh_link;                /* Section ID of another section; meaning depends on section type */
    uint32_t            sh_info;                /* Extra info depending on section type */
    uint32_t            sh_addralign;           /* Mapped alignment (0 and 1=>byte aligned); sh_addr must be aligned */
    uint32_t            sh_entsize;             /* If non-zero, size in bytes of each array member in the section */
};
struct Elf64SectionTableEntry_disk {
    uint32_t            sh_name;
    uint32_t            sh_type;
    uint64_t            sh_flags;
    uint64_t            sh_addr;
    uint64_t            sh_offset;
    uint64_t            sh_size;
    uint32_t            sh_link;
    uint32_t            sh_info;
    uint64_t            sh_addralign;
    uint64_t            sh_entsize;
};

class ElfSectionTableEntry {
  public:
    ElfSectionTableEntry(ByteOrder sex, const Elf32SectionTableEntry_disk *disk)
        : extra(NULL), nextra(0) {ctor(sex, disk);}
    ElfSectionTableEntry(ByteOrder sex, const Elf64SectionTableEntry_disk *disk)
        : extra(NULL), nextra(0) {ctor(sex, disk);};
    virtual ~ElfSectionTableEntry() {};
    virtual void dump(FILE*, const char *prefix);

    /* These are the native-format versions of the same members described in Elf*SectionTableEntry_disk */
    unsigned            sh_name, sh_type, sh_link, sh_info;
    uint64_t            sh_flags;
    addr_t              sh_addr, sh_offset, sh_size, sh_addralign, sh_entsize;

    /* The ELF header can define a section table entry to be larger than the Elf*SectionTableEntry_disk struct, so any
     * extra data gets stuffed into this member, which is a pointer directly into the mapped file and is null if there
     * is no extra data. */
    const unsigned char *extra;
    addr_t              nextra;                 /*extra size in bytes*/

    /* The pointer to the actual section on disk, null for sections like SHT_NOBITS */
    ExecSection         *section;

  private:
    void ctor(ByteOrder sex, const Elf32SectionTableEntry_disk *disk);
    void ctor(ByteOrder sex, const Elf64SectionTableEntry_disk *disk);
};

/* The section table is just a file section containing an array of section table entries (i.e., array of section headers) */
class ElfSectionTable : public ExecSection {
  public:
    ElfSectionTable(ElfFileHeader *fhdr)
        : ExecSection(fhdr->get_file(), fhdr->e_shoff, fhdr->e_shnum*fhdr->e_shentsize) {ctor(fhdr);}
    ElfSectionTableEntry *lookup(addr_t offset, addr_t size);
    virtual void dump(FILE*, const char *prefix);

  private:
    void ctor(ElfFileHeader *fhdr);    
    std::vector<ElfSectionTableEntry*> entries;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ELF Segment Table entries (program headers) and the table itself.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Segment types (host order). All other values are reserved. */
enum SegmentType {
    PT_NULL             = 0,                    /* entry is ignored; other values of entry are undefined */
    PT_LOAD             = 1,                    /* loadable by mapping file contents into memory (see "mapping" note below) */
    PT_DYNAMIC          = 2,                    /* dynamic linking information */
    PT_INTERP           = 3,                    /* segment contains NUL-terminated path name of interpreter */
    PT_NOTE             = 4,                    /* auxiliary information */
    PT_SHLIB            = 5,                    /* Reserved w/unspecified semantics; presence causes file to be nonconforming */
    PT_PHDR             = 6,                    /* Segment contains the segment table itself (program header array) */
    PT_LOPROC           = 0x70000000,           /* Values reserved for processor-specific semantics */
    PT_HIPROC           = 0x7fffffff
};

/* File format of an ELF Segment header. Byte order of members depends on e_ident value in file header. This code
 * comes directly from "Executable and Linkable Format (ELF)", Portable Formats Specification, Version 1.1, Tool Interface
 * Standards (TIS) and not from any header file. The 64-bit structure is gleaned from the Linux elf(5) man page. Segment
 * table entries (a.k.a., ELF program headers) either describe process segments or give supplementary info which does not
 * contribute to the process image.
 *
 * Mapping segments:
 * */
struct Elf32SegmentTableEntry_disk {
    uint32_t            p_type;                 /* kind of segment */
    uint32_t            p_offset;               /* file offset */
    uint32_t            p_vaddr;                /* desired mapped address of segment */
    uint32_t            p_paddr;                /* physical address where supported (unused by System V) */
    uint32_t            p_filesz;               /* number of bytes in the file (may be zero or other value smaller than p_memsz) */
    uint32_t            p_memsz;                /* number of bytes when mapped (may be zero) */
    uint32_t            p_flags;
    uint32_t            p_align;                /* alignment for file and memory (0,1=>none); must be a power of two */
};
struct Elf64SegmentTableEntry_disk {
    uint32_t            p_type;
    uint32_t            p_flags;
    uint64_t            p_offset;
    uint64_t            p_vaddr;
    uint64_t            p_paddr;
    uint64_t            p_filesz;
    uint64_t            p_memsz;
    uint64_t            p_align;
};

class ElfSegmentTableEntry {
  public:
    ElfSegmentTableEntry(ByteOrder sex, const Elf32SegmentTableEntry_disk *disk)
        : extra(NULL), nextra(0) {ctor(sex, disk);}
    ElfSegmentTableEntry(ByteOrder sex, const Elf64SegmentTableEntry_disk *disk)
        : extra(NULL), nextra(0) {ctor(sex, disk);}

    /* These are the native-format versions of the same members described in Elf*SegmentTableEntry_disk */
    unsigned            p_type, p_flags;
    addr_t              p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_align;

    /* The ELF header can define a segment table entry to be larger than the Elf*SegmentTableEntry_disk struct, so any
     * extra data gets stuffed into this member, which is a pointer directly into the mapped file and is null if there
     * is no extra data. */
    const unsigned char *extra;
    addr_t              nextra;                 /*extra size in bytes*/

    /* The pointer to the actual segment on disk */
    ExecSegment         *segment;

  private:
    void ctor(ByteOrder sex, const Elf32SegmentTableEntry_disk *disk);
    void ctor(ByteOrder sex, const Elf64SegmentTableEntry_disk *disk);
};

/* The segment table is just a file section containing an array of segment table entries (i.e., array of Elf program headers) */
class ElfSegmentTable : public ExecSection {
  public:
    ElfSegmentTable(ElfFileHeader *fhdr)
        : ExecSection(fhdr->get_file(), fhdr->e_phoff, fhdr->e_phnum*fhdr->e_phentsize) {ctor(fhdr);}
private:
    void ctor(ElfFileHeader*);
    std::vector<ElfSegmentTableEntry*> entries;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Functions */
bool is_ELF(ExecFile*);
void parse(ExecFile*);
    
}; //namespace ELF
}; //namespace Exec

#endif /*!Exec_ExecELF_h*/
