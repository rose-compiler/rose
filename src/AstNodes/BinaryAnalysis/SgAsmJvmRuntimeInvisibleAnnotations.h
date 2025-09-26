#include <sageContainer.h>

/** JVM RuntimeInvisibleAnnotations attribute.
 *
 *  The RuntimeInvisibleAnnotations attribute is a variable-length attribute in the attributes table of a ClassFile,
 *  field_info, method_info, or record_component_info structure, see sections (4.1, 4.5, 4.6, 4.7.30).
 *  The RuntimeInvisibleAnnotations attribute stores run-time invisible annotations on the declaration of the
 *  corresponding class, method, field, or record component. See section 4.7.17 of the JVM specification. */
class SgAsmJvmRuntimeInvisibleAnnotations: public SgAsmJvmAttribute {
public:
    /** Property: annotations
     *
     *  List of pointers to annotation entries. The annotation structure is specified in section 4.7.16. */
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmJvmRuntimeAnnotation*> annotations;

public:
    /** Initialize the RuntimeInvisibleAnnotations attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmRuntimeInvisibleAnnotations(SgAsmJvmAttributeTable*);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmRuntimeInvisibleAnnotations* parse(SgAsmJvmConstantPool*) override;

    /** Write the local variable table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
