/** Represents the file header of an JVM binary container.
 *
 *  The file header contains information that the Java Virtual Machine (JVM) uses to find the various parts within the
 *  container. Most of the object properties are defined in the official JVM specification and their documentation is not
 *  replicated here.
 */
class SgAsmJvmFileHeader: public SgAsmGenericHeader {
public:
    /** Property: Minor version.
     *
     *  Minor version number of this class file. */
    [[using Rosebud: rosetta]]
    uint16_t minor_version = 0;

    /** Property: Major version.
     *
     *  Major version number of this class file. */
    [[using Rosebud: rosetta]]
    uint16_t major_version = 0;

    /** Property: Access flags.
     *
     *  Mask of flags used to denote access permissions and properties of this class or interface. */
    [[using Rosebud: rosetta]]
    uint16_t access_flags = 0;

    /** Property: This class index.
     *
     *  Index into constant pool table for this class or interface. */
    [[using Rosebud: rosetta]]
    uint16_t this_class = 0;

    /** Property: Super class.
     *
     *  Index into constant pool table for direct super class of this class or interface. */
    [[using Rosebud: rosetta]]
    uint16_t super_class = 0;

    /** Property: Constant pool.
     *
     *  Points to the AST node that represents the JVM constant pool. The constant pool is a table of structures
     *  (see 4.4) representing various string constants, class and interface names, field names, and other constants
     *  that are referred to within the ClassFile structure and its substructures. The format of each constant-pool
     *  table entry is indicated by its first "tag" byte. */
    [[using Rosebud: rosetta]]
    SgAsmJvmConstantPool* constant_pool = nullptr;

    /** Property: Interfaces.
     *
     *  Each value in the interfaces array must be a valid index into the constant_pool table. The constant_pool entry
     *  at each value is a CONSTANT_Class_info structure representing an interface that is a direct superinterface
     *  of this class or interface type. */
    [[using Rosebud: rosetta, large]]
    std::list<uint16_t> interfaces;

    /** Property: Field table.
     *
     *  Points to the AST node that represents the JVM field table. The constant pool is a table of structures
     *  (see 4.4) representing various string constants, class and interface names, field names, and other constants
     *  that are referred to within the ClassFile structure and its substructures. The format of each constant-pool
     *  table entry is indicated by its first "tag" byte. */
    [[using Rosebud: rosetta]]
    SgAsmJvmFieldTable* field_table = nullptr;

    /** Property: Method table.
     *
     *  Points to the AST node that represents the JVM method table. The constant pool is a table of structures
     *  (see 4.4) representing various string constants, class and interface names, field names, and other constants
     *  that are referred to within the ClassFile structure and its substructures. The format of each constant-pool
     *  table entry is indicated by its first "tag" byte. */
    [[using Rosebud: rosetta]]
    SgAsmJvmMethodTable* method_table = nullptr;

    /** Property: Attribute table.
     *
     *  Points to the AST node that represents the JVM attribute table. The constant pool is a table of structures
     *  (see 4.4) representing various string constants, class and interface names, field names, and other constants
     *  that are referred to within the ClassFile structure and its substructures. The format of each constant-pool
     *  table entry is indicated by its first "tag" byte. */
    [[using Rosebud: rosetta]]
    SgAsmJvmAttributeTable* attribute_table = nullptr;

public:
    /** Construct a new JVM File Header with default values; ready to be initialized via parse(). */
    explicit SgAsmJvmFileHeader(SgAsmGenericFile *);

    /** Parse header from file.
     *
     *  Initialize this header with information parsed from the file and construct and parse everything that's reachable
     *  from the header. Since the size of the ELF File Header is determined by the contents of the ELF File Header as
     *  stored in the file, the size of the ELF File Header will be adjusted upward if necessary. */
    virtual SgAsmJvmFileHeader* parse() override;

    /** Write the section to a binary file */
    virtual void unparse(std::ostream&) const override;

    /** Returns true if a cursory look at the file indicates that it could be a JVM class file. */
    static bool is_JVM(SgAsmGenericFile*);

protected:
    // Some extra clean-up actions on destruction
    virtual void destructorHelper() override;
};
