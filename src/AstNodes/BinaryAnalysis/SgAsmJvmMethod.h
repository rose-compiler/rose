#ifdef ROSE_IMPL
#include <SgAsmInstructionList.h>
#include <SgAsmJvmAttributeTable.h>
#endif

/** JVM Method.
 *
 *  A method describes an instance of an initialization method (2.9.1) and the class or interface initialization method (2.9.2), see
 *  section 4.6 of the JVM specification. */
class SgAsmJvmMethod: public SgAsmJvmNode {
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

    /** Property: Instruction list.
     *
     *  Pointer to list of instructions (@ref SgAsmInstruction) belonging to this method. */
    [[using Rosebud: rosetta]]
    SgAsmInstructionList* instruction_list = createAndParent<SgAsmInstructionList>(this);

public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmMethod(SgAsmJvmMethodTable*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmMethod* parse(SgAsmJvmConstantPool*);

    /** Write method to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
