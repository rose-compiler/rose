#include <Rose/BinaryAnalysis/Address.h>

/** DOS exteded header.
 *
 *  Most of the properties correspond to those defined in the PE/DOS specifications and that documentation is not repeated here. */
class SgAsmDOSExtendedHeader: public SgAsmGenericSection {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    struct DOSExtendedHeader_disk {
        uint32_t      e_res1;     /* 0x00 reserved */
        uint16_t      e_oemid;    /* 0x04 OEM Identifier */
        uint16_t      e_oeminfo;  /* 0x06 other OEM information; oemid specific */
        uint32_t      e_res2;     /* 0x08 reserved */
        uint32_t      e_res3;     /* 0x0c reserved */
        uint32_t      e_res4;     /* 0x10 reserved */
        uint32_t      e_res5;     /* 0x14 reserved */
        uint32_t      e_res6;     /* 0x18 reserved */
        uint32_t      e_lfanew;   /* 0x1c file offset of new exe (PE) header */
    }                                 /* 0x20 */
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
    /** Property: Rerserved area 1.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_res1 = 0;

    /** Property: OEM ID.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_oemid = 0;

    /** Property: OEM info.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_oeminfo = 0;

    /** Property: Rerserved area 2.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_res2 = 0;

    /** Property: Rerserved area 3.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_res3 = 0;

    /** Property: Rerserved area 4.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_res4 = 0;

    /** Property: Rerserved area 5.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_res5 = 0;

    /** Property: Rerserved area 6.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    unsigned e_res6 = 0;

    /** Property: lfanew.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address e_lfanew = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    explicit SgAsmDOSExtendedHeader(SgAsmDOSFileHeader*);
    virtual SgAsmDOSExtendedHeader *parse() override;
    void *encode(SgAsmDOSExtendedHeader::DOSExtendedHeader_disk*) const;
    virtual void unparse(std::ostream&) const override;
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
