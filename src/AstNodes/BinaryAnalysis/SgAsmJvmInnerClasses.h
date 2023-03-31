/** Represents a JVM InnerClasses attribute.
 *
 *  The InnerClasses attribute is a variable-length attribute in the attributes table of a ClassFile structure. See section 4.7.6 of
 *  the JVM specification.
 */
class SgAsmJvmInnerClasses: public SgAsmJvmAttribute {
    /** Property: List of pointers to InnerClasses attribute entry. */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmInnerClassesEntryPtrList classes;

public:
    /** Initialize the InnerClasses attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmInnerClasses(SgAsmJvmAttribute*);

    /** Parses a JVM innerclasses attribute.
     *
     *  Parses a JVM innerclasses attribute and constructs and parses all innerclasses entries reachable from the table. Returns a
     *  pointer to this object. */
    SgAsmJvmInnerClasses* parse(SgAsmJvmConstantPool*) override;
};
