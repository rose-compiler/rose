#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

#ifdef ROSE_IMPL
#include <SgAsmCoffSymbolTable.h>
#include <SgAsmPERVASizePairList.h>
#include <SgAsmPESectionTable.h>
#endif

/** Windows PE file header.
 *
 *  Most of the properties come directly from the PE specification and their documentation is not repeated here. */
class SgAsmPEFileHeader: public SgAsmGenericHeader {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /* File format of a PE File Header. All fields are little endian. */
    struct PEFileHeader_disk {
        unsigned char e_magic[4];       /* 0x00 magic number "PE\0\0" */
        uint16_t    e_cpu_type;         /* 0x04 e.g., 0x014c = Intel 386 */
        uint16_t    e_nsections;        /* 0x06 number of sections defined in the Section Table */
        uint32_t    e_time;             /* 0x08 time and date file was created or modified by the linker */
        uint32_t    e_coff_symtab;      /* 0x0c offset to COFF symbol table */
        uint32_t    e_coff_nsyms;       /* 0x10 number of symbols in COFF symbol table */
        uint16_t    e_nt_hdr_size;      /* 0x14 num remaining bytes in the header following the 'flags' field */
        uint16_t    e_flags;            /* 0x16 Bit flags: exe file, program/library image, fixed address, etc. */
    }                                       /* 0x18 */
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct PE32OptHeader_disk {
        uint16_t    e_opt_magic;        /* 0x18 magic number */
        uint16_t    e_lmajor;           /* 0x1a linker version */
        uint16_t    e_lminor;           /* 0x1c */
        uint16_t    e_code_size;        /* 0x1e Size of .text or sum of all code sections */
        uint32_t    e_data_size;        /* 0x20 Sum size of initialized data */
        uint32_t    e_bss_size;         /* 0x24 Sum size of uninitialized data */
        uint32_t    e_entrypoint_rva;   /* 0x28 RVA="relative virtual address"; relative to 'image_base', below */
        uint32_t    e_code_rva;         /* 0x2c Addr relative to image base for code section when memory mapped */
        uint32_t    e_data_rva;         /* 0x30 Address relative to image base for data section */
        uint32_t    e_image_base;       /* 0x34 Virt base of image (first byte of file, DOS header). 64k aligned */
        uint32_t    e_section_align;    /* 0x38 Alignment of sections in memory. Power of two 512<=x<=256M */
        uint32_t    e_file_align;       /* 0x3c Alignment factor (in bytes) for image pages */
        uint16_t    e_os_major;         /* 0x40 OS version number required to run this image */
        uint16_t    e_os_minor;         /* 0x42 */
        uint16_t    e_user_major;       /* 0x44 User-specified for differentiating between image revs */
        uint16_t    e_user_minor;       /* 0x46 */
        uint16_t    e_subsys_major;     /* 0x48 Subsystem version number */
        uint16_t    e_subsys_minor;     /* 0x4a */
        uint32_t    e_reserved9;        /* 0x4c */
        uint32_t    e_image_size;       /* 0x50 Virtual size of the image inc. all headers; section_align */
        uint32_t    e_header_size;      /* 0x54 Total header size (DOS Header + PE Header + Section table */
        uint32_t    e_file_checksum;    /* 0x58 Checksum for entire file; Set to zero by the linker */
        uint16_t    e_subsystem;        /* 0x5c Native, WindowsGUI, WindowsCharacter, OS/2 Character, etc. */
        uint16_t    e_dll_flags;        /* 0x5e Bit flags for library init/terminate per process or thread */
        uint32_t    e_stack_reserve_size;/*0x60 Virtual mem reserved for stack; non-committed pages are guards */
        uint32_t    e_stack_commit_size;/* 0x64 Size of valid stack; other pages are guards; <=stack_reserve_size*/
        uint32_t    e_heap_reserve_size;/* 0x68 Size (bytes) of local heap to reserve */
        uint32_t    e_heap_commit_size; /* 0x6c Size (bytes) of valid local heap */
        uint32_t    e_loader_flags;     /* 0x70 Reserved, must be zero */
        uint32_t    e_num_rvasize_pairs;/* 0x74 Num RVASizePair entries that follow this member; part of header */
    }                                       /* 0x78 */
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct PE64OptHeader_disk {
        uint16_t    e_opt_magic;        /* 0x18 */
        uint16_t    e_lmajor;           /* 0x1a */
        uint16_t    e_lminor;           /* 0x1c */
        uint16_t    e_code_size;        /* 0x1e */
        uint32_t    e_data_size;        /* 0x20 */
        uint32_t    e_bss_size;         /* 0x24 */
        uint32_t    e_entrypoint_rva;   /* 0x28 */
        uint32_t    e_code_rva;         /* 0x2c */
        // uint32_t  e_data_rva;             /* Not present in PE32+ */
        uint64_t    e_image_base;       /* 0x30 */
        uint32_t    e_section_align;    /* 0x38 */
        uint32_t    e_file_align;       /* 0x3c */
        uint16_t    e_os_major;         /* 0x40 */
        uint16_t    e_os_minor;         /* 0x42 */
        uint16_t    e_user_major;       /* 0x44 */
        uint16_t    e_user_minor;       /* 0x46 */
        uint16_t    e_subsys_major;     /* 0x48 */
        uint16_t    e_subsys_minor;     /* 0x4a */
        uint32_t    e_reserved9;        /* 0x4c */
        uint32_t    e_image_size;       /* 0x50 */
        uint32_t    e_header_size;      /* 0x54 */
        uint32_t    e_file_checksum;    /* 0x58 */
        uint16_t    e_subsystem;        /* 0x5c */
        uint16_t    e_dll_flags;        /* 0x5e */
        uint64_t    e_stack_reserve_size;/*0x60 */
        uint64_t    e_stack_commit_size;/* 0x68 */
        uint64_t    e_heap_reserve_size;/* 0x70 */
        uint64_t    e_heap_commit_size; /* 0x78 */
        uint32_t    e_loader_flags;     /* 0x80 */
        uint32_t    e_num_rvasize_pairs;/* 0x84 */
    }                                       /* 0x88 */
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
        ;
#ifdef _MSC_VER
# pragma pack ()
#endif

    /* Bit flags for the PE header 'flags' member */
    enum HeaderFlags {
        HF_PROGRAM          = 0x0000,       /* Program image (no non-reserved bits set) */
        HF_EXECUTABLE       = 0x0002,       /* Clear indicates can't load: link errors or incrementally linked */
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

    /** Reason for each rva/size pair in the PE header. */
    enum PairPurpose {              // Values are important
        PAIR_EXPORTS            = 0,
        PAIR_IMPORTS            = 1,
        PAIR_RESOURCES          = 2,
        PAIR_EXCEPTIONS         = 3,
        PAIR_CERTIFICATES       = 4,
        PAIR_BASERELOCS         = 5,
        PAIR_DEBUG              = 6,
        PAIR_ARCHITECTURE       = 7,
        PAIR_GLOBALPTR          = 8,
        PAIR_TLS                = 9,    // Thread local storage
        PAIR_LOADCONFIG         = 10,
        PAIR_BOUNDIMPORT        = 11,
        PAIR_IAT                = 12,   // Import address table
        PAIR_DELAYIMPORT        = 13,   // Delay import descriptor
        PAIR_CLRRUNTIME         = 14,   // CLR(?) runtime header
        PAIR_RESERVED15         = 15    // Reserved (always zero according to specification)
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: CPU type.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_cpu_type = 0;

    /** Property: Number of sections.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_nsections = 0;

    /** Property: Time.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_time = 0;

    /** Property: COFF symbol table.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address e_coff_symtab = 0;

    /** Property: Size of NT header.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address e_nt_hdr_size = 0;

    /** Property: Number of COFF symbols.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_coff_nsyms = 0;

    /** Property: Flags
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_flags = 0;

    /** Property: Magic.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_opt_magic = 0;

    /** Property: lmajor.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_lmajor = 0;

    /** Property: lminor.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_lminor = 0;

    /** Property: Code size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_code_size = 0;

    /** Property: Data size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_data_size = 0;

    /** Property: BSS size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_bss_size = 0;

    /** Property: Code RVA.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress e_code_rva;

    /** Property: Data RVA.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress e_data_rva;

    /** Property: Section alignment.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_section_align = 0;

    /** Property: File alignment.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_file_align = 0;

    /** Property: OS major number.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_os_major = 0;

    /** Property: OS minor number.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_os_minor = 0;

    /** Property: User major number.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_user_major = 0;

    /** Property: User minor number.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_user_minor = 0;

    /** Property: Subsystem major number.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_subsys_major = 0;

    /** Property: Subsystem minor number.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_subsys_minor = 0;

    /** Property: Reserved area #9.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_reserved9 = 0;

    /** Property: Image size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_image_size = 0;

    /** Property: Header size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_header_size = 0;

    /** Property: File checksum.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_file_checksum = 0;

    /** Property: Subsystem.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_subsystem = 0;

    /** Property: DLL flags.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_dll_flags = 0;

    /** Property: Stack reserve size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_stack_reserve_size = 0;

    /** Property: Stack commit size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_stack_commit_size = 0;

    /** Property: Heap reserve size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_heap_reserve_size = 0;

    /** Property: Heap commit size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_heap_commit_size = 0;

    /** Property: Loader flags.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_loader_flags = 0;

    /** Property: Number of RVA/size pairs.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_num_rvasize_pairs = 0;

    /** Property: RVA/size pairs.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmPERVASizePairList* rvaSizePairs = nullptr;

    /** Property: Section table.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    SgAsmPESectionTable* sectionTable = nullptr;

    /** Property: COFF symbol table.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    SgAsmCoffSymbolTable* coffSymbolTable = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    explicit SgAsmPEFileHeader(SgAsmGenericFile*);

    virtual const char* formatName() const override;

    /** Return true if the file looks like it might be a PE file according to the magic number.
     *
     *  The file must contain what appears to be a DOS File Header at address zero, and what appears to be a PE File Header
     *  at a file offset specified in part of the DOS File Header (actually, in the bytes that follow the DOS File
     *  Header). */
    static bool isPe(SgAsmGenericFile*);

    /** Convert an RVA/Size Pair index number into a section name.
     *
     *  The enum value passed as the first argument is looked up in an internal table and its full, static name from the PE
     *  documentation is returned. Additionally, if @p short_name is non-null then it points to a static abbreviated name. For
     *  example, if the first argument is @c PAIR_EXPORTS from the @ref SgAsmPEFileHeader::PairPurpose enum, then this function
     *  returns "Export Table" as the full name, and "Exports" as the abbreviated name. */
    std::string rvaSizePairName(PairPurpose, const char **short_name);

    /** Define an RVA/Size pair in the PE file header. */
    void set_rvaSizePair(PairPurpose, SgAsmPESection*);

    /** Update all the RVA/Size pair info from the section to which it points. */
    void updateRvaSizePairs();

    void addRvaSizePairs();

    virtual SgAsmPEFileHeader *parse() override;
    virtual bool reallocate() override;
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    void createTableSections();

    /* Loader memory maps */
    Rose::BinaryAnalysis::MemoryMap::Ptr get_loaderMap() const;
    void set_loaderMap(const Rose::BinaryAnalysis::MemoryMap::Ptr&);

private:
    void *encode(SgAsmPEFileHeader::PEFileHeader_disk*) const;
    void *encode(SgAsmPEFileHeader::PE32OptHeader_disk*) const;
    void *encode(SgAsmPEFileHeader::PE64OptHeader_disk*) const;
    Rose::BinaryAnalysis::MemoryMap::Ptr p_loader_map;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmPERVASizePairList* get_rvasize_pairs() const ROSE_DEPRECATED("use get_rvaSizePairs");
    void set_rvasize_pairs(SgAsmPERVASizePairList*) ROSE_DEPRECATED("use set_rvaSizePairs");
    SgAsmPESectionTable* get_section_table() const ROSE_DEPRECATED("use get_sectionTable");
    void set_section_table(SgAsmPESectionTable*) ROSE_DEPRECATED("use set_sectionTable");
    SgAsmCoffSymbolTable* get_coff_symtab() const ROSE_DEPRECATED("use get_coffSymbolTable");
    void set_coff_symtab(SgAsmCoffSymbolTable*) ROSE_DEPRECATED("use set_coffSymbolTable");
    virtual const char *format_name() const override ROSE_DEPRECATED("use formatName");
    static bool is_PE (SgAsmGenericFile*) ROSE_DEPRECATED("use isPe");
    std::string rvasize_pair_name(PairPurpose, const char**) ROSE_DEPRECATED("use rvaSizePairName");
    void set_rvasize_pair(PairPurpose, SgAsmPESection*) ROSE_DEPRECATED("use set_rvaSizePair");
    void update_rvasize_pairs() ROSE_DEPRECATED("use updateRvaSizePairs");
    void add_rvasize_pairs() ROSE_DEPRECATED("use addRvaSizePairs");
    void create_table_sections() ROSE_DEPRECATED("use createTableSections");
    Rose::BinaryAnalysis::MemoryMap::Ptr get_loader_map() const ROSE_DEPRECATED("use get_loaderMap");
    void set_loader_map(const Rose::BinaryAnalysis::MemoryMap::Ptr&) ROSE_DEPRECATED("use set_loaderMap");
};
