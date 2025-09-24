#include <sageContainer.h>

/** JVM RuntimeVisibleAnnotations attribute.
 *
 *  The RuntimeVisibleAnnotations attribute is a variable-length attribute in the attributes table of a ClassFile,
 *  field_info, or method_info structure, see sections (4.1, 4.5, 4.6). The RuntimeVisibleAnnotations attribute
 *  records run-time visible annotations on the declaration of the corresponding class, field, or method. See
 *  section 4.7.16 of the JVM specification. */
class SgAsmJvmRuntimeVisibleAnnotations: public SgAsmJvmAttribute {
public:
    /** Property: annotations
     *
     *  List of pointers to annotation entries. The annotation structure is specified in section 4.7.16. */
    [[using Rosebud: rosetta, traverse, mutators(), large]]
    std::vector<SgAsmJvmRuntimeAnnotation*> annotations;

public:
    /** Initialize the RuntimeVisibleAnnotations attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmRuntimeVisibleAnnotations(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmRuntimeVisibleAnnotations* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the local variable table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
