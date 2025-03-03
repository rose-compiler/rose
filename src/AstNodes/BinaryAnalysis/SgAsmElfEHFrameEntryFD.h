#include <Rose/BinaryAnalysis/Address.h>
#include <Rose/BinaryAnalysis/RelativeVirtualAddress.h>
#include <sageContainer.h>

/** ELF error handling frame entry frame description entry. */
class SgAsmElfEHFrameEntryFD: public SgAsmExecutableFileFormat {
public:
    /** Property: Beginning relative virtual address.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta, large]]
    Rose::BinaryAnalysis::RelativeVirtualAddress begin_rva;

    /** Property: Size in bytes.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address size = 0;

    /** Property: Augmentation data.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList augmentation_data;

    /** Property: Instructions.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList instructions;

public:
    /** Non-parsing constructor */
    explicit SgAsmElfEHFrameEntryFD(SgAsmElfEHFrameEntryCI*);

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    /** Unparse to string.
     *
     *  Unparses the frame description entry (FDE) into a string but do not include the leading length field(s) or the CIE
     *  back pointer. */
    std::string unparse(const SgAsmElfEHFrameSection*, SgAsmElfEHFrameEntryCI*) const;
};
