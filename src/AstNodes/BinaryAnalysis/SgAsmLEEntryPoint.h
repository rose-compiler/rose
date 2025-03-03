#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/ByteOrder.h>
#include <sageContainer.h>

class SgAsmLEEntryPoint: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /* If 0x01 bit of "flags" is clear then the remainder (next 9 bytes) of the entry point is not stored in the file and the
     * next entry point description follows immediately after the flag. */
    struct LEEntryPoint_disk {
        uint8_t     flags;          /* 0x00 Bit flags (0x01=>non-empty bundle; 0x02=>32-bit entry*/
        uint16_t    objnum;         /* 0x01 Object number */
        uint8_t     entry_type;     /* 0x03 Flags for entry type */
        uint32_t    entry_offset;   /* 0x04 Offset of entry point */
        uint16_t    res1;           /* 0x08 Reserved */
    }                               /* 0x0a */
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    [[using Rosebud: rosetta, traverse, large]]
    SgAsmLEEntryPointPtrList entries;

    [[using Rosebud: rosetta]]
    unsigned flags = 0;

    [[using Rosebud: rosetta]]
    unsigned objnum = 0;

    [[using Rosebud: rosetta]]
    unsigned entry_type = 0;

    [[using Rosebud: rosetta]]
    unsigned res1 = 0;

    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address entry_offset = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmLEEntryPoint(Rose::BinaryAnalysis::ByteOrder::Endianness sex, const SgAsmLEEntryPoint::LEEntryPoint_disk *disk);
    SgAsmLEEntryPoint(Rose::BinaryAnalysis::ByteOrder::Endianness sex, unsigned flags);
    Rose::BinaryAnalysis::Address unparse(std::ostream&, Rose::BinaryAnalysis::ByteOrder::Endianness, const SgAsmGenericSection*,
                                          Rose::BinaryAnalysis::Address spos) const;
    void dump(FILE*, const char *prefix, ssize_t idx) const;
};
