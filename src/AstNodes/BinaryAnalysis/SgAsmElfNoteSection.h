#ifdef ROSE_IMPL
#include <SgAsmElfNoteEntryList.h>
#endif

class SgAsmElfNoteSection: public SgAsmElfSection {
public:
    /** Property: List of note entries.
     *
     *  This property points to an AST node that contains the list rather than being a list directly because of limitations
     *  of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmElfNoteEntryList* entries = createAndParent<SgAsmElfNoteEntryList>(this);

public:
    /** Non-parsing constructor */
    explicit SgAsmElfNoteSection(SgAsmElfFileHeader*);

    virtual SgAsmElfNoteSection *parse() override;

    virtual bool reallocate() override;

    /** Write data to note section */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
