#include <Rose/BinaryAnalysis/Address.h>

class SgAsmNERelocEntry: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /* NERelocEntry_disk -- variable size with multiple levels of 'union'. It's easier to just parse it in NERelocEntry::ctor()
     * than defining it here as a struct. */
    enum NERelocSrcType {
        RF_SRCTYPE_8OFF     = 0,            /* Byte offset */
        RF_SRCTYPE_WORDSEG  = 2,            /* Word segment, 16-bit selector */
        RF_SRCTYPE_16PTR    = 3,            /* 16-bit far pointer */
        RF_SRCTYPE_16OFF    = 5,            /* 16-bit offset */
        RF_SRCTYPE_32PTR    = 6,            /* 32-bit far pointer */
        RF_SRCTYPE_32OFF    = 7,            /* 32-bit offset */
        RF_SRCTYPE_NEARCALL = 8,            /* near call or jump, WORD/DWROD based on section attribute */
        RF_SRCTYPE_48PTR    = 11,           /* 48-bit pointer */
        RF_SRCTYPE_32OFF_b  = 13            /* 32-bit offset (not sure how this differs from case 7) */
    };

    enum NERelocTgtType {
        RF_TGTTYPE_IREF     = 0,            /* Internal reference */
        RF_TGTTYPE_IORD     = 1,            /* Imported (extern) ordinal */
        RF_TGTTYPE_INAME    = 2,            /* Imported (extern) name */
        RF_TGTTYPE_OSFIXUP  = 3             /* Operating system fixup */
    };

    enum NERelocModifiers {
        RF_MODIFIER_SINGLE  = 1,
        RF_MODIFIER_MULTI   = 3
    };

    enum NERelocFlags {
        RF_ADDITIVE         = 0x01,         /* add target to source rather than replace source with target */
        RF_RESERVED         = 0x02,         /* reserved bits */
        RF_2EXTRA           = 0x04,         /* relocation info has size with new two bytes at end */
        RF_32ADD            = 0x08,         /* addition with 32-bits rather than 16 */
        RF_16SECTION        = 0x10,         /* 16-bit object number & module name rather than 8-bit */
        RF_8ORDINAL         = 0x20          /* Ordinal is 8-bits rather than 16 */
    };

    // DQ (8/7/2008): At only (I hope) the risk of using more memory that required, break the union so that we can better support
    // this in ROSETTA. One solution might be to implement a common base class of unsigned, unsigned, Rose::BinaryAnalysis::Address;
    // and then use member functions to access the data in the base class.
    struct iref_type { /*tgt_type==0x00: internal reference*/
        unsigned    sect_idx;       /* section index (1-origin) */
        unsigned    res1;           /* reserved */
        Rose::BinaryAnalysis::Address tgt_offset;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(sect_idx);
            s & BOOST_SERIALIZATION_NVP(res1);
            s & BOOST_SERIALIZATION_NVP(tgt_offset);
        }
#endif

        // Added to support RTI support in ROSE
        friend std::ostream & operator<< ( std::ostream & os, const iref_type & x );

        iref_type();
    };

    struct iord_type { /*tgt_type==0x01: imported ordinal*/
        unsigned modref;                                /* 1-based index into import module table */
        unsigned ordinal;
        Rose::BinaryAnalysis::Address addend;           /* value to add (only present for flags & RF_2EXTRA) */

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(modref);
            s & BOOST_SERIALIZATION_NVP(ordinal);
            s & BOOST_SERIALIZATION_NVP(addend);
        }
#endif

        // Added to support RTI support in ROSE
        friend std::ostream & operator<< ( std::ostream & os, const iord_type & x );

        iord_type();
    };

    struct iname_type { /*tgt_type==0x02: imported name*/
        unsigned modref;                                /* 1-based index into import module table */
        unsigned nm_off;                                /* offset into import procedure names */
        Rose::BinaryAnalysis::Address addend;           /* value to add (only present for flags & RF_2EXTRA) */

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(modref);
            s & BOOST_SERIALIZATION_NVP(nm_off);
            s & BOOST_SERIALIZATION_NVP(addend);
        }
#endif

        // Added to support RTI support in ROSE
        friend std::ostream & operator<< ( std::ostream & os, const iname_type & x );

        iname_type();
    };

    struct osfixup_type { /*tgt_type==0x03: operating system fixup*/
        unsigned    type;
        unsigned    res3;

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(type);
            s & BOOST_SERIALIZATION_NVP(res3);
        }
#endif

        // Added to support RTI support in ROSE
        friend std::ostream & operator<< ( std::ostream & os, const osfixup_type & x );

        osfixup_type();
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    [[using Rosebud: rosetta]]
    SgAsmNERelocEntry::NERelocSrcType src_type = SgAsmNERelocEntry::RF_SRCTYPE_8OFF;

    [[using Rosebud: rosetta]]
    SgAsmNERelocEntry::NERelocModifiers modifier = SgAsmNERelocEntry::RF_MODIFIER_SINGLE;

    [[using Rosebud: rosetta]]
    SgAsmNERelocEntry::NERelocTgtType tgt_type = SgAsmNERelocEntry::RF_TGTTYPE_IREF;

    [[using Rosebud: rosetta]]
    SgAsmNERelocEntry::NERelocFlags flags = SgAsmNERelocEntry::RF_ADDITIVE;

    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address src_offset = 0;

    [[using Rosebud: rosetta]]
    SgAsmNERelocEntry::iref_type iref;

    [[using Rosebud: rosetta]]
    SgAsmNERelocEntry::iord_type iord;

    [[using Rosebud: rosetta]]
    SgAsmNERelocEntry::iname_type iname;

    [[using Rosebud: rosetta]]
    SgAsmNERelocEntry::osfixup_type osfixup;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmNERelocEntry(SgAsmGenericSection *relocs, Rose::BinaryAnalysis::Address at, Rose::BinaryAnalysis::Address *rec_size);
    Rose::BinaryAnalysis::Address unparse(std::ostream&, const SgAsmGenericSection*, Rose::BinaryAnalysis::Address spos) const;
    void dump(FILE*, const char *prefix, ssize_t idx) const;
};
