#include <sageContainer.h>

/** JVM RuntimeVisibleParameterAnnotations attribute.
 *
 *  The RuntimeVisibleParameterAnnotations attribute is a variable-length attribute in the
 *  attributes table of the method_info structure (section 4.6). The RuntimeVisibleParameterAnnotations
 *  attribute stores run-time visible annotations on the declarations of formal parameters
 *  of the corresponding method. See section 4.7.18 of the JVM specification. */
class SgAsmJvmRuntimeVisibleParameterAnnotations: public SgAsmJvmAttribute {
public:
    /** Property: parameter_annotations
     *
     *  Each entry in the parameter_annotations table represents all of the run-time visible
     *  annotations on the declaration of a single formal parameter.
     *  The parameter_annotation structure is specified in section 4.7.18. */
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmJvmRuntimeParameterAnnotation*> parameter_annotations;

public:
    /** Initialize the RuntimeVisibleParameterAnnotations attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmRuntimeVisibleParameterAnnotations(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmRuntimeVisibleParameterAnnotations* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the local variable table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
