/** Base class for JVM branch of binary analysis IR nodes. */
[[Rosebud::abstract]]
class SgAsmJvmNode: public SgAsmNode {
public:
    /** Write node to a binary file. */
    virtual void unparse(std::ostream&) const;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;
};
