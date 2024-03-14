#ifdef ROSE_IMPL
#include <SgAsmJvmAttributeTable.h>
#endif

/** JVM Field.
 *
 *  Each field is described by a field_info structure. */
class SgAsmJvmField: public SgAsmJvmNode {
public:
    /** Property: access_flags
     *
     *  The value of the access_flags item is a mask of flags used to denote access permission to and properties of this method (see
     *  JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t access_flags = 0;

    /** Property: name_index
     *
     *  The value of the name_index item must be a valid index into the constant_pool table (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t name_index = 0;

    /** Property: descriptor_index
     *
     *  The value of the descriptor_index item must be a valid index into the constant_pool table (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t descriptor_index = 0;

    /** Property: Table of attributes. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmJvmAttributeTable* attribute_table = nullptr;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmField(SgAsmJvmFieldTable*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmField* parse(SgAsmJvmConstantPool* pool);

    /** Write field to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
