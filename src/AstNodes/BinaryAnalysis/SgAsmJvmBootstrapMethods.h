#include <sageContainer.h>

/** The BootstrapMethods attribute is a variable-length attribute in the attributes table of a ClassFile
 *  structure (Section 4.1). The BootstrapMethods attribute records bootstrap methods used to produce
 *  dynamically-computed constants and dynamically-computed call sites (Section 4.4.10).
 */
class SgAsmJvmBootstrapMethods: public SgAsmJvmAttribute {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: List of bootstrap method entries. */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmBootstrapMethodPtrList bootstrap_methods;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Initialize the bootstrap methods object before parsing.
     *
     *  This is the preferred constructor to use before parsing as it sets its parent. */
    explicit SgAsmJvmBootstrapMethods(SgAsmJvmAttributeTable*);

    /** Initialize the object by parsing content from the class file. */
    virtual SgAsmJvmBootstrapMethods* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the BootstrapMethods attribute to a binary file */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
