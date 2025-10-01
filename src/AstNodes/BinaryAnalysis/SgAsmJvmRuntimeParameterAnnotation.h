/** JVM RuntimeParameterAnnotation.
 *
 *  Each entry in the parameter_annotations table represents all of the run-time visible annotations
 *  on the declaration of a single formal parameter. See section 4.7.18 of the JVM specification. */
class SgAsmJvmRuntimeParameterAnnotation: public SgAsmJvmNode {
public:
    /** Property: annotations
     *
     *  Each entry in the annotations table represents a single run-time visible annotation on the
     *  declaration of the formal parameter corresponding to the parameter_annotations entry.
     *  The annotation structure is specified in section 4.7.16. */
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmJvmRuntimeAnnotation*> annotations;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmRuntimeParameterAnnotation(SgAsmJvmRuntimeVisibleParameterAnnotations*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmRuntimeParameterAnnotation* parse(SgAsmJvmConstantPool*);

    /** Write line number entry to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
