/** JVM LineNumberEntry.
 *
 *  Each line_number is described by a line_number_table anonymous structure.  See the JVM specification, section 4.7.12. */
class SgAsmJvmLineNumberEntry: public SgAsmJvmNode {
    /** Property: start_pc
     *
     *  The value of the start_pc item must be a valid index into the constant_pool table (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t start_pc = 0;

    /** Property: line_number
     *
     *  The value of the line_number entry must be a valid index into the constant_pool table (see JVM specification). */
    [[using Rosebud: rosetta]]
    uint16_t line_number = 0;

 public:
    /** Initialize the object before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    explicit SgAsmJvmLineNumberEntry(SgAsmJvmLineNumberTable*);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmLineNumberEntry* parse(SgAsmJvmConstantPool*);

    /** Print some debugging information */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
