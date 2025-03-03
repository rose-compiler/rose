#include <Rose/BinaryAnalysis/Address.h>

/** String associated with a binary file.
 *
 *  Basic strings need not be stored in the binary specimen; they can be generated on the fly by the parser. See also, @ref
 *  SgAsmStoredString, which is present in the binary specimen. */
class SgAsmBasicString: public SgAsmGenericString {
public:
    [[using Rosebud: rosetta, accessors(), mutators()]]
    std::string string;
    virtual std::string get_string(bool escape=false) const override;
    virtual void set_string(const std::string&) override;
    virtual void set_string(Rose::BinaryAnalysis::Address) override;

public:
    /** Constructor that gives a value to the object.
     *
     * @{ */
    explicit SgAsmBasicString(const std::string&);
    explicit SgAsmBasicString(const char*);
    /** @} */

    // Overrides documented in base class
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
