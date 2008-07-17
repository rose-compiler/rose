/* Copyright 2008 Lawrence Livermore National Security, LLC */
#ifndef Exec_ExecNE_h
#define Exec_ExecNE_h

#include "ExecDOS.h"

namespace Exec {
namespace NE {

/* Forwards */
class NEFileHeader;
class NESectionTable;
class NEResNameTable;
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtendedDOSHeader -- extra components of the DOS header when used in an NE file
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ExtendedDOSHeader_disk {
    uint16_t    e_res1[14];             /* reserved */
    uint32_t    e_nehdr_offset;         /* file offset for NE header */
} __attribute__((packed));

class ExtendedDOSHeader : public ExecSection {
  public:
    ExtendedDOSHeader(ExecFile *f, addr_t offset)
        : ExecSection(f, offset, sizeof(ExtendedDOSHeader_disk))
        {ctor(f, offset);}
    virtual ~ExtendedDOSHeader() {};
    void *encode(ExtendedDOSHeader_disk*);
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* These are the native-format versions of the same members described in the ExtendedDOSHeader_disk struct. */
    unsigned e_res1[14];
    addr_t e_nehdr_offset;

  private:
    void ctor(ExecFile *f, addr_t offset);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* File format of an NE File Header. All fields are little endian. */
struct NEFileHeader_disk {
    unsigned char e_magic[2];           /* 0x00 magic number "NE" */
    unsigned char e_linker_major;       /* 0x02 linker major version number */
    unsigned char e_linker_minor;       /* 0x03 linker minor version number */
    uint16_t    e_entry_table_rfo;      /* 0x04 entry table offset relative to start of header */
    uint16_t    e_entry_table_size;     /* 0x06 size of entry table in bytes */
    uint32_t    e_checksum;             /* 0x08 32-bit CRC of entire file (this word is taken a zero during the calculation) */
    uint16_t    e_flags;                /* 0x0c file-level bit flags */
    uint16_t    e_autodata_sn;          /* 0x0e auto data section number if (flags & 0x3)==0; else zero */
    uint16_t    e_bss_size;             /* 0x10 num bytes added to data segment for BSS */
    uint16_t    e_stack_size;           /* 0x12 num bytes added to data segment for stack */
    uint32_t    e_csip;                 /* 0x14 section number:offset of CS:IP */
    uint32_t    e_sssp;                 /* 0x18 section number:offset of SS:SP */
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
    unsigned char e_res1[9];            /* 0x37 reserved */
} __attribute__((packed));              /* 0x40 */

/* Bit flags for the NE header 'flags' member */
enum HeaderFlags {
    HF_SINGLE_DATA      = 0x0001,       /* Shared automatic data segment */
    HF_MULTIPLE_DATA    = 0x0002,       /* Instanced automatic data segment */
    HF_FATAL_ERRORS     = 0x2000,       /* Errors detected at link time; module will not load */
    HF_LIBRARY          = 0x8000,       /* Module is a library */
};

class NEFileHeader : public ExecHeader {
  public:
    NEFileHeader(ExecFile *f, addr_t offset)
        : ExecHeader(f, offset, sizeof(NEFileHeader_disk)),
        dos2_header(NULL), section_table(NULL)
        {ctor(f, offset);}
    virtual ~NEFileHeader() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    virtual const char *format_name() {return "NE";}

    /* Accessors for protected/private data members */
    ExtendedDOSHeader *get_dos2_header() {return dos2_header;}
    void set_dos2_header(ExtendedDOSHeader *h) {dos2_header=h;}
    NESectionTable *get_section_table() {return section_table;}
    void set_section_table(NESectionTable *ot) {section_table=ot;}
    NEResNameTable *get_resname_table() {return resname_table;}
    void set_resname_table(NEResNameTable *ot) {resname_table=ot;}
    
    /* These are the native-format versions of the same members described in the NEFileHeader_disk format struct. */
    unsigned char e_res1[9];
    unsigned    e_linker_major, e_linker_minor, e_checksum, e_flags, e_autodata_sn, e_bss_size, e_stack_size;
    unsigned    e_csip, e_sssp, e_nsections, e_nmodrefs, e_nnonresnames, e_nmovable_entries, e_sector_align;
    unsigned    e_exetype, e_nresources;
    addr_t      e_entry_table_rfo, e_entry_table_size, e_sectab_rfo, e_rsrctab_rfo, e_resnametab_rfo, e_modreftab_rfo;
    addr_t      e_importnametab_rfo, e_nonresnametab_offset;

  private:
    void ctor(ExecFile *f, addr_t offset);
    void *encode(NEFileHeader_disk*);
    ExtendedDOSHeader *dos2_header;
    NESectionTable *section_table;
    NEResNameTable *resname_table;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NE Section (Segment) Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
/* File format of a section table entry. All fields are little endian. */
struct NESectionTableEntry_disk {
    uint16_t    sector;                 /* 0x00 File offset (sector size defined in hdr); zero means no file data */
    uint16_t    physical_size;          /* 0x02 Length of segment in file; zero means 64k */
    uint16_t    flags;                  /* 0x04 Segment bit flags */
    uint16_t    virtual_size;           /* 0x06 Total size of segment when mapped to memory; zero means 64k */
} __attribute__((packed));              /* 0x08 */

enum NESectionFlags {
    SF_TYPE_MASK = 0x0007,              /* segment-type field */
    SF_CODE      = 0x0000,              /* code-segment type */
    SF_DATA      = 0x0001,              /* data-segment type */
    SF_MOVABLE   = 0x0010,              /* segment is not fixed */
    SF_PRELOAD   = 0x0040,              /* segment will be preloaded; read-only if this is a data segment */
    SF_RELOCINFO = 0x0100,              /* segment has relocation records */
    SF_DISCARD   = 0xf000               /* discard priority */
};

class NESectionTableEntry {
  public:
    NESectionTableEntry(const NESectionTableEntry_disk *disk)
        {ctor(disk);}
    virtual ~NESectionTableEntry() {};
    void *encode(NESectionTableEntry_disk*);
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) {dump(f, prefix, idx, NULL);}
    void dump(FILE*, const char *prefix, ssize_t idx, NEFileHeader *fhdr);
    
    /* These are the native-format versions of the same members described in the NESectionTableEntry_disk struct. */
    unsigned    flags, sector;
    addr_t      physical_size, virtual_size;

  private:
    void ctor(const NESectionTableEntry_disk*);
};

/* Non-synthesized NE sections (i.e., present in the section table) */
class NESection : public ExecSection {
  public:
    NESection(ExecFile *ef, addr_t offset, addr_t size)
        : ExecSection(ef, offset, size),
        st_entry(NULL)
        {}
    virtual ~NESection() {}
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors for protected/private data */
    NESectionTableEntry *get_st_entry() {return st_entry;}
    void set_st_entry(NESectionTableEntry *e) {st_entry=e;}

  private:
    NESectionTableEntry *st_entry;
};

/* The table entries are stored in the sections themselves. */
class NESectionTable : public ExecSection {
  public:
    NESectionTable(NEFileHeader *fhdr)
        : ExecSection(fhdr->get_file(), fhdr->end_offset(), fhdr->e_nsections*sizeof(NESectionTableEntry_disk))
        {ctor(fhdr);}
    virtual ~NESectionTable() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
  private:
    void ctor(NEFileHeader*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Resident-Name Table (exported symbols)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NEResNameTable : public ExecSection 
{
  public:
    NEResNameTable(NEFileHeader *fhdr)
        : ExecSection(fhdr->get_file(), fhdr->e_resnametab_rfo+fhdr->get_offset(), 0)
        {ctor(fhdr);}
    virtual ~NEResNameTable() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
  private:
    void ctor(NEFileHeader*);
    std::vector<std::string> names;
    std::vector<unsigned> ordinals;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Functions */
bool is_NE(ExecFile*);
void parseBinaryFormat(ExecFile*, SgAsmFile* asmFile);
NEFileHeader *parse(ExecFile*);

}; //namespace NE
}; //namespace Exec

#endif /*!Exec_ExecNE_h*/
