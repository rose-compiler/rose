#include <Rose/BinaryAnalysis/ByteOrder.h>

#ifdef ROSE_IMPL
#include <SgAsmDOSExtendedHeader.h>
#include <SgAsmLEEntryTable.h>
#include <SgAsmLENameTable.h>
#include <SgAsmLEPageTable.h>
#include <SgAsmLERelocTable.h>
#include <SgAsmLESectionTable.h>
#endif

class SgAsmLEFileHeader: public SgAsmGenericHeader {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
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
    }                                       /* 0xac */
// DQ (3/7/2013): Adding support to restrict visability to SWIG.
#ifndef SWIG
#ifndef _MSC_VER
          __attribute__((packed))
#endif
#endif
          ;
#ifdef _MSC_VER
# pragma pack ()
#endif

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
        HF_MODTYPE_VDEV     = 0x00028000    /* Virtual device driver module */
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    [[using Rosebud: rosetta]]
    unsigned e_byte_order = 0;

    [[using Rosebud: rosetta]]
    unsigned e_word_order = 0;

    [[using Rosebud: rosetta]]
    unsigned e_format_level = 0;

    [[using Rosebud: rosetta]]
    unsigned e_cpu_type = 0;

    [[using Rosebud: rosetta]]
    unsigned e_os_type = 0;

    [[using Rosebud: rosetta]]
    unsigned e_module_version = 0;

    [[using Rosebud: rosetta]]
    unsigned e_flags = 0;

    [[using Rosebud: rosetta]]
    unsigned e_eip_section = 0;

    [[using Rosebud: rosetta]]
    unsigned e_esp_section = 0;

    [[using Rosebud: rosetta]]
    unsigned e_last_page_size = 0;

    [[using Rosebud: rosetta]]
    unsigned e_page_offset_shift = 0;

    [[using Rosebud: rosetta]]
    unsigned e_fixup_sect_cksum = 0;

    [[using Rosebud: rosetta]]
    unsigned e_loader_sect_cksum = 0;

    [[using Rosebud: rosetta]]
    unsigned e_secttab_nentries = 0;

    [[using Rosebud: rosetta]]
    unsigned e_rsrctab_nentries = 0;

    [[using Rosebud: rosetta]]
    unsigned e_fmtdirtab_nentries = 0;

    [[using Rosebud: rosetta]]
    unsigned e_import_modtab_nentries = 0;

    [[using Rosebud: rosetta]]
    unsigned e_preload_npages = 0;

    [[using Rosebud: rosetta]]
    unsigned e_nonresnametab_size = 0;

    [[using Rosebud: rosetta]]
    unsigned e_nonresnametab_cksum = 0;

    [[using Rosebud: rosetta]]
    unsigned e_auto_ds_section = 0;

    [[using Rosebud: rosetta]]
    unsigned e_debug_info_size = 0;

    [[using Rosebud: rosetta]]
    unsigned e_num_instance_preload = 0;

    [[using Rosebud: rosetta]]
    unsigned e_num_instance_demand = 0;

    [[using Rosebud: rosetta]]
    unsigned e_heap_size = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_npages = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_eip = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_esp = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_page_size = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_fixup_sect_size = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_loader_sect_size = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_secttab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_pagetab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_iterpages_offset = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_rsrctab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_resnametab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_entrytab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_fmtdirtab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_fixup_pagetab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_fixup_rectab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_import_modtab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_import_proctab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_ppcksumtab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_data_pages_offset = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_nonresnametab_offset = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_debug_info_rfo = 0;

    [[using Rosebud: rosetta, traverse]]
    SgAsmDOSExtendedHeader* dos2Header = nullptr;

    [[using Rosebud: rosetta]]
    SgAsmLESectionTable* sectionTable = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmLEPageTable* pageTable = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmLENameTable* residentNameTable = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmLENameTable* nonresidentNameTable = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmLEEntryTable* entryTable = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmLERelocTable* relocationTable = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmLEFileHeader(SgAsmGenericFile *f, rose_addr_t offset);
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    // Overloaded base class virtual function
    const char *formatName() const override;

    static bool isLe (SgAsmGenericFile*);
    static SgAsmLEFileHeader *parse(SgAsmDOSFileHeader*);

private:
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness sex, SgAsmLEFileHeader::LEFileHeader_disk*) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmDOSExtendedHeader* get_dos2_header() const ROSE_DEPRECATED("use get_dos2Header");
    void set_dos2_header(SgAsmDOSExtendedHeader*) ROSE_DEPRECATED("use set_dos2Header");
    SgAsmLESectionTable* get_section_table() const ROSE_DEPRECATED("use get_sectionTable");
    void set_section_table(SgAsmLESectionTable*) ROSE_DEPRECATED("use set_sectionTable");
    SgAsmLEPageTable* get_page_table() const ROSE_DEPRECATED("use get_pageTable");
    void set_page_table(SgAsmLEPageTable*) ROSE_DEPRECATED("use set_pageTable");
    SgAsmLENameTable* get_resname_table() const ROSE_DEPRECATED("use get_residentNameTable");
    void set_resname_table(SgAsmLENameTable*) ROSE_DEPRECATED("use set_residentNameTable");
    SgAsmLENameTable* get_nonresname_table() const ROSE_DEPRECATED("use get_nonresidentNameTable");
    void set_nonresname_table(SgAsmLENameTable*) ROSE_DEPRECATED("use set_nonresidentNameTable");
    SgAsmLEEntryTable* get_entry_table() const ROSE_DEPRECATED("use get_entryTable");
    void set_entry_table(SgAsmLEEntryTable*) ROSE_DEPRECATED("use set_entryTable");
    SgAsmLERelocTable* get_reloc_table() const ROSE_DEPRECATED("use get_relocationTable");
    void set_reloc_table(SgAsmLERelocTable*) ROSE_DEPRECATED("use set_relocationTable");
    const char *format_name() const override ROSE_DEPRECATED("use formatName");
    static bool is_LE (SgAsmGenericFile*) ROSE_DEPRECATED("use isLe");
};
