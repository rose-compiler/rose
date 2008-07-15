/* Copyright 2008 Lawrence Livermore National Security, LLC */
#ifndef Exec_ExecNE_h
#define Exec_ExecNE_h

#include "ExecDOS.h"

namespace Exec {
namespace NE {

/* Forwards */
class NEFileHeader;
class NESectionTable;
    
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// PE Import Directory (".idata" segment)
////
//// The ".idata" segment contains import info for all functions that need to be linked in from all DLLs. The segment consists of
//// ImportDirectory objects (terminated by an all-zero value) where each ImportDirectory points to the DLL name and the names of
//// all the functions needed from that DLL.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//struct PEImportDirectory_disk {
//    uint32_t            hintnames_rva;          /* address (RVA) of array of addresses (RVAs) of hint/name pairs */
//    uint32_t            time;
//    uint32_t            forwarder_chain;
//    uint32_t            dll_name_rva;           /* address of NUL-terminated library name */
//    uint32_t            bindings_rva;           /* address (RVA) of array of object addresses after binding to DLL */
//} __attribute__((packed));
//
//class PEImportDirectory {
//  public:
//    PEImportDirectory(const PEImportDirectory_disk *disk)
//        {ctor(disk);}
//    virtual ~PEImportDirectory() {}
//    void *encode(PEImportDirectory_disk*);
//    virtual void dump(FILE*, const char *prefix, ssize_t idx);
//
//    /* Native versions of the fields in PEImportDirectory_disk */
//    addr_t              hintnames_rva, bindings_rva, dll_name_rva;
//    time_t              time;
//    unsigned            forwarder_chain;
//
//  private:
//    void ctor(const PEImportDirectory_disk*);
//};
//
//class PEDLL : public ExecDLL {
//  public:
//    PEDLL(const std::string &name)
//        : ExecDLL(name), idir(NULL)
//        {}
//    virtual ~PEDLL() {}
//    virtual void dump(FILE*, const char *prefix, ssize_t idx);
//    void add_hintname_rva(addr_t a) {hintname_rvas.push_back(a);}
//    void add_hintname(PEImportHintName *hn) {hintnames.push_back(hn);}
//    void add_binding(addr_t a) {bindings.push_back(a);}
//
//    /* Accessors for protected/private data members */
//    PEImportDirectory *get_idir() {return idir;}
//    void set_idir(PEImportDirectory *d) {idir=d;}
//    const std::vector<addr_t>& get_hintname_rvas() const {return hintname_rvas;}
//    const std::vector<PEImportHintName*>& get_hintnames() const {return hintnames;}
//    const std::vector<addr_t>& get_bindings() const {return bindings;}
//    
//  private:
//    PEImportDirectory *idir;
//    std::vector<addr_t> hintname_rvas;          /* RVAs for the hint/name pairs of the DLL functions */
//    std::vector<PEImportHintName*> hintnames;   /* The hint/name pairs */
//    std::vector<addr_t> bindings;               /* Bindings (RVA) for each function */
//};
//
//
///* Hint/name pairs */
//struct PEImportHintName_disk {
//    uint16_t            hint;                   /* Possible index into lib's export name pointer table */
//    /* NUL-terminated name follows */
//    /* Optional byte to pad struct to an even number of bytes */
//};
//
//    
//class PEImportHintName {
//  public:
//    PEImportHintName(ExecSection *section, addr_t offset)
//        : hint(0), padding('\0')
//        {ctor(section, offset);}
//    virtual ~PEImportHintName() {};
//    void unparse(FILE *f, addr_t offset); /*not the standard unparse() function*/
//    virtual void dump(FILE*, const char *prefix, ssize_t idx);
//
//    /* Accessors for protected/private data members */
//    void set_name(std::string name) {this->name=name;}
//    std::string get_name() {return name;}
//    
//  private:
//    void ctor(ExecSection*, addr_t offset);
//    unsigned hint;
//    std::string name;
//    unsigned char padding;
//};
//
//class PEImportSection : public PESection {
//  public:
//    PEImportSection(PEFileHeader *fhdr, addr_t offset, addr_t size, addr_t mapped_rva)
//        : PESection(fhdr->get_file(), offset, size)
//        {ctor(fhdr, offset, size, mapped_rva);}
//    virtual ~PEImportSection() {}
//    virtual void unparse(FILE*);
//    virtual void dump(FILE*, const char *prefix, ssize_t idx);
//    void add_dll(PEDLL *d) {dlls.push_back(d);}
//
//    /* Accessors for protected/private data members */
//    const std::vector<PEDLL*>& get_dlls() {return dlls;}
//
//  private:
//    void ctor(PEFileHeader*, addr_t offset, addr_t size, addr_t mapped_rva);
//    std::vector<PEDLL*> dlls;
//};
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// COFF Symbol Table
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///* WARNING: Instances of this type are aligned on 4-byte boundaries, which means sizeof() will return 20 rather than 18. */
//#define COFFSymbol_disk_size 18
//struct COFFSymbol_disk {
//    union {
//        char            st_name[8];
//        struct {
//            uint32_t    st_zero;
//            uint32_t    st_offset;
//        };
//    };
//    uint32_t            st_value;
//    int16_t             st_section_num;
//    uint16_t            st_type;
//    unsigned char       st_storage_class;
//    unsigned char       st_num_aux_entries;
//} __attribute__((packed));
//
//class COFFSymbol : public ExecSymbol {
//  public:
//    COFFSymbol(PEFileHeader *fhdr, ExecSection *symtab, ExecSection *strtab, size_t idx)
//        : st_name_offset(0), st_section_num(0), st_type(0), st_storage_class(0), st_num_aux_entries(0),
//        aux_data(NULL), aux_size(0)
//        {ctor(fhdr, symtab, strtab, idx);}
//    virtual ~COFFSymbol() {}
//    void *encode(COFFSymbol_disk*);
//    virtual void dump(FILE *f, const char *prefix, ssize_t idx);
//
//    /* Auxilliary data for the symbol */
//    const unsigned char *get_aux_data() {return aux_data;}
//    size_t get_aux_size() {return aux_size;}
//
//    /* Native versions of the COFFSymbol_disk members */
//    std::string         st_name;        /* The original name; super.name might be modified */
//    addr_t              st_name_offset;
//    int                 st_section_num;
//    unsigned            st_type, st_storage_class, st_num_aux_entries;
//    
//  private:
//    void ctor(PEFileHeader*, ExecSection *symtab, ExecSection *strtab, size_t idx);
//    const unsigned char *aux_data;      /* Auxilliary data from table entries that follow */
//    size_t              aux_size;       /* Size (bytes) of auxilliary data */
//};
//
//class COFFSymtab : public ExecSection {
//  public:
//    COFFSymtab(ExecFile *f, PEFileHeader *fhdr)
//        : ExecSection(f, fhdr->e_coff_symtab, fhdr->e_coff_nsyms*COFFSymbol_disk_size)
//        {ctor(f, fhdr);}
//    virtual ~COFFSymtab() {}
//    virtual void unparse(FILE*);
//    virtual void dump(FILE*, const char *prefix, ssize_t idx);
//
//    /* Accessors for protected/private data members */
//    std::vector<COFFSymbol*>& get_symbols() {return symbols;}
//    ExecSection *get_strtab() {return strtab;}
//
//  private:
//    void ctor(ExecFile*, PEFileHeader*);
//    std::vector<COFFSymbol*> symbols;
//    ExecSection *strtab;                /* Section containing symbol names */
//};
//

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Functions */
bool is_NE(ExecFile*);
void parseBinaryFormat(ExecFile*, SgAsmFile* asmFile);
NEFileHeader *parse(ExecFile*);

}; //namespace NE
}; //namespace Exec

#endif /*!Exec_ExecNE_h*/
