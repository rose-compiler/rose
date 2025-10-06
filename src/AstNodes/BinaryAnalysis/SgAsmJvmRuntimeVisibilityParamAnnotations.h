#include <sageContainer.h>

/** JVM RuntimeVisibleParameterAnnotations and RuntimeInvisibleParameterAnnotations attribute.
 *
 *  The RuntimeVisibleParameterAnnotations attribute is a variable-length attribute in the
 *  attributes table of the method_info structure (section 4.6). The RuntimeVisibleParameterAnnotations
 *  attribute stores run-time visible annotations on the declarations of formal parameters
 *  of the corresponding method. See section 4.7.18 of the JVM specification.
 *
 *  The RuntimeInvisibleParameterAnnotations attribute is a variable-length attribute in the
 *  attributes table of a method_info structure (section 4.6). The RuntimeInvisibleParameterAnnotations
 *  attribute stores run-time invisible annotations on the declarations of formal parameters
 *  of the corresponding method. See section 4.7.19 of the JVM specification.
 */
class SgAsmJvmRuntimeVisibilityParamAnnotations: public SgAsmJvmAttribute {
public:
    /** Property: parameter_annotations
     *
     *  Each entry in the parameter_annotations table represents all of the run-time visible
     *  (or invisible) annotations on the declaration of a single formal parameter.
     *  The parameter_annotation structure is specified in section 4.7.18. */
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmJvmRuntimeParameterAnnotation*> parameter_annotations;

    /** Property: isVisible
     *
     *  The isVisible property is true for a RuntimeVisibleParameterAnnotations attribute,
     *  false for a RuntimeInvisibleParameterAnnotations attribute. */
    [[using Rosebud: rosetta]]
    bool isVisible = true;

public:
    /** Initialize an SgAsmJvmRuntimeVisibilityParamAnnotations object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    SgAsmJvmRuntimeVisibilityParamAnnotations(SgAsmJvmAttributeTable* table, bool visibility);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmRuntimeVisibilityParamAnnotations* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the local variable table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
