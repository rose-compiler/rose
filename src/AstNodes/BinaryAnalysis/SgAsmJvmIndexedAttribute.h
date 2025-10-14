#include <sageContainer.h>

/** JVM IndexedAttribute.
 *
 *  The IndexedAttribute class can represent one of a collection of attributes that contain a
 *  list of uint16_t indices.
 *
 *  The PermittedSubclasses attribute is a variable-length attribute in the attributes table of a ClassFile
 *  structure (section 4.1). The PermittedSubclasses attribute records the classes and interfaces that are
 *  authorized to directly extend or implement the current class or interface (section 5.3.5). See section
 *  4.7.31 of the JVM specification.
 */
class SgAsmJvmIndexedAttribute: public SgAsmJvmAttribute {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Type of JVM attribute.
     *
     * Attribute will have list of indices. */
    enum AttributeType {
        ATTR_NONE,
        ATTR_PermittedSubclasses,
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: attribute_type
     *
     *  Indicates which JVM attribute is represented by this class instance. */
    [[using Rosebud: rosetta]]
    unsigned attribute_type = SgAsmJvmIndexedAttribute::ATTR_NONE;

    /** Property: indices.
     *
     *  Each value in the indices array must be a valid index into the constant_pool table. */
    [[using Rosebud: rosetta]]
    SgUnsigned16List indices;

 public:
    /** Initialize the IndexedAttribute attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    SgAsmJvmIndexedAttribute(SgAsmJvmAttributeTable* table, unsigned type);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmIndexedAttribute* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the local variable table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
