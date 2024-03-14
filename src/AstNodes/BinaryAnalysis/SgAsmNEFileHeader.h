#include <Rose/BinaryAnalysis/Address.h>

#ifdef ROSE_IMPL
#include <SgAsmDOSExtendedHeader.h>
#include <SgAsmNEEntryTable.h>
#include <SgAsmNEModuleTable.h>
#include <SgAsmNENameTable.h>
#include <SgAsmNESectionTable.h>
#endif

class SgAsmNEFileHeader: public SgAsmGenericHeader {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /* File format of an NE File Header. All fields are little endian.
     *
     * NOTES
     *
     * e_sssp: The value specified in SS is an index (1-origin) into the segment table. If SS addresses the automatic data segment
     *         and SP is zero then SP is set to the address obtained by adding the size of the automatic data segment to the size
     *         of the stack. */
    struct NEFileHeader_disk {
         unsigned char e_magic[2];           /* 0x00 magic number "NE" */
         unsigned char e_linker_major;       /* 0x02 linker major version number */
         unsigned char e_linker_minor;       /* 0x03 linker minor version number */
         uint16_t    e_entrytab_rfo;         /* 0x04 entry table offset relative to start of header */
         uint16_t    e_entrytab_size;        /* 0x06 size of entry table in bytes */
         uint32_t    e_checksum;             /* 0x08 32-bit CRC of entire file (this word is taken a zero during the calculation) */
         uint16_t    e_flags1;               /* 0x0c file-level bit flags (see HeaderFlags1) */
         uint16_t    e_autodata_sn;          /* 0x0e auto data section number if (flags & 0x3)==0; else zero */
         uint16_t    e_bss_size;             /* 0x10 num bytes added to data segment for BSS */
         uint16_t    e_stack_size;           /* 0x12 num bytes added to data segment for stack (zero of SS!=DS registers) */
         uint32_t    e_csip;                 /* 0x14 section number:offset of CS:IP */
         uint32_t    e_sssp;                 /* 0x18 section number:offset of SS:SP (see note 1 above) */
         uint16_t    e_nsections;            /* 0x1c number of entries in the section table */
         uint16_t    e_nmodrefs;             /* 0x1e number of entries in the module reference table */
         uint16_t    e_nnonresnames;         /* 0x20 number of entries in the non-resident name table */
         uint16_t    e_sectab_rfo;           /* 0x22 offset of section table relative to start of header */
         uint16_t    e_rsrctab_rfo;          /* 0x24 offset of resource table relative to start of header */
         uint16_t    e_resnametab_rfo;       /* 0x26 offset of resident name table relative to start of header */
         uint16_t    e_modreftab_rfo;        /* 0x28 offset of module reference table relative to start of header */
         uint16_t    e_importnametab_rfo;    /* 0x2a offset of imported names table relative to start of header */
         uint32_t    e_nonresnametab_offset; /* 0x2c file offset of non-resident name table */
         uint16_t    e_nmovable_entries;     /* 0x30 number of movable entries in Entry Table */
         uint16_t    e_sector_align;         /* 0x32 sector alignment shift count (log2 of segment sector size) */
         uint16_t    e_nresources;           /* 0x34 number of resource entries */
         unsigned char e_exetype;            /* 0x36 executable type (2==windows) */
         unsigned char e_flags2;             /* 0x37 additional flags (see HeaderFlags2) */
         uint16_t    e_fastload_sector;      /* 0x38 sector offset to fast-load area (only for Windows) */
         uint16_t    e_fastload_nsectors;    /* 0x3a size of fast-load area in sectors (only for Windows) */
         uint16_t    e_res1;                 /* 0x3c reserved */
         uint16_t    e_winvers;              /* 0x3e expected version number for Windows (only for Windows) */
       }                                     /* 0x40 */
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

    /* Bit flags for the NE header 'e_flags' member.
     *
     * If HF_LIBRARY (bit 15) is set then the CS:IP registers point to an initialization procedure called with the value in the AX
     * register equal to the module handle. The initialization procedure must execute a far return to the caller. The resulting
     * value in AX is a status indicator (non-zero for success, zero for failure). */
    enum HeaderFlags1 {
         HF1_RESERVED         = 0x57f4,      /* Reserved bits */
         HF1_NO_DATA          = 0x0000,      /* (flags&0x03==0) => an exe not containing a data segment */
         HF1_SINGLE_DATA      = 0x0001,      /* Executable contains one data segment; set if file is a DLL */
         HF1_MULTIPLE_DATA    = 0x0002,      /* Exe with multiple data segments; set if a windows application */
         HF1_LOADER_SEGMENT   = 0x0800,      /* First segment contains code that loads the application */
         HF1_FATAL_ERRORS     = 0x2000,      /* Errors detected at link time; module will not load */
         HF1_LIBRARY          = 0x8000       /* Module is a library */
       };

    /* Bit flags for the NE header 'e_flags2' member. */
    enum HeaderFlags2 {
         HF2_RESERVED         = 0xf1,        /* Reserved bits */
         HF2_PROTECTED_MODE   = 0x02,        /* Windows 2.x application that runs in 3.x protected mode */
         HF2_PFONTS           = 0x04,        /* Windows 2.x application that supports proportional fonts */
         HF2_FASTLOAD         = 0x08         /* Executable contains a fast-load area */
       };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    [[using Rosebud: rosetta]]
    unsigned e_linker_major = 0;

    [[using Rosebud: rosetta]]
    unsigned e_linker_minor = 0;

    [[using Rosebud: rosetta]]
    unsigned e_checksum = 0;

    [[using Rosebud: rosetta]]
    unsigned e_flags1 = 0;

    [[using Rosebud: rosetta]]
    unsigned e_autodata_sn = 0;

    [[using Rosebud: rosetta]]
    unsigned e_bss_size = 0;

    [[using Rosebud: rosetta]]
    unsigned e_stack_size = 0;

    [[using Rosebud: rosetta]]
    unsigned e_csip = 0;

    [[using Rosebud: rosetta]]
    unsigned e_sssp = 0;

    [[using Rosebud: rosetta]]
    unsigned e_nsections = 0;

    [[using Rosebud: rosetta]]
    unsigned e_nmodrefs = 0;

    [[using Rosebud: rosetta]]
    unsigned e_nnonresnames = 0;

    [[using Rosebud: rosetta]]
    unsigned e_nmovable_entries = 0;

    [[using Rosebud: rosetta]]
    unsigned e_sector_align = 0;

    [[using Rosebud: rosetta]]
    unsigned e_nresources = 0;

    [[using Rosebud: rosetta]]
    unsigned e_exetype = 0;

    [[using Rosebud: rosetta]]
    unsigned e_flags2 = 0;

    [[using Rosebud: rosetta]]
    unsigned e_res1 = 0;

    [[using Rosebud: rosetta]]
    unsigned e_winvers = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_entrytab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_entrytab_size = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_sectab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_rsrctab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_resnametab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_modreftab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_importnametab_rfo = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_nonresnametab_offset = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_fastload_sector = 0;

    [[using Rosebud: rosetta]]
    rose_addr_t e_fastload_nsectors = 0;

    [[using Rosebud: rosetta, traverse]]
    SgAsmDOSExtendedHeader* dos2Header = nullptr;

    [[using Rosebud: rosetta]]
    SgAsmNESectionTable* sectionTable = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmNENameTable* residentNameTable = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmNENameTable* nonresidentNameTable = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmNEModuleTable* moduleTable = nullptr;

    [[using Rosebud: rosetta, traverse]]
    SgAsmNEEntryTable* entryTable = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmNEFileHeader(SgAsmGenericFile *f, rose_addr_t offset);

    static bool isNe (SgAsmGenericFile*);
    static SgAsmNEFileHeader *parse(SgAsmDOSFileHeader*);
    virtual void unparse(std::ostream&) const override;
    virtual const char *formatName() const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

private:
    void *encode(SgAsmNEFileHeader::NEFileHeader_disk*) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmDOSExtendedHeader* get_dos2_header() const ROSE_DEPRECATED("use get_dos2Header");
    void set_dos2_header(SgAsmDOSExtendedHeader*) ROSE_DEPRECATED("use set_dos2Header");
    SgAsmNESectionTable* get_section_table() const ROSE_DEPRECATED("use get_sectionTable");
    void set_section_table(SgAsmNESectionTable*) ROSE_DEPRECATED("use set_sectionTable");
    SgAsmNENameTable* get_resname_table() const ROSE_DEPRECATED("use get_residentNameTable");
    void set_resname_table(SgAsmNENameTable*) ROSE_DEPRECATED("use set_residentNameTable");
    SgAsmNENameTable* get_nonresname_table() const ROSE_DEPRECATED("use get_nonresidentNameTable");
    void set_nonresname_table(SgAsmNENameTable*) ROSE_DEPRECATED("use set_nonresidentNameTable");
    SgAsmNEModuleTable* get_module_table() const ROSE_DEPRECATED("use get_moduleTable");
    void set_module_table(SgAsmNEModuleTable*) ROSE_DEPRECATED("use set_moduleTable");
    SgAsmNEEntryTable* get_entry_table() const ROSE_DEPRECATED("use get_entryTable");
    void set_entry_table(SgAsmNEEntryTable*) ROSE_DEPRECATED("use set_entryTable");
    static bool is_NE (SgAsmGenericFile*) ROSE_DEPRECATED("use isNe");
    virtual const char *format_name() const override ROSE_DEPRECATED("use formatName");

};
