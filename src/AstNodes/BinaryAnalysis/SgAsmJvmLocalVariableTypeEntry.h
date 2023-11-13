/** JVM LocalVariableTypeEntry.
 *
 *  Each local_variable is described by a local_variable_table anonymous structure.  See the JVM specification, section 4.7.14. */
class SgAsmJvmLocalVariableTypeEntry: public SgAsmJvmNode {
    /** Property: start_pc
     *
     *  The value of the start_pc item must be a valid index into the code array of this Code attribute and must be
     *  the index of the opcode of an instruction (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t start_pc = 0;

    /** Property: length
     *
     *  The value of the start_pc + length item must either be a valid index into the code array of this Code attribute
     *  and be the index of the opcode of an instruction, or it must be the first index beyond the end of that code
     *  array (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t length = 0;

    /** Property: name_index
     *
     *  The value of the name_index item must be a valid index into the constant_pool table. The constant_pool entry
     *  at that index must contain a CONSTANT_Utf8_info structure representing a valid unqualified name denoting
     *  a local variable (see JVM specification, 4.2.2). */
    [[using Rosebud: rosetta]]
    uint16_t name_index = 0;

    /** Property: signature_index
     *
     *  The value of the signature_index item must be a valid index into the constant_pool table. The constant_pool entry
     *  at that index must contain a CONSTANT_Utf8_info structure representing a field signature which encodes
     *  the type of a variable in the source program (see JVM specification, 4.7.9.1). */
    [[using Rosebud: rosetta]]
    uint16_t signature_index = 0;

    /** Property: index
     *
     *  The value of the index item must be a valid index into the local variable array of the current frame. The
     *  given local variable is at index in the local variable array of the current frame (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t index = 0;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmLocalVariableTypeEntry(SgAsmJvmLocalVariableTypeTable*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmLocalVariableTypeEntry* parse(SgAsmJvmConstantPool*);

    /** Write the local variable entry to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
