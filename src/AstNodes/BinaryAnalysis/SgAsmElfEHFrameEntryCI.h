#include <Rose/BinaryAnalysis/Address.h>

#include <sageContainer.h>

#ifdef ROSE_IMPL
#include <SgAsmElfEHFrameEntryFDList.h>
#endif

/** ELF error handling frame entry, common information entry.
 *
 *  Most of the properties of this class are documented in the official ELF specification. */
class SgAsmElfEHFrameEntryCI: public SgAsmExecutableFileFormat {
public:
    /** Property: Version number.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    int version = 0;

    /** Property: Augmentation string.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    std::string augmentation_string;

    /** Property: Data value.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    uint64_t eh_data =0;

    /** Property: Code alignment factor.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    uint64_t code_alignment_factor = 0;

    /** Property: Data alignment factor.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    int64_t data_alignment_factor = 0;

    /** Property: Augmentation data length.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    uint64_t augmentation_data_length = 0;

    /** Property: LSDA encoding.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    int lsda_encoding = -1;

    /** Property: PRH encoding.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    int prh_encoding = -1;

    /** Property: PRH argument.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    unsigned prh_arg = 0;

    /** Property: PRH address.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address prh_addr = 0;

    /** Property: Address encoding.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    int addr_encoding = -1;

    /** Property: Signal frame.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta]]
    bool sig_frame = false;

    /** Property: Instructions.
     *
     *  See official ELF specification. */
    [[using Rosebud: rosetta, large]]
    SgUnsignedCharList instructions;

    /** Property: FD entries.
     *
     *  See official ELF specification. This points to an AST node that holds the list of pointers to the actual entry
     *  nodes due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfEHFrameEntryFDList* fd_entries = createAndParent<SgAsmElfEHFrameEntryFDList>(this);

public:
    /** Non-parsing constructor */
    explicit SgAsmElfEHFrameEntryCI(SgAsmElfEHFrameSection *ehframe);

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    /** Unparse an entry.
     *
     *  Unparse one Common Information Entry (CIE) without unparsing the Frame Description Entries (FDE) to which it
     *  points. The initial length fields are not included in the result string. */
    std::string unparse(const SgAsmElfEHFrameSection*) const;
};
