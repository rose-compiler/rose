/* Copyright 2008 Lawrence Livermore National Security, LLC */
#ifndef Exec_ExecPE_h
#define Exec_ExecPE_h

#include "ExecDOS.h"

namespace Exec {
namespace PE {

/* Forwards */
class COFFSymtab;
class PESectionTable;
class PEImportHintName;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtendedDOSHeader -- extra components of the DOS header when used in a PE file
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ExtendedDOSHeader_disk {
    unsigned char e_res1[4];            /* 0x00 reserved */
    uint16_t    e_oemid;                /* 0x04 OEM Identifier */
    uint16_t    e_oeminfo;              /* 0x06 other OEM information; oemid specific */
    uint16_t    e_res2[10];             /* 0x08 reserved */
    uint32_t    e_lfanew;               /* 0x1c file offset of new exe (PE) header */
} __attribute__((packed));              /* 0x20 */

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
    unsigned e_res1[4], e_oemid, e_oeminfo, e_res2[10];
    addr_t e_lfanew;

  private:
    void ctor(ExecFile *f, addr_t offset);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE File Header
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* File format for an RVA/Size pair. Such pairs are considered to be part of the PE file header. All fields are little endian. */
struct RVASizePair_disk {
    uint32_t    e_rva;
    uint32_t    e_size;
} __attribute__((packed));

class RVASizePair {
  public:
    RVASizePair(const RVASizePair_disk *disk) {
        e_rva  = le_to_host(disk->e_rva);
        e_size = le_to_host(disk->e_size);
    }
    void *encode(RVASizePair_disk *disk) {
        host_to_le(e_rva,  &(disk->e_rva));
        host_to_le(e_size, &(disk->e_size));
        return disk;
    }
    addr_t      e_rva, e_size;
};

/* File format of a PE File Header. All fields are little endian. */
struct PEFileHeader_disk {
    unsigned char e_magic[4];           /* magic number "PE\0\0" */
    uint16_t    e_cpu_type;             /* e.g., 0x014c = Intel 386 */
    uint16_t    e_nsections;            /* number of sections defined in the Section Table */
    uint32_t    e_time;                 /* time and date file was created or modified by the linker */
    uint32_t    e_coff_symtab;          /* offset to COFF symbol table */
    uint32_t    e_coff_nsyms;           /* number of symbols in COFF symbol table */
    uint16_t    e_nt_hdr_size;          /* number of remaining bytes in the header following the 'flags' field */
    uint16_t    e_flags;                /* Bit flags: executable file, program/library image, fixed address, etc. */
} __attribute__((packed));

struct PE32OptHeader_disk {
    uint16_t    e_opt_magic;            /* magic number */
    uint16_t    e_lmajor;               /* linker version */
    uint16_t    e_lminor;
    uint16_t    e_code_size;            /* Size of .text or sum of all code sections */
    uint32_t    e_data_size;            /* Sum size of initialized data */
    uint32_t    e_bss_size;             /* Sum size of uninitialized data */
    uint32_t    e_entrypoint_rva;       /* RVA="relative virtual address"; relative to 'image_base', below */
    uint32_t    e_code_rva;             /* Address relative to image base for code section when memory mapped */
    uint32_t    e_data_rva;             /* Address relative to image base for data section */
    uint32_t    e_image_base;           /* Virtual base of the image (first byte of file, DOS header). Multiple of 64k. */
    uint32_t    e_section_align;        /* Alignment of sections in memory. Power of two 512<=x<=256M */
    uint32_t    e_file_align;           /* Alignment factor (in bytes) for image pages */
    uint16_t    e_os_major;             /* OS version number required to run this image */
    uint16_t    e_os_minor;
    uint16_t    e_user_major;           /* User-specified at link time. Useful for differentiating between image revisions */
    uint16_t    e_user_minor;
    uint16_t    e_subsys_major;         /* Subsystem version number */
    uint16_t    e_subsys_minor;
    uint32_t    e_reserved9;
    uint32_t    e_image_size;           /* Virtual size (bytes) of the image inc. all headers; multiple of 'section_align' */
    uint32_t    e_header_size;          /* Total header size (DOS Header + PE Header + Section table */
    uint32_t    e_file_checksum;        /* Checksum for entire file; Set to zero by the linker */
    uint16_t    e_subsystem;            /* Unknown, Native, WindowsGUI, WindowsCharacter, OS/2 Character, POSIX Character */
    uint16_t    e_dll_flags;            /* Bit flags for library init/terminate per process or thread */
    uint32_t    e_stack_reserve_size;   /* Virtual memory reserved for stack; non-committed pages are guards */
    uint32_t    e_stack_commit_size;    /* Size (bytes) of valid stack; other pages are guards; <= 'stack_reserve_size' */
    uint32_t    e_heap_reserve_size;    /* Size (bytes) of local heap to reserve */
    uint32_t    e_heap_commit_size;     /* Size (bytes) of valid local heap */
    uint32_t    e_loader_flags;         /* Reserved, must be zero */
    uint32_t    e_num_rvasize_pairs;    /* Number of RVASizePair entries that follow this member; also part of the PE header */
} __attribute__((packed));

struct PE64OptHeader_disk {
    uint16_t    e_opt_magic;
    uint16_t    e_lmajor;
    uint16_t    e_lminor;
    uint16_t    e_code_size;
    uint32_t    e_data_size;
    uint32_t    e_bss_size;
    uint32_t    e_entrypoint_rva;
    uint32_t    e_code_rva;
    //uint32_t  e_data_rva;             /* Not present in PE32+ */
    uint64_t    e_image_base;
    uint32_t    e_section_align;
    uint32_t    e_file_align;
    uint16_t    e_os_major;
    uint16_t    e_os_minor;
    uint16_t    e_user_major;
    uint16_t    e_user_minor;
    uint16_t    e_subsys_major;
    uint16_t    e_subsys_minor;
    uint32_t    e_reserved9;
    uint32_t    e_image_size;
    uint32_t    e_header_size;
    uint32_t    e_file_checksum;
    uint16_t    e_subsystem;
    uint16_t    e_dll_flags;
    uint64_t    e_stack_reserve_size;
    uint64_t    e_stack_commit_size;
    uint64_t    e_heap_reserve_size;
    uint64_t    e_heap_commit_size;
    uint32_t    e_loader_flags;
    uint32_t    e_num_rvasize_pairs;
} __attribute__((packed));

/* Bit flags for the PE header 'flags' member */
enum HeaderFlags {
    HF_PROGRAM          = 0x0000,       /* Program image (no non-reserved bits set) */
    HF_EXECUTABLE       = 0x0002,       /* Clear indicates can't load: either link errors or image is being incrementally linked */
    HF_FIXED            = 0x0200,       /* Image *must* be loaded at image_base address or error */
    HF_LIBRARY          = 0x2000,       /* Library image */
    HF_RESERVED_MASK    = 0xddfd        /* Reserved bits */
};

/* Values for the PE header 'subsystem' member */
enum Subsystem {
    HF_SPEC_UNKNOWN     = 0x0000,       /* Specified as 'unknown' in the file */
    HF_NATIVE           = 0x0001,       /* Native */
    HF_WINGUI           = 0x0002,       /* Windows GUI */
    HF_WINCHAR          = 0x0003,       /* Windows character */
    HF_OS2CHAR          = 0x0005,       /* OS/2 character */
    HF_POSIX            = 0x0007        /* POSIX character */
};

/* Bit flags for the PE header 'dll_flags' member */
enum DLLFlags {
    DLL_PROC_INIT       = 0x0001,       /* Per-process library initialization */
    DLL_PROC_TERM       = 0x0002,       /* Per-process library termination */
    DLL_THRD_INIT       = 0x0004,       /* Per-thread library initialization */
    DLL_THRD_TERM       = 0x0008,       /* Per-thread library termination */
    DLL_RESERVED_MASK   = 0xfff0        /* Reserved bits */
};

class PEFileHeader : public ExecHeader {
  public:
    PEFileHeader(ExecFile *f, addr_t offset)
        : ExecHeader(f, offset, sizeof(PEFileHeader_disk)), /*extended in ctor()*/
        dos2_header(NULL), section_table(NULL), coff_symtab(NULL)
        {ctor(f, offset);}
    virtual ~PEFileHeader() {}
    void add_rvasize_pairs();
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    virtual const char *format_name() {return "PE";}

    /* Accessors for protected/private data members */
    ExtendedDOSHeader *get_dos2_header() {return dos2_header;}
    void set_dos2_header(ExtendedDOSHeader *h) {dos2_header=h;}
    PESectionTable *get_section_table() {return section_table;}
    void set_section_table(PESectionTable *ot) {section_table=ot;}
    COFFSymtab *get_coff_symtab() {return coff_symtab;}
    void set_coff_symtab(COFFSymtab *st) {coff_symtab=st;}
    
    /* These are the native-format versions of the same members described in the PEFileHeader_disk format struct. */
    unsigned    e_cpu_type, e_nsections, e_time;
    addr_t      e_coff_symtab, e_nt_hdr_size;
    unsigned    e_coff_nsyms, e_flags, e_opt_magic;
    unsigned    e_lmajor, e_lminor, e_code_size, e_data_size, e_bss_size, e_entrypoint_rva, e_code_rva, e_data_rva;
    addr_t      e_image_base;
    unsigned    e_section_align, e_file_align, e_os_major, e_os_minor, e_user_major, e_user_minor;
    unsigned    e_subsys_major, e_subsys_minor, e_reserved9, e_image_size, e_header_size, e_file_checksum, e_subsystem;
    unsigned    e_dll_flags, e_stack_reserve_size, e_stack_commit_size, e_heap_reserve_size, e_heap_commit_size;
    unsigned    e_loader_flags, e_num_rvasize_pairs;
    std::vector<RVASizePair> rvasize_pairs;

  private:
    void ctor(ExecFile *f, addr_t offset);
    void *encode(PEFileHeader_disk*);
    void *encode(PE32OptHeader_disk*);
    void *encode(PE64OptHeader_disk*);
    ExtendedDOSHeader *dos2_header;
    PESectionTable *section_table;
    COFFSymtab *coff_symtab;
};

    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Section Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* File format of a section table entry. All fields are little endian. Sections are ordered by RVA. */
struct PESectionTableEntry_disk {
    char        name[8];                /* NUL-padded */
    uint32_t    virtual_size;           /* virtual memory size (bytes), >= physical_size and difference is zero filled */
    uint32_t    rva;                    /* relative virtual address wrt Image Base; multiple of section_align; dense space */
    uint32_t    physical_size;          /* bytes of initialized data on disk; multiple of file_align & <= virtual_size */
    uint32_t    physical_offset;        /* location of initialized data on disk; multiple of file_align */
    uint32_t    coff_line_nums;         /* file offset of COFF line number info or zero */
    uint32_t    n_relocs;               /* number of relocation entries; should be zero for executables */
    uint32_t    n_coff_line_nums;       /* number of COFF line number entries */
    uint32_t    flags;                  /* PESectionFlags bits: code, data, caching, paging, shared, permissions, etc. */
} __attribute__((packed));

/* These come from the windows PE documentation and http://en.wikibooks.org/wiki/X86_Disassembly/Windows_Executable_Files */
enum PESectionFlags {
    OF_CODE             = 0x00000020,   /* section contains code */
    OF_IDATA            = 0x00000040,   /* initialized data */
    OF_UDATA            = 0x00000080,   /* uninitialized data */
    OF_INFO             = 0x00000200,   /* comments or some other type of info */
    OF_REMOVE           = 0x00000800,   /* section will not become part of image */
    OF_COMDAT           = 0x00001000,   /* section contains comdat */
    OF_NO_DEFER_SPEC_EXC= 0x00004000,   /* reset speculative exception handling bits in the TLB entires for this section */
    OF_GPREL            = 0x00008000,   /* section content can be access relative to GP */
    OF_ALIGN_1          = 0x00100000,   /* no alignment */
    OF_ALIGN_2          = 0x00200000,   /* 2-byte alignment */
    OF_ALIGN_4          = 0x00300000,   /* 4-byte alignment */
    OF_ALIGN_8          = 0x00400000,   /* 8-byte alignment */
    OF_ALIGN_16         = 0x00500000,   /* 16-byte alignment (default if not other alignment specified) */
    OF_ALIGN_32         = 0x00600000,   /* 32-byte alignment */
    OF_ALIGN_64         = 0x00700000,   /* 64-byte alignment */
    OF_ALIGN_128        = 0x00800000,   /* 128-byte alignment */
    OF_ALIGN_256        = 0x00900000,   /* 256-byte alignment */
    OF_ALIGN_512        = 0x00a00000,   /* 512-byte alignment */
    OF_ALIGN_1k         = 0x00b00000,   /* 1024-byte alignment */
    OF_ALIGN_2k         = 0x00c00000,   /* 2048-byte alignment */
    OF_ALIGN_4k         = 0x00d00000,   /* 4096-byte alignment */
    OF_ALIGN_8k         = 0x00e00000,   /* 8192-byte alignment */
    OF_ALIGN_MASK       = 0x00f00000,   /* mask for alignment value */
    OF_NRELOC_OVFL      = 0x01000000,   /* section contains extended relocations */
    OF_DISCARDABLE      = 0x02000000,   /* can be discarded */
    OF_NO_CACHE         = 0x04000000,   /* section must not be cached */
    OF_NO_PAGING        = 0x08000000,   /* section is not pageable */
    OF_SHARED           = 0x10000000,   /* section is shared */
    OF_EXECUTABLE       = 0x20000000,   /* execute permission */
    OF_READABLE         = 0x40000000,   /* read permission */
    OF_WRITABLE         = 0x80000000,   /* write permission */
};

class PESectionTableEntry {
  public:
    PESectionTableEntry(const PESectionTableEntry_disk *disk)
        {ctor(disk);}
    virtual ~PESectionTableEntry() {};
    void *encode(PESectionTableEntry_disk*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    
    /* These are the native-format versions of the same members described in the PESectionTableEntry_disk struct. */
    std::string name;
    addr_t      virtual_size, rva, physical_size, physical_offset;
    unsigned    coff_line_nums, n_relocs, n_coff_line_nums, flags;

  private:
    void ctor(const PESectionTableEntry_disk*);
};

/* Non-synthesized PE sections (i.e., present in the section table) */
class PESection : public ExecSection {
  public:
    PESection(ExecFile *ef, addr_t offset, addr_t size)
        : ExecSection(ef, offset, size),
        st_entry(NULL)
        {}
    virtual ~PESection() {}
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors for protected/private data */
    PESectionTableEntry *get_st_entry() {return st_entry;}
    void set_st_entry(PESectionTableEntry *e) {st_entry=e;}

  private:
    PESectionTableEntry *st_entry;
};

/* The table entries are stored in the segments themselves. We can reconstruct the table by realizing that the segments each
 * live in their own section and the section IDs are generated from the table entry indices. */
class PESectionTable : public ExecSection {
  public:
    PESectionTable(PEFileHeader *fhdr, addr_t offset, addr_t size)
        : ExecSection(fhdr->get_file(), offset, size)
        {ctor(fhdr);}
    virtual ~PESectionTable() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
  private:
    void ctor(PEFileHeader*);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PE Import Directory (".idata" segment)
//
// The ".idata" segment contains import info for all functions that need to be linked in from all DLLs. The segment consists of
// ImportDirectory objects (terminated by an all-zero value) where each ImportDirectory points to the DLL name and the names of
// all the functions needed from that DLL.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PEImportDirectory_disk {
    uint32_t            hintnames_rva;          /* address (RVA) of array of addresses (RVAs) of hint/name pairs */
    uint32_t            time;
    uint32_t            forwarder_chain;
    uint32_t            dll_name_rva;           /* address of NUL-terminated library name */
    uint32_t            bindings_rva;           /* address (RVA) of array of object addresses after binding to DLL */
} __attribute__((packed));

class PEImportDirectory {
  public:
    PEImportDirectory(const PEImportDirectory_disk *disk)
        {ctor(disk);}
    virtual ~PEImportDirectory() {}
    void *encode(PEImportDirectory_disk*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Native versions of the fields in PEImportDirectory_disk */
    addr_t              hintnames_rva, bindings_rva, dll_name_rva;
    time_t              time;
    unsigned            forwarder_chain;

  private:
    void ctor(const PEImportDirectory_disk*);
};

class PEDLL : public ExecDLL {
  public:
    PEDLL(const std::string &name)
        : ExecDLL(name), idir(NULL)
        {}
    virtual ~PEDLL() {}
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    void add_hintname_rva(addr_t a) {hintname_rvas.push_back(a);}
    void add_hintname(PEImportHintName *hn) {hintnames.push_back(hn);}
    void add_binding(addr_t a) {bindings.push_back(a);}

    /* Accessors for protected/private data members */
    PEImportDirectory *get_idir() {return idir;}
    void set_idir(PEImportDirectory *d) {idir=d;}
    const std::vector<addr_t>& get_hintname_rvas() const {return hintname_rvas;}
    const std::vector<PEImportHintName*>& get_hintnames() const {return hintnames;}
    const std::vector<addr_t>& get_bindings() const {return bindings;}
    
  private:
    PEImportDirectory *idir;
    std::vector<addr_t> hintname_rvas;          /* RVAs for the hint/name pairs of the DLL functions */
    std::vector<PEImportHintName*> hintnames;   /* The hint/name pairs */
    std::vector<addr_t> bindings;               /* Bindings (RVA) for each function */
};


/* Hint/name pairs */
struct PEImportHintName_disk {
    uint16_t            hint;                   /* Possible index into lib's export name pointer table */
    /* NUL-terminated name follows */
    /* Optional byte to pad struct to an even number of bytes */
};

    
class PEImportHintName {
  public:
    PEImportHintName(ExecSection *section, addr_t offset)
        : hint(0), padding('\0')
        {ctor(section, offset);}
    virtual ~PEImportHintName() {};
    void unparse(FILE *f, addr_t offset); /*not the standard unparse() function*/
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors for protected/private data members */
    void set_name(std::string name) {this->name=name;}
    std::string get_name() {return name;}
    
  private:
    void ctor(ExecSection*, addr_t offset);
    unsigned hint;
    std::string name;
    unsigned char padding;
};

class PEImportSection : public PESection {
  public:
    PEImportSection(PEFileHeader *fhdr, addr_t offset, addr_t size, addr_t mapped_rva)
        : PESection(fhdr->get_file(), offset, size)
        {ctor(fhdr, offset, size, mapped_rva);}
    virtual ~PEImportSection() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);
    void add_dll(PEDLL *d) {dlls.push_back(d);}

    /* Accessors for protected/private data members */
    const std::vector<PEDLL*>& get_dlls() {return dlls;}

  private:
    void ctor(PEFileHeader*, addr_t offset, addr_t size, addr_t mapped_rva);
    std::vector<PEDLL*> dlls;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// COFF Symbol Table
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* WARNING: Instances of this type are aligned on 4-byte boundaries, which means sizeof() will return 20 rather than 18. */
#define COFFSymbol_disk_size 18
struct COFFSymbol_disk {
    union {
        char            st_name[8];
        struct {
            uint32_t    st_zero;
            uint32_t    st_offset;
        };
    };
    uint32_t            st_value;
    int16_t             st_section_num;
    uint16_t            st_type;
    unsigned char       st_storage_class;
    unsigned char       st_num_aux_entries;
} __attribute__((packed));

class COFFSymbol : public ExecSymbol {
  public:
    COFFSymbol(PEFileHeader *fhdr, ExecSection *symtab, ExecSection *strtab, size_t idx)
        : st_name_offset(0), st_section_num(0), st_type(0), st_storage_class(0), st_num_aux_entries(0),
        aux_data(NULL), aux_size(0)
        {ctor(fhdr, symtab, strtab, idx);}
    virtual ~COFFSymbol() {}
    void *encode(COFFSymbol_disk*);
    virtual void dump(FILE *f, const char *prefix, ssize_t idx);

    /* Auxilliary data for the symbol */
    const unsigned char *get_aux_data() {return aux_data;}
    size_t get_aux_size() {return aux_size;}

    /* Native versions of the COFFSymbol_disk members */
    std::string         st_name;        /* The original name; super.name might be modified */
    addr_t              st_name_offset;
    int                 st_section_num;
    unsigned            st_type, st_storage_class, st_num_aux_entries;
    
  private:
    void ctor(PEFileHeader*, ExecSection *symtab, ExecSection *strtab, size_t idx);
    const unsigned char *aux_data;      /* Auxilliary data from table entries that follow */
    size_t              aux_size;       /* Size (bytes) of auxilliary data */
};

class COFFSymtab : public ExecSection {
  public:
    COFFSymtab(ExecFile *f, PEFileHeader *fhdr)
        : ExecSection(f, fhdr->e_coff_symtab, fhdr->e_coff_nsyms*COFFSymbol_disk_size)
        {ctor(f, fhdr);}
    virtual ~COFFSymtab() {}
    virtual void unparse(FILE*);
    virtual void dump(FILE*, const char *prefix, ssize_t idx);

    /* Accessors for protected/private data members */
    std::vector<COFFSymbol*>& get_symbols() {return symbols;}
    ExecSection *get_strtab() {return strtab;}

  private:
    void ctor(ExecFile*, PEFileHeader*);
    std::vector<COFFSymbol*> symbols;
    ExecSection *strtab;                /* Section containing symbol names */
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* Functions */
bool is_PE(ExecFile*);
void parseBinaryFormat(ExecFile*, SgAsmFile* asmFile);
PEFileHeader *parse(ExecFile*);

}; //namespace PE
}; //namespace Exec

#endif /*!Exec_ExecPE_h*/
