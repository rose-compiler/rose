#include <sageContainer.h>

/** JVM IndexedAttr.
 *
 *  The SgAsmJvmIndexTableAttr class can represent one of a collection of attributes that contain
 *  zero or one uint16_t index property.
 *
 *  A ConstantValue attribute represents the value of a constant expression, see section 4.7.2 of the
 *  JVM specification.
 *
 *  A Synthetic attribute is a fixed-length attribute in the attributes table of a ClassFile, field_info,
 *  or method_info structure, see section 4.7.8 of the JVM specification.
 *
 *  A Signature attribute stores a signature for a class, interface, constructor, method, field, or
 *  record component, see section 4.7.9 of the JVM specification.
 *
 *  A SourceFile attribute stores an index to the name of the class file, see section 4.7.10 of the
 *  JVM specification.
 *
 *  A Deprecated attribute is an optional fixed-length attribute in the attributes table of a
 *  ClassFile, field_info, or method_info structure, see section 4.7.15 of the JVM specification.
 *  This attribute has no index in the specification so actual index value is unspecified in this class.
 *
 *  A ModuleMainClass attribute stores a signature for a class, interface, constructor, method, field, or
 *  record component, see section 4.7.27 of the JVM specification.
 *
 *  The NestHost attribute records the nest host of the nest to which the current class or
 *  interface claims to belong, see section 4.7.28 of the JVM specification.
 */
class SgAsmJvmIndexedAttr: public SgAsmJvmAttribute {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Type of JVM attribute.
     *
     * Attribute shall contain zero or one index property. */
    enum AttributeType {
        ATTR_NONE,
        ATTR_ConstantValue,   // 4.7.2
        ATTR_Synthetic,       // 4.7.8
        ATTR_Signature,       // 4.7.9
        ATTR_SourceFile,      // 4.7.10
        ATTR_Deprecated,      // 4.7.15
        ATTR_ModuleMainClass, // 4.7.27
        ATTR_NestHost         // 4.7.28
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: attribute_type
     *
     *  Indicates which JVM attribute is represented by this class instance. */
    [[using Rosebud: rosetta]]
    unsigned attribute_type = SgAsmJvmIndexedAttr::ATTR_NONE;

    /** Property: index.
     *
     *  Each index must be a valid index into the constant_pool table. */
    [[using Rosebud: rosetta]]
    uint16_t index;

 public:
    /** Initialize the IndexedAttr attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    SgAsmJvmIndexedAttr(SgAsmJvmAttributeTable* table, unsigned type);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmIndexedAttr* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the local variable table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
