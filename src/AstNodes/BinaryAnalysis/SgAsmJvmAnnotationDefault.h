/** JVM AnnotationDefault.
 *

 *  The AnnotationDefault attribute is a variable-length attribute in the attributes table of certain
 *  method_info structures (section 4.6), namely those representing elements of annotation interfaces.
 *  The AnnotationDefault attribute records the default value for the element represented by the
 *  method_info structure.  See section 4.7.22 of the JVM specification. */
class SgAsmJvmAnnotationDefault: public SgAsmJvmAttribute {
public:
    /** Property: value
     *
     *  The default_value item (section 4.7.16.1) represents the default value of the annotation
     *  interface element represented by the method_info structure enclosing this AnnotationDefault
     *  attribute. */
    [[using Rosebud: rosetta]]
    SgAsmJvmRuntimeAnnotationValue* default_value = nullptr;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmAnnotationDefault(SgAsmJvmAttributeTable*);

    /** Initialize the object by parsing content from the class file. */
    virtual SgAsmJvmAnnotationDefault* parse(SgAsmJvmConstantPool*) override;

    /** Write line number entry to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
