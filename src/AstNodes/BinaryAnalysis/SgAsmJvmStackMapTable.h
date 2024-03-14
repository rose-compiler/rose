#include <sageContainer.h>

/** The StackMapTable attribute is a variable-length attribute in the attributes table
 *  of a Code attribute (see section 4.7.3). A StackMapTable attribute is used during the
 *  process of verification by type checking (4.10.1).
 */
class SgAsmJvmStackMapTable: public SgAsmJvmAttribute {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: List of stack map table entries/frames. */
    [[using Rosebud: rosetta, large]]
    SgAsmJvmStackMapFramePtrList entries;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /**
     * Initialize the object before parsing. This is the preferred constructor
     * as it sets the parent. */
    explicit SgAsmJvmStackMapTable(SgAsmJvmAttributeTable* table);

    /** Initialize the object by parsing content from the class file. */
    virtual SgAsmJvmStackMapTable* parse(SgAsmJvmConstantPool* pool) override;

    /** Write stack map table to a binary file */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
