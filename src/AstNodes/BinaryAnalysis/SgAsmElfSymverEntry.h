/** Entry in an ELF symbol version table. */
class SgAsmElfSymverEntry: public SgAsmExecutableFileFormat {
    /** Property: Value.
     *
     *  Numeric value of this symbol. */
    [[using Rosebud: rosetta]]
    size_t value = 0;

public:
    /** Constructor that links new entry into the AST. */
    explicit SgAsmElfSymverEntry(SgAsmElfSymverSection*);

    /** Prints some debugging info. */
    virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;
};
