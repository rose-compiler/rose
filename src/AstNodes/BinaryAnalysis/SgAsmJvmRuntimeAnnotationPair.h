/** JVM RuntimeAnnotationPair.
 *
 *  Each value of the element_value_pairs table represents a single element-value pair in the annotation
 *  represented by this annotation structure.  See section 4.7.16 of the JVM specification. */
class SgAsmJvmRuntimeAnnotationPair: public SgAsmJvmNode {
public:
    /** Property: element_name_index
     *
     *  The element_name_index denotes the name of the element of the element-value pair
     *  represented by this element_value_pairs entry. The constant_pool entry at this index must
     *  be a CONSTANT_Utf8_info structure (section 4.4.7). */
    [[using Rosebud: rosetta]]
    uint16_t element_name_index = 0;

    /** Property: value
     *
     *  The value item represents the value of the element-value pair represented by this
     *  element_value_pairs entry. */
    [[using Rosebud: rosetta]]
    SgAsmJvmRuntimeAnnotationValue* value = nullptr;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmRuntimeAnnotationPair(SgAsmJvmRuntimeAnnotation*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmRuntimeAnnotationPair* parse(SgAsmJvmConstantPool*);

    /** Write line number entry to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
