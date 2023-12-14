#include <Rose/BinaryAnalysis/ByteOrder.h>

/** Represents the file header of an ELF binary container.
 *
 *  The file header contains information that the operating system uses to find the various parts within the
 *  container. Most of the object properties are defined in the official ELF specification and their documentation is not
 *  replicated here.
 *
 *  ROSE does not require or use an ELF support library or headers and is thus able to parse ELF files on systems where ELF
 *  is not normally employed. */
class SgAsmElfFileHeader: public SgAsmGenericHeader {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Enum for the @ref e_type property. */
    enum ObjectType {
        ET_NONE         = 0                         /**< No file type */
        ,ET_REL          = 1                        /**< Relocatable file */
        ,ET_EXEC         = 2                        /**< Executable file */
        ,ET_DYN          = 3                        /**< Shared object file */
        ,ET_CORE         = 4                        /**< Core file */

        ,ET_LOOS         = 0xfe00                   /**< OS-specific range start */
        ,ET_HIOS         = 0xfeff                   /**< OS-specific range end */
        ,ET_LOPROC       = 0xff00                   /**< Processor-specific range start */
        ,ET_HIPROC       = 0xffff                   /**< Processor-specific range end */
    };

    // Some structures are used to represent certain things whose layout is very precise in binary files, thus we need to
    // make sure the compiler doesn't insert alignment padding between the struct members.  ROSE can be compiled on an
    // architecture that has different alignment constraints than the architecture that these structs describe. GNU
    // compilers have long used the attribute mechanism. Microsoft compilers on the other hand use pragmas. GCC versions
    // 4.0 and earlier do not recognize the Microsoft pragmas and issue compiler errors when one is encountered.
#ifdef _MSC_VER
# pragma pack (1)
#endif

    /** File format of an ELF header.
     *
     *  Byte order of members depends on e_ident value. This code comes directly from "System V Application Binary
     *  Interface, Edition 4.1" and the FreeBSD elf(5) man page, and the "Executable and Linkable Format (ELF) Portable
     *  Formats Specifications, Version 1.2" and not from any header file. */
    struct Elf32FileHeader_disk {
        unsigned char       e_ident_magic[4];       /**< 0x7f, 'E', 'L', 'F' */
        unsigned char       e_ident_file_class;     /**< 1=>32-bit; 2=>64-bit; other is error */
        unsigned char       e_ident_data_encoding;  /**< 1=>LSB; 2=>MSB; other is error */
        unsigned char       e_ident_file_version;   /**< Format version number (same as e_version); must be 1 */
        unsigned char       e_ident_padding[9];     /**< Padding to byte 16; must be zero */
        uint16_t            e_type;                 /**< Object file type: relocatable, executable, lib, core */
        uint16_t            e_machine;              /**< Required architecture for an individual file */
        uint32_t            e_version;              /**< Object file version, currently zero or one */
        uint32_t            e_entry;                /**< Entry virtual address or zero if none */
        uint32_t            e_phoff;                /**< File offset of program header table or zero if none */
        uint32_t            e_shoff;                /**< File offset of section header table or zero if none */
        uint32_t            e_flags;                /**< Processor-specific flags (EF_* constants in docs) */
        uint16_t            e_ehsize;               /**< Size of ELF header in bytes */
        uint16_t            e_phentsize;            /**< Size of each entry in the program header table */
        uint16_t            e_phnum;                /**< Number of program headers, or PN_XNUM, or zero */
        uint16_t            e_shentsize;            /**< Size of each entry in the section header table */
        uint16_t            e_shnum;                /**< Number of section headers, or zero for extended entries */
        uint16_t            e_shstrndx;             /**< Index of name section, or SHN_UNDEF, or SHN_XINDEX */
    }
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

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
    }
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

#ifdef _MSC_VER
# pragma pack ()
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: File class.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned char e_ident_file_class = 0;

    /** Property: Data encoding.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned char e_ident_data_encoding = 0;

    /** Property: File version.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned char e_ident_file_version = 0;

    /** Property: Padding.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    SgUnsignedCharList e_ident_padding;

    /** Property: Type.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long e_type = 0;

    /** Property: Machine.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long e_machine = 0;

    /** Property: Flags.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long e_flags = 0;

    /** Property: ehsize.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long e_ehsize = 0;

    /** Property: phextrasz.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long phextrasz = 0;

    /** Property: phnum.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long e_phnum = 0;

    /** Property: shextrasz.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long shextrasz = 0;

    /** Property: shnum.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long e_shnum = 0;

    /** Property: shstrndx.
     *
     *  See ELF specification for details. */
    [[using Rosebud: rosetta]]
    unsigned long e_shstrndx = 0;

    /** Property: Section table.
     *
     *  Points to the AST node that represents the ELF section table that describes each section of the file. ELF sections
     *  are generally those parts of the file that are of interest to linkers, debuggers, etc. but not needed by the
     *  program loader. */
    [[using Rosebud: rosetta]]
    SgAsmElfSectionTable* sectionTable = nullptr;

    /** Property: Segment table.
     *
     *  Points to the AST node that represents the ELF segment table that describes each segment of the file. Segments
     *  describe how parts of the file are mapped into virtual memory by the loader. */
    [[using Rosebud: rosetta]]
    SgAsmElfSegmentTable* segmentTable = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Construct a new ELF File Header with default values.
     *
     *  The new section is placed at file offset zero and the size is initially one byte (calling @ref parse will extend it
     *  as necessary). Setting the initial size of non-parsed sections to a positive value works better when adding
     *  sections to the end-of-file since the sections will all have different starting offsets and therefore @ref
     *  SgAsmGenericFile::shiftExtend will know what order the sections should be in when they are eventually resized. */
    explicit SgAsmElfFileHeader(SgAsmGenericFile*);

    /** Maximum page size according to the ABI.
     *
     *  This is used by the loader when calculating the program base address. Since parts of the file are mapped into the
     *  process address space those parts must be aligned (both in the file and in memory) on the largest possible page
     *  boundary so that any smaller page boundary will also work correctly. */
    uint64_t maximumPageSize();

    /** Convert ELF "machine" identifier to generic instruction set architecture value. */
    static SgAsmExecutableFileFormat::InsSetArchitecture machineToIsa(unsigned machine);

    /** Convert architecture value to an ELF "machine" value. */
    unsigned isaToMachine(SgAsmExecutableFileFormat::InsSetArchitecture isa) const;

    /** Parse header from file.
     *
     *  Initialize this header with information parsed from the file and construct and parse everything that's reachable
     *  from the header. Since the size of the ELF File Header is determined by the contents of the ELF File Header as
     *  stored in the file, the size of the ELF File Header will be adjusted upward if necessary. */
    virtual SgAsmElfFileHeader *parse() override;

    virtual bool reallocate() override;

    /** Write ELF contents back to a file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    /** Return true if the file looks like it might be an ELF file according to the magic number. */
    static bool isElf(SgAsmGenericFile*);

    /** Get the list of sections defined in the ELF Section Table */
    SgAsmGenericSectionPtrList get_sectionTableSections();

    /** Get the list of sections defined in the ELF Segment Table */
    SgAsmGenericSectionPtrList get_segmentTableSections();

    // Overrides documented in base class
    virtual const char *formatName() const override;

private:
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfFileHeader::Elf32FileHeader_disk*) const;
    void *encode(Rose::BinaryAnalysis::ByteOrder::Endianness, SgAsmElfFileHeader::Elf64FileHeader_disk*) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmElfSectionTable* get_section_table() const ROSE_DEPRECATED("use get_sectionTable");
    void set_section_table(SgAsmElfSectionTable*) ROSE_DEPRECATED("use set_sectionTable");
    SgAsmElfSegmentTable* get_segment_table() const ROSE_DEPRECATED("use get_segmentTable");
    void set_segment_table(SgAsmElfSegmentTable*) ROSE_DEPRECATED("use set_segmentTable");
    uint64_t max_page_size() ROSE_DEPRECATED("use maximumPageSize");
    static SgAsmExecutableFileFormat::InsSetArchitecture machine_to_isa(unsigned) ROSE_DEPRECATED("use machineToIsa");
    unsigned isa_to_machine(SgAsmExecutableFileFormat::InsSetArchitecture) const ROSE_DEPRECATED("use isaToMachine");
    static bool is_ELF(SgAsmGenericFile*) ROSE_DEPRECATED("use isElf");
    SgAsmGenericSectionPtrList get_sectab_sections() ROSE_DEPRECATED("use get_sectionTableSections");
    SgAsmGenericSectionPtrList get_segtab_sections() ROSE_DEPRECATED("use get_segmentTableSections");
    virtual const char *format_name() const override ROSE_DEPRECATED("use formatName");
};
