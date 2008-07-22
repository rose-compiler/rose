/* Copyright 2008 Lawrence Livermore National Security, LLC */
/* NOTE: OS/2 LX files are an extension of Microsoft LE (VxD) files and are handled by these same "LE" classes */
#ifndef Exec_ExecLE_h
#define Exec_ExecLE_h

#include "ExecDOS.h"
#include "ExecNE.h"

namespace Exec {
namespace LE {

/* Forwards */
class LEFileHeader;
class LESectionTable;
class LEPageTable;
class LENameTable;
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtendedDOSHeader -- extra components of the DOS header when used in an LE/LX file
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* The extended DOS header for LE/LX is the same as for NE */
typedef NE::ExtendedDOSHeader ExtendedDOSHeader;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* File format of an LE/LX File Header (they are identical except bytes 0x2c-0x2f) */
struct LEFileHeader_disk {
    unsigned char e_magic[2];           /* 0x00 magic number "LX" */
    unsigned char e_byte_order;         /* 0x02 byte order (0=>little endian; otherwise big endian) */
    unsigned char e_word_order;         /* 0x03 word order (0=>little endian; otherwise big endian) */
    uint32_t    e_format_level;         /* 0x04 LX file format version number */
    uint16_t    e_cpu_type;             /* 0x08 1=>80286, 2=>80386, 3=>80486,4=80586, etc. (see ctor) */
    uint16_t    e_os_type;              /* 0x0a 0=>unknown, 1=>0S/2, 2=>Windows, 3=>DOS 4.x, 4=>Windows 386 */
    uint32_t    e_module_version;       /* 0x0c user-specified module version number */
    uint32_t    e_flags;                /* 0x10 bit flags (see LXFileHeaderFlags) */
    uint32_t    e_npages;               /* 0x14 number of physically contained pages (see e_page_size) */
    uint32_t    e_eip_section;          /* 0x18 the section number to which e_eip is relative */
    uint32_t    e_eip;                  /* 0x1c entry address relative to e_eip_section */
    uint32_t    e_esp_section;          /* 0x20 the section number to which e_esp is relative */
    uint32_t    e_esp;                  /* 0x24 starting stack address relative to e_esp_section */
    uint32_t    e_page_size;            /* 0x28 page size in bytes */
    uint32_t    e_lps_or_shift;         /* 0x2c size of last page (LE) or shift for page table's page offset field (LX) */
    uint32_t    e_fixup_sect_size;      /* 0x30 total size of fixup info in bytes (fixup page/record tables + import names) */
    uint32_t    e_fixup_sect_cksum;     /* 0x34 cryptographic checksum of all fixup info, or zero */
    uint32_t    e_loader_sect_size;     /* 0x38 size of memory resident tables (section table through per-page checksum table) */
    uint32_t    e_loader_sect_cksum;    /* 0x3c cryptographic checksum for all loader info, or zero */
    uint32_t    e_secttab_rfo;          /* 0x40 offset of section table relative to this header */
    uint32_t    e_secttab_nentries;     /* 0x44 number of entries in section table */
    uint32_t    e_pagetab_rfo;          /* 0x48 section page table offset relative to this header */
    uint32_t    e_iterpages_offset;     /* 0x4c section iterated pages offset (absolute file offset) */
    uint32_t    e_rsrctab_rfo;          /* 0x50 offset of resource table relative to this header */
    uint32_t    e_rsrctab_nentries;     /* 0x54 number of entries in the resource table */
    uint32_t    e_resnametab_rfo;       /* 0x58 offset of resident name table relative to this header */
    uint32_t    e_entrytab_rfo;         /* 0x5c offset of entry table relative to this header */
    uint32_t    e_fmtdirtab_rfo;        /* 0x60 offset of module format directives relative to this header */
    uint32_t    e_fmtdirtab_nentries;   /* 0x64 number of entries in module format directives table */
    uint32_t    e_fixup_pagetab_rfo;    /* 0x68 offset of fixup page table relative to this header */
    uint32_t    e_fixup_rectab_rfo;     /* 0x6c offset of fixup record table relative to this header */
    uint32_t    e_import_modtab_rfo;    /* 0x70 offset of import module name table relative to this header */
    uint32_t    e_import_modtab_nentries;/*0x74 number of entries in import module name table */
    uint32_t    e_import_proctab_rfo;   /* 0x78 offset of import procedure name table relative to this header */
    uint32_t    e_ppcksumtab_rfo;       /* 0x7c offset of per-page checksum table relative to this header */
    uint32_t    e_data_pages_offset;    /* 0x80 offset of data pages (absolute file offset) */
    uint32_t    e_preload_npages;       /* 0x84 number of preload pages (not respected by OS/2) */
    uint32_t    e_nonresnametab_offset; /* 0x88 offset of non-resident name table (absolute file offset) */
    uint32_t    e_nonresnametab_size;   /* 0x8c size of non-resident name table in bytes */
    uint32_t    e_nonresnametab_cksum;  /* 0x90 cryptographic checksum of the non-resident name table */
    uint32_t    e_auto_ds_section;      /* 0x94 auto data segment section number (not used by 32-bit modules) */
    uint32_t    e_debug_info_rfo;       /* 0x98 offset of debug information relative to this header */
    uint32_t    e_debug_info_size;      /* 0x9c size of debug information in bytes */
    uint32_t    e_num_instance_preload; /* 0xa0 number of instance data pages found in the preload section */
    uint32_t    e_num_instance_demand;  /* 0xa4 number of instance data pages found in the demand section */
    uint32_t    e_heap_size;            /* 0xa8 number of bytes added to auto data segment by loader (not used by 32-bit) */
} __attribute__((packed));              /* 0xac */

enum LEFileHeaderFlags {
    HF_RESERVED         = 0xbffc5ccb,   /* Reserved bits */
    HF_PROC_LIB_INIT    = 0x00000004,   /* Per-process library initialization; not used for executables */
    HF_IFIXUPS_APPLIED  = 0x00000010,   /* Sections have preferred load addresses and internal relocs have been applied */
    HF_EFIXUPS_APPLIED  = 0x00000020,   /* External fixups for the module have been applied */
    HF_PM_WINDOW_NO     = 0x00000100,   /* Incompatible with PM windowing */
    HF_PM_WINDOW_OK     = 0x00000200,   /* Compatible with PM windowing */
    HF_PM_WINDOW_USE    = 0x00000300,   /* Uses PM windowing API */
    HF_NOT_LOADABLE     = 0x00002000,   /* Module is not loadable (has errors or incrementally linked) */
    HF_PROC_LIB_TERM    = 0x40000000,   /* Per-process library termination; not used for executables */
    
    HF_MODTYPE_MASK     = 0x00038000,   /* Module type mask */
    HF_MODTYPE_PROG     = 0x00000000,   /* Program module (other modules cannot link to this one) */
    HF_MODTYPE_LIB      = 0x00008000,   /* Library module */
    HF_MODTYPE_PLIB     = 0x00018000,   /* Protected memory library module */
    HF_MODTYPE_PDEV     = 0x00020000,   /* Physical device driver module */
    HF_MODTYPE_VDEV     = 0x00028000,   /* Virtual device driver module */
};

class LEFileHeader : public ExecHeader {
  public:
    LEFileHeader(ExecFile *f, addr_t offset)
        : ExecHeader(f, offset, sizeof(LEFileHeader_disk)),
        dos2_header(NULL), section_table(NULL), page_table(NULL), resname_table(NULL)
        {ctor(f, offset);}
    virtual ~LEFileHeader() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    virtual const char *format_name();

    /* Accessors for protected/private data members */
    ExtendedDOSHeader *get_dos2_header() {return dos2_header;}
    void set_dos2_header(ExtendedDOSHeader *h) {dos2_header=h;}
    LESectionTable *get_section_table() {return section_table;}
    void set_section_table(LESectionTable *t) {section_table=t;}
    LEPageTable *get_page_table() {return page_table;}
    void set_page_table(LEPageTable *t) {page_table=t;}
    LENameTable *get_resname_table() {return resname_table;}
    void set_resname_table(LENameTable *t) {resname_table=t;}
    
    /* These are the native-format versions of the same members described in the NEFileHeader_disk format struct. */
    unsigned e_byte_order, e_word_order, e_format_level, e_cpu_type, e_os_type, e_module_version, e_flags;
    unsigned e_eip_section, e_esp_section, e_last_page_size, e_page_offset_shift, e_fixup_sect_cksum, e_loader_sect_cksum;
    unsigned e_secttab_nentries, e_rsrctab_nentries, e_fmtdirtab_nentries, e_import_modtab_nentries, e_preload_npages;
    unsigned e_nonresnametab_size, e_nonresnametab_cksum, e_auto_ds_section, e_debug_info_size, e_num_instance_preload;
    unsigned e_num_instance_demand, e_heap_size;
    
    addr_t e_npages, e_eip, e_esp, e_page_size, e_fixup_sect_size, e_loader_sect_size, e_secttab_rfo, e_pagetab_rfo;
    addr_t e_iterpages_offset, e_rsrctab_rfo, e_resnametab_rfo, e_entrytab_rfo, e_fmtdirtab_rfo, e_fixup_pagetab_rfo;
    addr_t e_fixup_rectab_rfo, e_import_modtab_rfo, e_import_proctab_rfo, e_ppcksumtab_rfo, e_data_pages_offset;
    addr_t e_nonresnametab_offset, e_debug_info_rfo;

  private:
    void ctor(ExecFile *f, addr_t offset);
    void *encode(ByteOrder sex, LEFileHeader_disk*);
    ExtendedDOSHeader *dos2_header;
    LESectionTable *section_table;
    LEPageTable *page_table;
    LENameTable *resname_table;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Section (Object) Page Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* The object page table provides information about a logical page in a section. A logical page may be an enumerated page, a
 * pseudo page, or an iterated page. The page table allows for efficient access to a page when a page fault occurs, while
 * still allowing the physical page to be located in the preload page, demand load page, or iterated data page sections of the
 * executable file. Entries in the page table use 1-origin indices.  This table is parallel with the Fixup Page Table (they
 * are both indexed by the logical page number). */

/* File format for a page table entry */
struct LEPageTableEntry_disk {
    uint16_t            pageno_hi;
    unsigned char       pageno_lo;
    unsigned char       flags;
} __attribute__((packed));

class LEPageTableEntry {
  public:
    LEPageTableEntry(ByteOrder sex, const LEPageTableEntry_disk *disk)
        : pageno(0), flags(0)
        {ctor(sex, disk);}
    void dump(FILE*, const char *prefix, ssize_t idx);
    void *encode(ByteOrder, LEPageTableEntry_disk*);

    unsigned get_pageno() {return pageno;}
  private:
    void ctor(ByteOrder, const LEPageTableEntry_disk*);
    unsigned    pageno;
    unsigned    flags;
};

class LEPageTable : public ExecSection {
  public:
    LEPageTable(LEFileHeader *fhdr, addr_t offset, addr_t size)
        : ExecSection(fhdr->get_file(), offset, size)
        {ctor(fhdr);}
    virtual ~LEPageTable() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    LEPageTableEntry *get_page(size_t idx);
  private:
    void ctor(LEFileHeader*);
    std::vector<LEPageTableEntry*> entries;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LE/LX Section (Object) Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* File format of a section table entry. Fields are big- or little-endian depending on file header. */
struct LESectionTableEntry_disk {
    uint32_t    mapped_size;            /* 0x00 virtual segment size in bytes */
    uint32_t    base_addr;              /* 0x04 relocation base address */
    uint32_t    flags;                  /* 0x08 bit flags, see LESectionFlags */
    uint32_t    pagemap_index;          /* 0x0c */
    uint32_t    pagemap_nentries;       /* 0x10 number of entries in the page map */
    uint32_t    res1;                   /* 0x14 reserved */
} __attribute__((packed));              /* 0x18 */

/* SF_BIG_BIT: The "big/default" bit, for data segments, controls the setting of the Big bit in the segment descriptor. (The
 *             Big bit, or B-bit, determines whether ESP or SP is used as the stack pointer.) For code segments, this bit
 *             controls the setting of the Default bit in the segment descriptor. (The Default bit, or D-bit, determines
 *             whether the default word size is 32-bits or 16-bits. It also affects the interpretation of the instruction
 *             stream.) */
enum LESectionFlags {
    SF_RESERVED         = 0xffff0800,   /* Reserved bits (FIXME) */
    
    SF_READABLE         = 0x00000001,   /* Read permission granted when mapped */
    SF_WRITABLE         = 0x00000002,   /* Write permission granted when mapped */
    SF_EXECUTABLE       = 0x00000004,   /* Execute permission granted when mapped */
    
    SF_RESOURCE         = 0x00000008,   /* Section contains resource objects */
    SF_DISCARDABLE      = 0x00000010,   /* Discardable section */
    SF_SHARED           = 0x00000020,   /* Section is shared */
    SF_PRELOAD_PAGES    = 0x00000040,   /* Section has preload pages */
    SF_INVALID_PAGES    = 0x00000080,   /* Section has invalid pages */

    SF_TYPE_MASK        = 0x00000300,
    SF_TYPE_NORMAL      = 0x00000000,
    SF_TYPE_ZERO        = 0x00000100,   /* Section has zero-filled pages */
    SF_TYPE_RESIDENT    = 0x00000200,   /* Section is resident (valid for VDDs and PDDs only) */
    SF_TYPE_RESCONT     = 0x00000300,   /* Section is resident and contiguous */
    
    SF_RES_LONG_LOCK    = 0x00000400,   /* Section is resident and "long-lockable" (VDDs and PDDs only) */
    SF_1616_ALIAS       = 0x00001000,   /* 16:16 alias required (80x86 specific) */
    SF_BIG_BIT          = 0x00002000,   /* Big/default bit setting (80x86 specific); see note above */
    SF_CODE_CONFORM     = 0x00004000,   /* Section is conforming for code (80x86 specific) */
    SF_IO_PRIV          = 0x00008000,   /* Section I/O privilege level (80x86 specific; used only for 16:16 alias objects) */
};

class LESectionTableEntry {
  public:
    LESectionTableEntry(ByteOrder sex, const LESectionTableEntry_disk *disk)
        {ctor(sex, disk);}
    virtual ~LESectionTableEntry() {};
    void *encode(ByteOrder, LESectionTableEntry_disk*);
    virtual void dump(FILE *f, const char *prefix, ssize_t idx);
    
    /* These are the native-format versions of the same members described in the NESectionTableEntry_disk struct. */
    unsigned    flags, pagemap_index, pagemap_nentries, res1;
    addr_t      mapped_size, base_addr;

  private:
    void ctor(ByteOrder, const LESectionTableEntry_disk*);
};

/* Non-synthesized LE/LX sections (i.e., present in the section table) */
class LESection : public ExecSection {
  public:
    LESection(ExecFile *ef, addr_t offset, addr_t size)
        : ExecSection(ef, offset, size),
        st_entry(NULL)
        {}
    virtual ~LESection() {}
    //virtual void unparse(FILE*); /*nothing special to do*/
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors for protected/private data */
    LESectionTableEntry *get_st_entry() {return st_entry;}
    void set_st_entry(LESectionTableEntry *e) {st_entry=e;}

  private:
    LESectionTableEntry *st_entry;
};

/* The table entries are stored in the sections themselves. */
class LESectionTable : public ExecSection {
  public:
    LESectionTable(LEFileHeader *fhdr, addr_t offset, addr_t size)
        : ExecSection(fhdr->get_file(), offset, size)
        {ctor(fhdr);}
    virtual ~LESectionTable() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
  private:
    void ctor(LEFileHeader*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resident and Non-Resident Name Tables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* This table contains a module name followed by the list of exported function names. Each name is associated with an "ordinal"
 * which serves as an index into the Entry Table. The ordinal for the first string (module name) is meaningless and should be
 * zero. In the non-resident name table the first entry is a module description and the functions are not always resident in
 * system memory (they are discardable). */
class LENameTable : public ExecSection {
  public:
    LENameTable(LEFileHeader *fhdr, addr_t offset)
        : ExecSection(fhdr->get_file(), offset, 0)
        {ctor(fhdr);}
    virtual ~LENameTable() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
  private:
    void ctor(LEFileHeader*);
    std::vector<std::string> names; /*first name is module name; remainder are symbols within the module*/
    std::vector<unsigned> ordinals; /*first entry is ignored but present in file*/
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Functions */
bool is_LE(ExecFile*);
void parseBinaryFormat(ExecFile*, SgAsmFile* asmFile);
LEFileHeader *parse(ExecFile*);

}; //namespace LE
}; //namespace Exec

#endif /*!Exec_ExecLE_h*/
