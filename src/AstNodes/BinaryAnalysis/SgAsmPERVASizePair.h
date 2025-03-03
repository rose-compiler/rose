#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>

/** RVA/size pair. */
class SgAsmPERVASizePair: public SgAsmExecutableFileFormat {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
    /** File format for an RVA/Size pair.
     *
     *  Such pairs are considered to be part of the PE file header.  All fields are little endian. */
    struct RVASizePair_disk {
        uint32_t    e_rva;
        uint32_t    e_size;
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
    /** Property: RVA.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress e_rva;

    /** Property: Size.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address e_size = 0;

    /** Property: Section.
     *
     *  See PE specification. */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    SgAsmGenericSection* section = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    SgAsmPERVASizePair(SgAsmPERVASizePairList *parent, const SgAsmPERVASizePair::RVASizePair_disk*);
    SgAsmPERVASizePair(SgAsmPERVASizePairList *parent, Rose::BinaryAnalysis::Address rva, Rose::BinaryAnalysis::Address size);

    /** Sets or removes the section associated with an RVA/size pair.
     *
     *  Setting or removing the section also updates the RVA and size according to the preferred mapping address and mapped
     *  size of the section. */
    void set_section(SgAsmGenericSection *section);

    /** Returns the section associated with an RVA/size pair.
     *
     *  This is the same as the ROSETTA-generated accessor, but we need a custom version of set_section(). */
    SgAsmGenericSection *get_section() const;

    void *encode(SgAsmPERVASizePair::RVASizePair_disk *disk) const;
};
