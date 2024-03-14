#include <sageContainer.h>

/** JVM MethodParameters attribute.
 *
 *  The MethodParameters attribute is a variable-length attribute in the attributes table of a method_info structure (4.6).
 *  A MethodParameters attribute records information about the formal parameters of a method, such as their names.
 *  See section 4.7.24 of the JVM specification. */
class SgAsmJvmMethodParameters: public SgAsmJvmAttribute {
public:
    /** Property: parameters
     *
     *  List of pointers to method parameter entries (see the JVM specification 4.7.24). */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmMethodParametersEntryPtrList parameters;

public:
    /** Initialize the MethodParameters attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmMethodParameters(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmMethodParameters* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the line number table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
