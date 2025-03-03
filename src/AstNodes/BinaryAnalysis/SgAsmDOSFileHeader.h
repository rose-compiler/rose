#include <Rose/BinaryAnalysis/Address.h>

/** Represents the file header for DOS executables.
 *
 *  Many of the properties for this node come directly from the DOS specification and have similar names to that documentation. Such
 *  properties are not documented in ROSE. */
class SgAsmDOSFileHeader: public SgAsmGenericHeader {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /** On-disk format of DOS file header. */
    struct DOSFileHeader_disk {
        unsigned char e_magic[2];                /**< 0x00 "MZ" */
        uint16_t      e_last_page_size;          /**< 0x02 bytes used on last page of file (1 page == 512 bytes);
                                                  *        zero implies if last page is full. */ //
        uint16_t      e_total_pages;             /**< 0x04 number of pages (inc. last possibly partial page) in file. */
        uint16_t      e_nrelocs;                 /**< 0x06 number of relocation entries stored after this header. */
        uint16_t      e_header_paragraphs;       /**< 0x08 header size in paragraphs (16-byte blocks) inc. relocations. */
        uint16_t      e_minalloc;                /**< 0x0a number of extra paragraphs needed, similar to BSS in Unix. */
        uint16_t      e_maxalloc;                /**< 0x0c max paragraphs to allocate for BSS. */
        uint16_t      e_ss;                      /**< 0x0e initial value of SS register relative to program load segment. */
        uint16_t      e_sp;                      /**< 0x10 initial value for SP register. */
        uint16_t      e_cksum;                   /**< 0x12 checksum; 16-bit sum of all words in file should be zero
                                                  *        (usually not filled in). */
        uint16_t      e_ip;                      /**< 0x14 initial value for IP register. */
        uint16_t      e_cs;                      /**< 0x16 initial value for CS register relative to program load segment. */
        uint16_t      e_relocs_offset;           /**< 0x18 file address of relocation table. */
        uint16_t      e_overlay;                 /**< 0x1a overlay number (zero indicates main program). */
        uint32_t      e_res1;                    /**< 0x1c unknown purpose. */
    }                                            /* 0x20 */
#if !defined(SWIG) && !defined(_MSC_VER)
    __attribute__((packed))
#endif
    ;

    struct DOSRelocEntry_disk {
        uint16_t      offset;                       /* 0x00 */
        uint16_t      segment;                      /* 0x02 */
    }                                               /* 0x04 */
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
    /** Property: Last page size.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_last_page_size = 0;

    /** Property: Total number of pages.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_total_pages = 0;

    /** Property: Number of relocations.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_nrelocs = 0;

    /** Property: Number of header paragraphs.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_header_paragraphs = 0;

    /** Property: Min alloc.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_minalloc = 0;

    /** Property: Max alloc.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_maxalloc = 0;

    /** Property: SS.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_ss = 0;

    /** Property: SP.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_sp = 0;

    /** Property: Checksum.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_cksum = 0;

    /** Property: IP.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_ip = 0;

    /** Property: CS.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_cs = 0;

    /** Property: Overlay.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    uint16_t e_overlay = 0;

    /** Property: Relocs offset.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address e_relocs_offset = 0;

    /** Property: Reserved field 1.
     *
     *  See DOS specification. */
    [[using Rosebud: rosetta]]
    unsigned e_res1 = 0;

    /** Property: Section containing relocation information. */
    [[using Rosebud: rosetta]]
    SgAsmGenericSection* relocs = nullptr;

    /** Property: Real mode section. */
    [[using Rosebud: rosetta]]
    SgAsmGenericSection* rm_section = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    explicit SgAsmDOSFileHeader(SgAsmGenericFile*);
    virtual SgAsmDOSFileHeader *parse() override {return parse(true);}
    SgAsmDOSFileHeader *parse(bool define_rm_section);
    virtual bool reallocate() override;
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
    virtual const char *formatName() const override;

    /** Parses the DOS real-mode text+data section and adds it to the AST.
     *
     *  If max_offset is non-zero then use that as the maximum offset of the real-mode section. If the DOS header indicates a zero
     *  sized section then return NULL. If the section exists or is zero size due to the max_offset then return the section. See
     *  also, update_from_rm_section(). */
    SgAsmGenericSection *parseRealModeSection(Rose::BinaryAnalysis::Address max_offset=0);

    /** Update DOS header with data from real-mode section.
     *
     *  The DOS real-mode data+text section is assumed to appear immediately after the DOS Extended Header, which appears
     *  immediately after the DOS File Header, which appears at the beginning of the file. These assumptions are not checked until
     *  SgAsmDOSFileHeader::unparse() is called. See also, @ref parseRealModeSection. */
    void updateFromRealModeSection();

    /** Returns true if a cursory look at the file indicates that it could be a DOS executable file. */
    static bool isDos(SgAsmGenericFile*);

private:
    void *encode(SgAsmDOSFileHeader::DOSFileHeader_disk*) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual const char* format_name() const override ROSE_DEPRECATED("use formatName");
    SgAsmGenericSection* parse_rm_section(Rose::BinaryAnalysis::Address max_offset = 0) ROSE_DEPRECATED("use parseRealModeSection");
    void update_from_rm_section() ROSE_DEPRECATED("use updateFromRealModeSection");
    static bool is_DOS(SgAsmGenericFile*) ROSE_DEPRECATED("use isDos");
};
