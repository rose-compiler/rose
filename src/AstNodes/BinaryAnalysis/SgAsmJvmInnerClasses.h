#include <sageContainer.h>

/** Represents a JVM InnerClasses attribute.
 *
 *  The InnerClasses attribute is a variable-length attribute in the attributes table of a ClassFile structure. See section 4.7.6 of
 *  the JVM specification.
 */
class SgAsmJvmInnerClasses: public SgAsmJvmAttribute {
public:
    /** Property: List of pointers to InnerClasses attribute entry. */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmInnerClassesEntryPtrList classes;

public:
    /** Initialize the InnerClasses attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing as it sets its parent. */
    explicit SgAsmJvmInnerClasses(SgAsmJvmAttributeTable* table);

    /** Parses a JVM InnerClasses attribute.
     *
     * Parses a JVM InnerClasses attribute and constructs and parses all classes
     * entries reachable from the table. Returns a pointer to this object. */
    SgAsmJvmInnerClasses* parse(SgAsmJvmConstantPool*) override;

    /** Write the InnerClasses attribute to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
