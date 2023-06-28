/** Represents an entry in a stack map table.
 *
 */
class SgAsmJvmStackMapVerificationType: public SgAsmJvmNode {
private:
    /** StackMapVerificationType tags.
     *
     * These tags indicate which item of the verification_type_info union is in use.
     */
    enum verification_type_tag {
        ITEM_Top = 0,
        ITEM_Integer = 1,
        ITEM_Float = 2,
        ITEM_Double = 3,
        ITEM_Long = 4,
        ITEM_Null = 5,
        ITEM_UninitializedThis = 6,
        ITEM_Object = 7,
        ITEM_Uninitialized = 8
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types (for documentation purposes only)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef DOCUMENTATION
    /** The following discriminated union is documented below and provides information about
     *    the SgAsmJvmStackMapVerificationType entry (see 4.7.4 in JVM specification).
     */
    union verification_type_info {
        Top_variable_info;
        Integer_variable_info;
        Float_variable_info;
        Long_variable_info;
        Double_variable_info;
        Null_variable_info;
        UninitializedThis_variable_info;
        Object_variable_info;
        Uninitialized_variable_info;
    };

    /** 4.7.4 */
    struct Top_variable_info {
        uint8_t tag;
    };
    struct Integer_variable_info {
        uint8_t tag;
    };
    struct Float_variable_info {
        uint8_t tag;
    };
    struct Long_variable_info {
        uint8_t tag;
    };
    struct Double_variable_info {
        uint8_t tag;
    };
    struct Null_variable_info {
        uint8_t tag;
    };
    struct UnitializedThis_variable_info {
        uint8_t tag;
    };
    struct Object_variable_info {
        uint8_t tag;
        uint16_t cpool_index;
    };
    struct Unitialized_variable_info {
        uint8_t tag;
        uint16_t offset;
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    /** Property: tag
     *
     *  Enum constant describing variable type. */
    [[using Rosebud: rosetta]]
    uint8_t tag = 9; /* Not a legal entry */

    /** Property: cpool_index
     *
     *  See the JVM specification. */
    [[using Rosebud: rosetta]]
    uint16_t cpool_index = 0;

    /** Property: offset
     *
     *  See the JVM specification. */
    [[using Rosebud: rosetta]]
    uint16_t offset = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /**
     * Initialize the object before parsing. This is the preferred constructor
     * as it sets the parent. */
    explicit SgAsmJvmStackMapVerificationType(SgAsmJvmStackMapFrame* frame);

    /** Initialize the object by parsing content from the class file. */
    SgAsmJvmStackMapVerificationType* parse(SgAsmJvmConstantPool* pool);

    /** Write stack map table to a binary file */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
