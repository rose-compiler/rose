/** Base class for JVM branch of binary analysis IR nodes. */
[[Rosebud::abstract]]
class SgAsmJvmNode: public SgAsmNode {
public:
    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const;
};
