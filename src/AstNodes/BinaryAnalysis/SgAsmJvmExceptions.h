#include <sageContainer.h>

/** JVM Exceptions attribute.
 *
 *  The Exceptions attribute is a variable-length attribute in the attributes table of a method_info
 *  struction (see section 4.6 of the JVM specification). The Exceptions attribute indicates which
 *  checked exceptions a method may throw. */
class SgAsmJvmExceptions: public SgAsmJvmAttribute {
public:
    /** Property: exception_index_table
     *
     *  Each value in the exception_index_table array must be a valid index into the constant_pool
     *  table. The constant_pool entry at that index must be the CONSTANT_Utf8_info structure
     *  representing the string "Exceptions" (see section 4.4.7 of the JVM specification). */
    [[using Rosebud: rosetta]]
    SgUnsigned16List exception_index_table;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing as it sets its parent. */
    explicit SgAsmJvmExceptions(SgAsmJvmAttributeTable* table);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmExceptions* parse(SgAsmJvmConstantPool*) override;

    /** Write the Exceptions attribute to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
