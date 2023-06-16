/** A JVM InnerClasses attribute array entry.
 *
 *  See the JVM specification (section 4.7.6) for more information.
 */
class SgAsmJvmInnerClassesEntry: public SgAsmJvmNode {
    /** Property: inner_class_info_index.
     *
     *  The value of the inner_class_info_index must be a valid index into the constant_pool table (see the JVM specification
     *  section 4.7.6). */
    [[using Rosebud: rosetta]]
    uint16_t inner_class_info_index = 0;

    /** Property: outer_class_info_index
     *
     *  The value of the outer_class_info_index must be a valid index into the constant_pool table (see the JVM specification
     *  section 4.7.6). */
    [[using Rosebud: rosetta]]
    uint16_t outer_class_info_index = 0;

    /** Property: inner_name_index
     *
     *  The value of the inner_name_index must be a valid index into the constant_pool table (see the JVM specification section
     *  4.7.6). */
    [[using Rosebud: rosetta]]
    uint16_t inner_name_index = 0;

    /** Property: inner_class_access_flags
     *
     *  The value of the inner_class_access_flags item is a mask of flags used to denote access permissions to and properties of
     *  class or interface C as declared in the source code from which this class file was compiled (see the JVM specification
     *  section 4.7.6). */
    [[using Rosebud: rosetta]]
    uint16_t inner_class_access_flags = 0;

public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmInnerClassesEntry(SgAsmJvmInnerClasses*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmInnerClassesEntry* parse(SgAsmJvmConstantPool*);

    /** Write inner classes entry to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
