/** Represents an JVM constant pool.
 *
 *  The JVM Constant Pool is itself a section.  The entries of the table are stored with the section they describe rather
 *  than storing them all in the SgAsmSectionTable node.  We can reconstruct the JVM Section Table since sections have
 *  unique ID numbers that are their original indices in the JVM Section Table. */
class SgAsmJvmConstantPool: public SgAsmGenericSection {
    /** Property: List of constant pool entries. */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmConstantPoolEntryPtrList entries;

public:
    /** Non-parsing constructor. */
    explicit SgAsmJvmConstantPool(SgAsmJvmFileHeader*);

    /** Parses a JVM Constant Pool.
     *
     *  Parses a JVM constant pool and constructs and parses all sections reachable from the table. The section is
     *  extended as necessary based on the number of entries and the size of each entry. Returns a pointer to this
     *  object. */
    virtual SgAsmJvmConstantPool* parse() override;

    /** Returns constant pool entry at given index.
     *
     *  Index starts at one.
     * @{ */
    SgAsmJvmConstantPoolEntry* get_entry(size_t index) const;
    /** @} */

    /** Return a string at the given index.
     *
     *  Throws an error if the index is invalid or if the constant pool entry
     *  at the index is not a CONSTANT_Utf8_info.
     */
    std::string get_utf8_string(size_t index) const;

    /** Write constant pool to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

protected:
    // Some extra clean-up actions on destruction
    virtual void destructorHelper() override;
};
