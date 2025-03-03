#include <Rose/BinaryAnalysis/Address.h>

/** Strings stored in an ELF or PE container. */
class SgAsmStringStorage: public SgAsmExecutableFileFormat {
public:
    /** Property: String table holding the string. */
    [[using Rosebud: rosetta]]
    SgAsmGenericStrtab* strtab = nullptr;

    /** Property: String value. */
    [[using Rosebud: rosetta]]
    std::string string;

    /** Property: Location of string in storage table. */
    [[using Rosebud: rosetta]]
    Rose::BinaryAnalysis::Address offset = 0;

public:
    SgAsmStringStorage(SgAsmGenericStrtab *strtab, const std::string &string, Rose::BinaryAnalysis::Address offset);

    void dump(FILE *s, const char *prefix, ssize_t idx) const;

    /* Accessors. The set_* accessors are private because we don't want anyone messing with them. These data members are
     * used to control string allocation in ELF string tables and must only be modified by allocators in closely related
     * classes.  For instance, to change the value of the string one should call SgAsmGenericString::set_string()
     * instead. */

 private:
      friend class SgAsmStoredString;                     /*allowed to set private data members*/
      friend class SgAsmStoredStrtab;                     /*allowed to set private data members*/
};
