/** Base class for strings related to binary specimens. */
class SgAsmGenericString: public SgAsmExecutableFileFormat {
public:
    /** Constant for addresses of unallocated strings. */
    static const rose_addr_t unallocated;

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
    virtual void set_string(rose_addr_t);
    /** @} */

    virtual rose_addr_t get_offset() const {return unallocated;}

    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;
};
