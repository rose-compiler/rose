#include <Rose/BinaryAnalysis/Address.h>

/** Base class for strings related to binary specimens. */
[[Rosebud::abstract]]
class SgAsmGenericString: public SgAsmExecutableFileFormat {
public:
    /** Constant for addresses of unallocated strings. */
    static const Rose::BinaryAnalysis::Address unallocated;

    /** Property: String value.
     *
     *  When retrieving the string, if @p escape is true then escape special charactes like the would be in C source code.
     *
     *  The base class implementation cannot be called and exists only due to ROSETTA limitations, otherwise we would have
     *  made them pure virtual.
     *
     * @{ */
    virtual std::string get_string(bool escape=false) const;
    virtual void set_string(const std::string &s);
    virtual void set_string(Rose::BinaryAnalysis::Address);
    /** @} */

    virtual Rose::BinaryAnalysis::Address get_offset() const {return unallocated;}

    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;
};
