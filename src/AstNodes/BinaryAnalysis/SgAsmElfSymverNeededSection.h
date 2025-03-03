#include <Rose/BinaryAnalysis/Address.h>

#ifdef ROSE_IMPL
#include <SgAsmElfSymverNeededEntryList.h>
#endif

/** GNU symbol version requirements table.
 *
 *  This section is usually named ".gnu.version_r".  The format of this table is similar to the GNU Symbol Version
 *  Definitions Table, namey that the table object (@ref SgAsmElfSymverNeededSection) points to a list of entries (@ref
 *  SgAsmElfSymverNeededEntry), which of which point to a list of auxilliary information (@ref SgAsmElfSymverNeededAux). */
class SgAsmElfSymverNeededSection: public SgAsmElfSection {
public:
    /** Property: List of entries.
     *
     *  This property points to an AST node whose only purpose is to hold the list of entries. The only reason the entries
     *  are not contained directly in this node where they're needed is due to ROSETTA limitations. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfSymverNeededEntryList* entries = createAndParent<SgAsmElfSymverNeededEntryList>(this);

public:
    /** Constructor that links objec into AST.
     *
     *  This constructor adds this entry to the specified GNU Symbol Version Requirements Table. */
    SgAsmElfSymverNeededSection(SgAsmElfFileHeader *fhdr, SgAsmElfStringSection *strsec);

    /** Parse a GNU Symbol Version Requirements Table.
     *
     *  The layout of this table is very similar to the layout of the GNU Symbol Version Definition Table and users should
     *  refer to @ref SgAsmElfSymverDefinedSection::parse for details.  Different data structures are used between the
     *  Definition and Requirements tables:
     *
     *  @li @ref SgAsmElfSymverNeededSection corresponds to SgAsmElfSymverDefinedSection.
     *  @li @ref SgAsmElfSymverNeededEntry corresponds to SgAsmElfSymverDefinedEntry.
     *  @li @ref SgAsmElfSymverNeededAux corresponds to SgAsmElfSymverDefinedAux. */
    virtual SgAsmElfSymverNeededSection* parse() override;

    using SgAsmElfSection::calculate_sizes;
    /** Return sizes for various parts of the table.
     *
     *  See documentation for @ref SgAsmElfSection::calculateSizes. */
    virtual Rose::BinaryAnalysis::Address
    calculateSizes(size_t *total, size_t *required, size_t *optional, size_t *nentries) const override;

    /** Write SymverNeeded section back to disk.
     *
     *  For more information about encoding, see @ref SgAsmElfSymverNeededSection::parse. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    virtual Rose::BinaryAnalysis::Address calculate_sizes(size_t*, size_t*, size_t*, size_t*) const override
        ROSE_DEPRECATED("use calculateSizes");
};
