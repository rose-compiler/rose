#include <Rose/BinaryAnalysis/Address.h>

/** ELF string table section.
 *
 *  A file section that holds string literals such as symbol names. */
class SgAsmElfStringSection: public SgAsmElfSection {
public:
    /** Property: String table.
     *
     *  Pointer to the actual string table for this section. */
    [[using Rosebud: rosetta]]
    SgAsmElfStrtab* strtab = nullptr;

public:
    /** Constructor that links new object into AST. */
    explicit SgAsmElfStringSection(SgAsmElfFileHeader*);

    /** Initialize object by parsing binary specimen. */
    virtual SgAsmElfStringSection *parse() override;

    /** Dump debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    /** Adjust size of table.
     *
     * Augments superclass to make sure free list and such are adjusted properly. Any time the ELF String Section size is
     * changed we adjust the free list in the ELF String Table contained in this section. */
    virtual void set_size(Rose::BinaryAnalysis::Address newsize) override;

    /** Reallocate space for the string section if necessary.
     *
     *  Note that reallocation is lazy here -- we don't shrink the section, we only enlarge it (if you want the section to
     *  shrink then call SgAsmGenericStrtab::reallocate(bool) with a true value rather than calling this
     *  function. SgAsmElfStringSection::reallocate is called in response to unparsing a file and gives the string table a
     *  chance to extend its container section if it needs to allocate more space for strings. */
    virtual bool reallocate() override;

    /** Unparse an ElfStringSection by unparsing the ElfStrtab */
    virtual void unparse(std::ostream&) const override;
};
