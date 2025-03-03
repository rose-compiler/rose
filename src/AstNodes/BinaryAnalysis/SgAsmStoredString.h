#include <Rose/BinaryAnalysis/Address.h>

/** Strings stored in an ELF or PE container. */
class SgAsmStoredString: public SgAsmGenericString {
public:
    /** Property: Storage area for this string. */
    [[using Rosebud: rosetta]]
    SgAsmStringStorage* storage = nullptr;

public:
    /** Construct a string existing in a string table. */
    SgAsmStoredString(SgAsmGenericStrtab*, Rose::BinaryAnalysis::Address offset);

    /** Construct a new string in a string table. */
    SgAsmStoredString(SgAsmGenericStrtab*, const std::string&);

    /** Construct a string that shares storage with another. */
    explicit SgAsmStoredString(class SgAsmStringStorage*);

    /** Print some debugging info */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;

    /** Returns the string table that holds this string.
     *
     *  Returns the table even if the string value isn't currently allocated in the table. */
    SgAsmGenericStrtab *get_strtab();

    /** Returns the std::string associated with the SgAsmStoredString. */
    virtual std::string get_string(bool escape=false) const override;

    /** Give the string a new value.
     *
     *  This also deallocates the previous value. */
    virtual void set_string(const std::string&) override;

    /** Give the string a new value.
     *
     *  The string is given a new value by specifying the offset of a string that already exists in the string table. */
    virtual void set_string(Rose::BinaryAnalysis::Address) override;

    /** Returns the offset into the string table where the string is allocated.
     *
     *  If the string is not allocated then this call triggers an allocation. */
    virtual Rose::BinaryAnalysis::Address get_offset() const override;
};
