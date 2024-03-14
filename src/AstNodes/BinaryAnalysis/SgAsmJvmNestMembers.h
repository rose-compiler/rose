#include <sageContainer.h>

/** JVM NestMembers attribute.
 *
 *  The NestMembers attribute records the classes and interfaces that are authorized to claim
 *  membership in the nest hosted by the current class or interface, see section 4.7.29 of the
 *  JVM specification. */
class SgAsmJvmNestMembers: public SgAsmJvmAttribute {
public:
    /** Property: classes
     *
     *  Each value in the classes array must be a valid index into the constant_pool table.
     *  The constant_pool entry at that index must be a CONSTANT_Class_info structure
     *  representing a class or interface which is a member of the nest hosted by the current
     *  class or interface (see section 4.4.1 of the JVM specification). */
    [[using Rosebud: rosetta]]
    SgUnsigned16List classes;

public:
    /** Initialize the attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing as it sets its parent. */
    explicit SgAsmJvmNestMembers(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmNestMembers* parse(SgAsmJvmConstantPool* pool) override;

    /** Write object to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
