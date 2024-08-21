/** Represents an entry in a JVM constant pool.
 *
 *  Constant pool entries are referenced by index starting at 1.
 */
class SgAsmJvmConstantPoolEntry: public SgAsmJvmNode {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constant pool tags.
     *
     * These tags indicate the kind of constant denoted by the pool entry.
     */
    enum Kind {
        EMPTY = 0,
        CONSTANT_Utf8 = 1,
        CONSTANT_Integer = 3,
        CONSTANT_Float = 4,
        CONSTANT_Long = 5,
        CONSTANT_Double = 6,
        CONSTANT_Class = 7,
        CONSTANT_String = 8,
        CONSTANT_Fieldref = 9,
        CONSTANT_Methodref = 10,
        CONSTANT_InterfaceMethodref = 11,
        CONSTANT_NameAndType = 12,
        CONSTANT_MethodHandle = 15,
        CONSTANT_MethodType = 16,
        CONSTANT_Dynamic = 17,
        CONSTANT_InvokeDynamic = 18,
        CONSTANT_Module = 19,
        CONSTANT_Package = 20
    };

#ifdef DOCUMENTATION
    /** These following structs are documented below to indicate which fields are active for each tag
     *    of an SgAsmJvmConstantPoolEntry instance. Other fields may not be accessed.
     */
#endif
    /** 4.4.1 CONSTANT_Class_info table entry. All fields are big endian. */
    struct CONSTANT_Class_info {
        uint8_t tag;
        uint16_t name_index;
    };

    /** 4.4.2 CONSTANT_Fieldref_info, CONSTANT_Methodref_info, and CONSTANT_InterfaceMethodref_info table
               entries. All fields are big endian. */
    struct CONSTANT_Fieldref_info {
        uint8_t tag;
        uint16_t class_index;
        uint16_t name_and_type_index;
    };
    struct CONSTANT_Methodref_info {
        uint8_t tag;
        uint16_t class_index;
        uint16_t name_and_type_index;
    };
    struct CONSTANT_InterfaceMethodref_info {
        uint8_t tag;
        uint16_t class_index;
        uint16_t name_and_type_index;
    };

    /** 4.4.3 CONSTANT_String_info table entry. All fields are big endian. */
    struct CONSTANT_String_info {
        uint8_t tag;
        uint16_t string_index;
    };

    /** 4.4.4 CONSTANT_Integer_info table entry. All fields are big endian. */
    struct CONSTANT_Integer_info {
        uint8_t tag;
        uint32_t bytes;
    };
    struct CONSTANT_Float_info {
        uint8_t tag;
        uint32_t bytes;
    };
    struct CONSTANT_Long_info {
        uint8_t tag;
        uint32_t hi_bytes;
        uint32_t low_bytes;
    };
    struct CONSTANT_Double_info {
        uint8_t tag;
        uint32_t hi_bytes;
        uint32_t low_bytes;
    };

    /** 4.4.6 CONSTANT_NameAndType_info table entry. All fields are big endian. */
    struct CONSTANT_NameAndType_info {
        uint8_t tag;
        uint16_t name_index;
        uint16_t descriptor_index;
    };

    /** 4.4.7 CONSTANT_Utf8_info table entry. All fields are big endian. */
    struct CONSTANT_Utf8_info {
        uint8_t tag;
        uint16_t length;
        uint8_t* bytes;
    };

    /** 4.4.8 CONSTANT_MethodHandle_info table entry. All fields are big endian. */
    struct CONSTANT_MethodHandle_info {
        uint8_t tag;
        uint8_t reference_kind;
        uint16_t reference_index;
    };

    /** 4.4.9 CONSTANT_MethodType_info table entry. All fields are big endian. */
    struct CONSTANT_MethodType_info {
        uint8_t tag;
        uint16_t descriptor_index;
    };

    /** 4.4.10 CONSTANT_Dynamic_info table entry. All fields are big endian. */
    struct CONSTANT_Dynamic_info {
        uint8_t tag;
        uint16_t bootstrap_method_attr_index;
        uint16_t name_and_type_index;
    };
    struct CONSTANT_InvokeDynamic_info {
        uint8_t tag;
        uint16_t bootstrap_method_attr_index;
        uint16_t name_and_type_index;
    };

    /** 4.4.11 CONSTANT_Module_info table entry. All fields are big endian. */
    struct CONSTANT_Module_info {
        uint8_t tag;
        uint16_t name_index;
    };

    /** 4.4.12 CONSTANT_Package_info table entry. All fields are big endian. */
    struct CONSTANT_Package_info {
        uint8_t tag;
        uint16_t name_index;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: tag
     *
     *  Enum constant describing the kind of this entry in the pool. */
    [[using Rosebud: rosetta]]
    SgAsmJvmConstantPoolEntry::Kind tag = SgAsmJvmConstantPoolEntry::EMPTY;

    /** Property: bytes
     *
     *  The bytes item of the CONSTANT_Integer_info structure represents the value of the int constant.  The bytes item of the
     *  CONSTANT_Float_info structure represents the value of the float constant in IEEE 754 binary32 floating-point format. */
    [[using Rosebud: rosetta]]
    uint32_t bytes = 0;

    /** Properties: hi_bytes
     *
     *  The unsigned high_bytes and low_bytes items of the CONSTANT_Long_info structure together represent the value of the long
     *  constant.  The unsigned high_bytes and low_bytes items of the CONSTANT_Double_info structure together represent the the
     *  double value in IEEE 754 binary64 floating-point format. */
    [[using Rosebud: rosetta]]
    uint32_t hi_bytes = 0;

    /** Properties: low_bytes
     *
     *  The unsigned high_bytes and low_bytes items of the CONSTANT_Long_info structure together represent the value of the long
     *  constant.  The unsigned high_bytes and low_bytes items of the CONSTANT_Double_info structure together represent the the
     *  double value in IEEE 754 binary64 floating-point format. */
    [[using Rosebud: rosetta]]
    uint32_t low_bytes = 0;

    /** Property: bootstrap_method_attr_index
     *
     *  The bootstrap_method_attr_index item is an index into the bootstrap_methods array of the bootstrap method table of this
     *  class file.  Present in CONSTANT_Dynamic_info and CONSTANT_Dynamic_info structs. */
    [[using Rosebud: rosetta]]
    uint16_t bootstrap_method_attr_index = 0;

    /** Property: class_index
     *
     *  The value of the class_index item must be a valid index into the constant_pool table (see JVM specification).  Present in
     *  CONSTANT_Fieldref_info, CONSTANT_Methodref_info, and CONSTANT_InterfaceMethodref_info table entries. */
    [[using Rosebud: rosetta]]
    uint16_t class_index = 0;

    /** Property: descriptor_index
     *
     *  The value of the descriptor_index item must be a valid index into the constant_pool table (see JVM specification).  Present
     *  in CONSTANT_NameAndType_info and CONSTANT_MethodHandle_info table entries. */
    [[using Rosebud: rosetta]]
    uint16_t descriptor_index = 0;

    /** Property: name_index
     *
     *  The value of the name_index item must be a valid index into the constant_pool table (see JVM specification).  Present in
     *  CONSTANT_Class_info, CONSTANT_NameAndType_info, CONSTANT_Module_info, and CONSTANT_Package_info table entries. */
    [[using Rosebud: rosetta]]
    uint16_t name_index = 0;

    /** Property: name_and_type_index
     *
     *  The value of the name_and_type_index item must be a valid index into the constant_pool table (see JVM specification).
     *  Present in CONSTANT_Fieldref_info, CONSTANT_Methodref_info, CONSTANT_InterfaceMethodref_info, CONSTANT_Dynamic_info, and
     *  CONSTANT_InvokeDynamic_info table entries. */
    [[using Rosebud: rosetta]]
    uint16_t name_and_type_index = 0;

    /** Property: reference_index
     *
     *  The value of the reference_index item must be a valid index into the constant_pool table (see JVM specification).  Present
     *  in CONSTANT_MethodHandle_info table entries. */
    [[using Rosebud: rosetta]]
    uint16_t reference_index = 0;

    /** Property: reference_kind
     *
     *  The value of the reference_kind item must be in the range 1 to 9. The value denotes the kind of a CONSTANT_MethodHandle_info
     *  table entry. */
    [[using Rosebud: rosetta]]
    uint8_t reference_kind = 0;

    /** Property: string_index
     *
     *  The value of the string_index item must be a valid index into the constant_pool table (see JVM specification).  Present in
     *  CONSTANT_String_info table entries. */
    [[using Rosebud: rosetta]]
    uint16_t string_index = 0;

    /** Property: length
     *
     *  The value of the length item gives the number of bytes in the bytes array (not the length of the resulting string).  Present
     *  in CONSTANT_Utf8_info table entries. */
    [[using Rosebud: rosetta]]
    uint16_t length = 0;

    // [Robb Matzke 2023-03-22]: I (and the serialization) have no idea what this points to, and therefore it cannot be
    // serialized. E.g., how many 'chars' should be serialized? Is the length stored in the `length` property? If so, `utf8_bytes`
    // should probably be an std::vector instead, which is a serializable thing. Furthermore, the data member cannot be 'const'
    // because that prevents it from being initialized by some de-serialization mechanisms (e.g., boost::serialize creates a default
    // constructed object and then fills it in).
    /** Property: utf8_bytes (bytes in CONSTANT_Utf8_info structure).
     *
     *  The bytes array contains the bytes of the string. Present in CONSTANT_Utf8_info table entries. */
    [[using Rosebud: rosetta, serialize()]]
    char* utf8_bytes = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Constructor creating an object ready to be initialized via parse(). */
    explicit SgAsmJvmConstantPoolEntry(SgAsmJvmConstantPoolEntry::Kind tag);

    /** Initialize a constant pool entry by parsing the file.
     *
     * @{ */
    SgAsmJvmConstantPoolEntry* parse(SgAsmJvmConstantPool* pool);
    /** @} */

    /** Write constant pool entry to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;

    /** Convert constant pool entry kind to a string */
    static std::string to_string(SgAsmJvmConstantPoolEntry::Kind);
};
