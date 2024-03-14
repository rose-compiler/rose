#ifdef ROSE_IMPL
#include <SgAsmElfSymverDefinedEntryList.h>
#endif

/** The GNU symbol version definitions.
 *
 *  This section is usually named ".gnu.version_d".  The @ref SgAsmElfSymverDefinedSection points to a list of entries (@ref
 *  SgAsmElfSymverDefinedEntry), which in turn point to a list of auxilliary members (@ref SgAsmElfSymverDefinedAux). See @ref
 *  SgAsmElfSymverDefinedSection::parse for a good description of the disk format. */
class SgAsmElfSymverDefinedSection: public SgAsmElfSection {
public:
    /** Property: List of entries.
     *
     *  This is a pointer to an AST node which contains the list. The reason the list isn't stored here directly is due to
     *  limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfSymverDefinedEntryList* entries = createAndParent<SgAsmElfSymverDefinedEntryList>(this);

public:
    /** Constructor linking object into AST. */
    SgAsmElfSymverDefinedSection(SgAsmElfFileHeader *fhdr, SgAsmElfStringSection *strsec);

    /** Initializes this ELF SymverDefined Section by parsing a file.
     *
     *  The structure is nominally the following (where n is from DT_VERDEFNUM - 1 in .dynamic)
     *
     *  @code
     *   [0]ElfSymverDefinedEntry_disk
     *        vd_next------------------------------------+
     *        vd_aux ---------------------------------+  |
     *        vd_cnt                                  |  |
     *      [0]       ElfSymverDefinedAux_disk <------+  |
     *                  vda_next -----------------+      |
     *      [1]       ElfSymverDefinedAux_disk <--+      |
     *         ...                                       |
     *      [vd_cnt-1]ElfSymverDefinedAux_disk           |
     *                  vda_next = 0 <== null term       |
     *   [1]ElfSymverDefinedEntry_disk <-----------------+
     *      ...
     *   [n]ElfSymverDefinedEntry_disk
     *        vd_next = 0
     *        vd_aux
     *        vd_cnt
     *      [0]       ElfSymverDefinedAux_disk
     *      [1]       ElfSymverDefinedAux_disk
     *         ...
     *      [vd_cnt-1]ElfSymverDefinedAux_disk
     *  @endcode
     *
     *  However, the spec doesn't specify any actual relationship to the layout of anything...so it could just as easily
     *  be:
     *
     *  @code
     *    [0]ElfSymverDefinedEntry_disk ---+---+
     *    [1]ElfSymverDefinedEntry_disk <--+   |
     *    ...                                  |
     *    [n]ElfSymverDefinedEntry_disk -------|---+
     *                                         |   |
     *    [0]ElfSymverDefinedAux_disk   <------+   |
     *    ...                                      |
     *    [x]ElfSymverDefinedAux_disk   <----------+
     *    [.]ElfSymverDefinedAux_disk
     *  @endcode
     *
     *  There is also nothing in particular that says Aux entries need to be next to each other.  So, the code handles the
     *  most rigidly compliant case, which is to use only the offsets and make no assumptions about layouts.
     *
     *  Also note the number of entries is specified in two ways -- via null termination on the "linked list", as well as
     *  the number from the .dynamic section [DT_VERDEFNUM].  For now, we'll support the null terminator, restricted by
     *  ensuring we don't exceed the size of the section (to keep from running away on a bad file).
     *
     *  We have a similar problem with the number of Aux's per Entry (vd_cnt versus vda_aux=0). However, in this case, we
     *  respect the min of the two (i.e. we assume cnt is right, but if vda_aux is zero earlier than expected, we stop).
     *  This is necessary because the spec allows two or more entries to point into (possibly different places) of a shared
     *  aux array.  This parser creates a new @ref SgAsmElfSymverDefinedAux object every time an element of the aux array
     *  is read from disk, ensuring that each @ref SgAsmElfSymverDefinedEntry points to its own copies.
     *
     *  All offsets are relative to the start of the struct they were specified in. I.e.,
     *
     *  @code
     *    Entry* entry=(0x0100);
     *    Aux* firstAux=(0x100 + entry->vd_aux)
     *    Aux* secondAux=(0x100 + entry->vd_aux + firstAux->vda_next)
     *
     *    Entry* secondEntry=(0x0100 + entry->vd_next);
     *  @endcode
     *
     *  Because of this rather complex structure, the section itself (@ref SgAsmElfSymverDefinedSection) manages all of the
     *  data related to structure (vd_next,vd_aux,vd_cnt, vda_next) -- the subclasses can simply ignore all of that.  The
     *  section also takes care of creating both *Entries and *Auxes and tying them together correctly. */
    virtual SgAsmElfSymverDefinedSection* parse() override;

    using SgAsmElfSection::calculateSizes;
    /** Return sizes for various parts of the table. See doc for SgAsmElfSection::calculateSizes. */
    virtual rose_addr_t calculateSizes(size_t *total, size_t *required, size_t *optional, size_t *nentries) const override;

    /** Write SymverDefined section back to disk.
     *
     *  For more information about encoding, see @ref parse. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging info. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    using SgAsmElfSection::calculate_sizes;
    virtual rose_addr_t calculate_sizes(size_t*, size_t*, size_t*, size_t*) const override ROSE_DEPRECATED("use calculateSizes");
};
