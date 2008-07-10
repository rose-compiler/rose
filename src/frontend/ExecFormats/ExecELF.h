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
} __attribute__((packed));
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
} __attribute__((packed));

class ElfFileHeader : public ExecHeader {
  public:
    ElfFileHeader(ExecFile *f, addr_t offset)   /* assume 32-bit for now and fix inside ctor() if necessary */
        : ExecHeader(f, offset, sizeof(Elf32FileHeader_disk)) {ctor(f, offset);}
    virtual ~ElfFileHeader() {}
    uint64_t max_page_size();
    virtual void unparse(FILE*);
    void *encode(ByteOrder, Elf32FileHeader_disk*);
    void *encode(ByteOrder, Elf64FileHeader_disk*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

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
// ELF Sections
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
} __attribute__((packed));
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
} __attribute__((packed));

class ElfSectionTableEntry {
  public:
    ElfSectionTableEntry(ByteOrder sex, const Elf32SectionTableEntry_disk *disk)
        : extra(NULL), nextra(0) {ctor(sex, disk);}
    ElfSectionTableEntry(ByteOrder sex, const Elf64SectionTableEntry_disk *disk)
        : extra(NULL), nextra(0) {ctor(sex, disk);};
    virtual ~ElfSectionTableEntry() {};
    void *encode(ByteOrder sex, Elf32SectionTableEntry_disk *disk);
    void *encode(ByteOrder sex, Elf64SectionTableEntry_disk *disk);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* These are the native-format versions of the same members described in Elf*SectionTableEntry_disk */
    unsigned            sh_name, sh_type, sh_link, sh_info;
    uint64_t            sh_flags;
    addr_t              sh_addr, sh_offset, sh_size, sh_addralign, sh_entsize;

    /* The ELF header can define a section table entry to be larger than the Elf*SectionTableEntry_disk struct, so any
     * extra data gets stuffed into this member, which is a pointer directly into the mapped file and is null if there
     * is no extra data. */
    const unsigned char *extra;
    addr_t              nextra;                 /*extra size in bytes*/

  private:
    void ctor(ByteOrder sex, const Elf32SectionTableEntry_disk *disk);
    void ctor(ByteOrder sex, const Elf64SectionTableEntry_disk *disk);
};

/* An ELF (non-synthesized) section */
class ElfSection : public ExecSection {
  public:
    /* Constructor for size based on section table entry (normal sections) */
    ElfSection(ElfFileHeader *fhdr, ElfSectionTableEntry *shdr)
        : ExecSection(fhdr->get_file(), shdr->sh_offset, shdr->sh_size),
        linked_section(NULL), st_entry(NULL)
        {ctor(fhdr, shdr);}
    /* Constructor for explicit size (e.g., BSS sections where file size is always zero) */
    ElfSection(ElfFileHeader *fhdr, ElfSectionTableEntry *shdr, addr_t file_size)
        : ExecSection(fhdr->get_file(), shdr->sh_offset, file_size),
        linked_section(NULL), st_entry(NULL)
        {ctor(fhdr, shdr);}
    virtual ~ElfSection() {}
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors for protected/private data members */
    ElfSection *get_linked_section() {return linked_section;}
    virtual void set_linked_section(ElfSection *sec) {linked_section=sec;}
    ElfSectionTableEntry *get_st_entry() {return st_entry;}
    void set_st_entry(ElfSectionTableEntry *e) {st_entry=e;}

    /* Convenience functions */
    ElfFileHeader *get_elf_header() {return dynamic_cast<ElfFileHeader*>(get_header());}

  private:
    void ctor(ElfFileHeader*, ElfSectionTableEntry*);
    ElfSection *linked_section;
    ElfSectionTableEntry *st_entry;
};

/* The section table is just a synthesized file section containing an array of section table entries (i.e., array of section
 * headers). The section table entry info parsed from the file is stored with its corresponding section rather than here. We
 * can reconstruct the section table since sections have unique ID numbers that are their original indices in the section table
 * entry array. */
class ElfSectionTable : public ExecSection {
  public:
    ElfSectionTable(ElfFileHeader *fhdr)
        : ExecSection(fhdr->get_file(), fhdr->e_shoff, fhdr->e_shnum*fhdr->e_shentsize)
        {ctor(fhdr);}
    virtual ~ElfSectionTable() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
  private:
    void ctor(ElfFileHeader *fhdr);    
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
} __attribute__((packed));
struct Elf64SegmentTableEntry_disk {
    uint32_t            p_type;
    uint32_t            p_flags;
    uint64_t            p_offset;
    uint64_t            p_vaddr;
    uint64_t            p_paddr;
    uint64_t            p_filesz;
    uint64_t            p_memsz;
    uint64_t            p_align;
} __attribute__((packed));

class ElfSegmentTableEntry {
  public:
    ElfSegmentTableEntry(ByteOrder sex, const Elf32SegmentTableEntry_disk *disk)
        : extra(NULL), nextra(0) {ctor(sex, disk);}
    ElfSegmentTableEntry(ByteOrder sex, const Elf64SegmentTableEntry_disk *disk)
        : extra(NULL), nextra(0) {ctor(sex, disk);}
    virtual ~ElfSegmentTableEntry() {};
    void *encode(ByteOrder, Elf32SegmentTableEntry_disk*);
    void *encode(ByteOrder, Elf64SegmentTableEntry_disk*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* These are the native-format versions of the same members described in Elf*SegmentTableEntry_disk */
    unsigned            p_type, p_flags;
    addr_t              p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_align;

    /* The ELF header can define a segment table entry to be larger than the Elf*SegmentTableEntry_disk struct, so any
     * extra data gets stuffed into this member, which is a pointer directly into the mapped file and is null if there
     * is no extra data. */
    const unsigned char *extra;
    addr_t              nextra;                 /*extra size in bytes*/

  private:
    void ctor(ByteOrder sex, const Elf32SegmentTableEntry_disk *disk);
    void ctor(ByteOrder sex, const Elf64SegmentTableEntry_disk *disk);
};

/* The segment table is a synthesized file section containing an array of segment table entries (i.e., array of ELF
 * program headers) */
class ElfSegmentTable : public ExecSection {
  public:
    ElfSegmentTable(ElfFileHeader *fhdr)
        : ExecSection(fhdr->get_file(), fhdr->e_phoff, fhdr->e_phnum*fhdr->e_phentsize)
        {ctor(fhdr);}
    virtual ~ElfSegmentTable() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
  private:
    void ctor(ElfFileHeader*);
    std::vector<ElfSegmentTableEntry*> entries;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Dynamic Linking Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Elf32DynamicEntry_disk {
    uint32_t            d_tag;                  /* Entry type, one of the DT_* constants */
    uint32_t            d_val;                  /* Tag's value */
} __attribute__((packed));

struct Elf64DynamicEntry_disk {
    uint64_t            d_tag;                  /* Entry type, one of the DT_* constants */
    uint64_t            d_val;                  /* Tag's value */
} __attribute__((packed));

class ElfDynamicEntry {
  public:
    ElfDynamicEntry(ByteOrder sex, const Elf32DynamicEntry_disk *disk) {ctor(sex, disk);}
    ElfDynamicEntry(ByteOrder sex, const Elf64DynamicEntry_disk *disk) {ctor(sex, disk);}
    virtual ~ElfDynamicEntry() {}
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    void *encode(ByteOrder, Elf32DynamicEntry_disk*);
    void *encode(ByteOrder, Elf64DynamicEntry_disk*);
    
    unsigned            d_tag;
    addr_t              d_val;
  private:
    void ctor(ByteOrder sex, const Elf32DynamicEntry_disk *disk);
    void ctor(ByteOrder sex, const Elf64DynamicEntry_disk *disk);
};

class ElfDynamicSection : public ElfSection {
  public:
    ElfDynamicSection(ElfFileHeader *fhdr, ElfSectionTableEntry *shdr)
        : ElfSection(fhdr, shdr),
        dt_pltrelsz(0), dt_pltgot(0), dt_hash(0), dt_strtab(0), dt_symtab(0), dt_rela(0), dt_relasz(0), dt_relaent(0),
        dt_strsz(0), dt_symentsz(0), dt_init(0), dt_fini(0), dt_pltrel(0), dt_jmprel(0),
        dt_verneednum(0), dt_verneed(0), dt_versym(0)
        {}
    virtual ~ElfDynamicSection() {}
    virtual void set_linked_section(ElfSection *sec);   /* Parsing happens here rather than in constructor */
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
  private:
    unsigned            dt_pltrelsz;                    /* Size in bytes of PLT relocations */
    addr_t              dt_pltgot;                      /* Address of global offset table */
    addr_t              dt_hash;                        /* Address of symbol hash table */
    addr_t              dt_strtab;                      /* Address of dynamic string table */
    addr_t              dt_symtab;                      /* Address of symbol table */
    addr_t              dt_rela;                        /* Address of Rela relocations */
    unsigned            dt_relasz;                      /* Total size in bytes of Rela relocations */
    unsigned            dt_relaent;                     /* Size of one Rela relocation */
    unsigned            dt_strsz;                       /* Size in bytes of string table */
    unsigned            dt_symentsz;                    /* Size in bytes of one symbol table entry */
    addr_t              dt_init;                        /* Address of initialization function */
    addr_t              dt_fini;                        /* Address of termination function */
    unsigned            dt_pltrel;                      /* Type of relocation in PLT */
    addr_t              dt_jmprel;                      /* Address of PLT relocations */
    unsigned            dt_verneednum;                  /* Number of entries in dt_verneed table */
    addr_t              dt_verneed;                     /* Address of table with needed versions */
    addr_t              dt_versym;                      /* GNU version symbol address */
    std::vector<ElfDynamicEntry*> other_entries;        /* Other values not specifically parsed out */
    std::vector<ElfDynamicEntry*> all_entries;          /* All parsed entries in order of appearance */
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Symbol Tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum ElfSymBinding {
    STB_LOCAL=0,
    STB_GLOBAL=1,
    STB_WEAK=2
};

enum ElfSymType {
    STT_NOTYPE  = 0,
    STT_OBJECT  = 1,
    STT_FUNC    = 2,
    STT_SECTION = 3,
    STT_FILE    = 4
};

struct Elf32SymbolEntry_disk {
    uint32_t            st_name;                        /* Name offset into string table */
    uint32_t            st_value;                       /* Value: absolute value, address, etc. depending on sym type */
    uint32_t            st_size;                        /* Symbol size in bytes */
    unsigned char       st_info;                        /* Type and binding attributes */
    unsigned char       st_res1;                        /* Reserved; always zero */
    uint16_t            st_shndx;                       /* Section index or special meaning */
} __attribute__((packed));

struct Elf64SymbolEntry_disk {
    uint32_t            st_name;
    unsigned char       st_info;
    unsigned char       st_res1;
    uint16_t            st_shndx;
    uint64_t            st_value;
    uint64_t            st_size;
} __attribute__((packed));

class ElfSymbol : public ExecSymbol {
  public:
    ElfSymbol(ByteOrder sex, const Elf32SymbolEntry_disk *disk)
        {ctor(sex, disk);}
    ElfSymbol(ByteOrder sex, const Elf64SymbolEntry_disk *disk)
        {ctor(sex, disk);}
    virtual ~ElfSymbol() {};
    void *encode(ByteOrder, Elf32SymbolEntry_disk*);
    void *encode(ByteOrder, Elf64SymbolEntry_disk*);
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) {dump(f, prefix, idx, NULL);}
    void dump(FILE*, const char *prefix, ssize_t idx, ExecSection*);
    ElfSymBinding get_elf_binding() {return (ElfSymBinding)(st_info>>4);}
    ElfSymType get_elf_type() {return (ElfSymType)(st_info & 0xf);}
        
    /* Members defined by the ELF standard */
    addr_t              st_name;
    unsigned char       st_info, st_res1;
    unsigned            st_shndx;
    addr_t              st_size; /*original size for unparsing; superclass holds adjusted size*/

  private:
    void ctor(ByteOrder, const Elf32SymbolEntry_disk*);
    void ctor(ByteOrder, const Elf64SymbolEntry_disk*);
    void ctor_common();                                 /* Initialization common to all constructors */
};

class ElfSymbolSection : public ElfSection {
  public:
    ElfSymbolSection(ElfFileHeader *fhdr, ElfSectionTableEntry *shdr)
        : ElfSection(fhdr, shdr)
        {ctor(shdr);}
    virtual ~ElfSymbolSection() {}
    virtual void set_linked_section(ElfSection *strtab);
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors for protected/private data members */
    std::vector<ElfSymbol*>& get_symbols() {return symbols;}
    
  private:
    void ctor(ElfSectionTableEntry*);
    std::vector<ElfSymbol*> symbols;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Functions */
bool is_ELF(ExecFile*);
void parseBinaryFormat(ExecFile*, SgAsmFile* asmFile);
ElfFileHeader *parse(ExecFile*);

}; //namespace ELF
}; //namespace Exec

#endif /*!Exec_ExecELF_h*/
