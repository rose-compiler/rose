/** JVM RuntimeAnnotation.
 *
 *  Each entry in the annotations table represents a single run-time visible or invisible annotation on a declaration.
 *  See section 4.7.16 of the JVM specification. */
class SgAsmJvmRuntimeAnnotation: public SgAsmJvmNode {
public:
    /** Property: type_index
     *
     *  The value of the type_index item must be a valid index into the constant_pool table
     *  (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t type_index = 0;

    /** Property: element_value_pairs
     *
     *  Each value of the element_value_pairs table represents a single element-value pair
     *  in the annotation represented by this annotation structure, section 4.7.16. */
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmJvmRuntimeAnnotationPair*> element_value_pairs;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmRuntimeAnnotation(SgAsmJvmRuntimeVisibleAnnotations*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmRuntimeAnnotation* parse(SgAsmJvmConstantPool*);

    /** Write line number entry to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
