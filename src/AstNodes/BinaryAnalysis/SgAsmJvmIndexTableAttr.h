#include <sageContainer.h>

/** JVM IndexTableAttr.
 *
 *  The IndexTableAttr class can represent one of a collection of attributes that contain a
 *  list of uint16_t indices.
 *
 *  The Exceptions attribute is a variable-length attribute in the attributes table of a method_info
 *  struction (see section 4.6 of the JVM specification). The Exceptions attribute indicates which
 *  checked exceptions a method may throw.  Each constant_pool entry in the table must be a
 *  CONSTANT_Class_info structure (section 4.4.1) representing a class type that this method is
 *  declared to throw.  See section 4.7.5 of the JVM specification.
 *
 *  The ModulePackages attribute is a variable-length attribute in the attributes table of a ClassFile
 *  structure (section 4.1). The ModulePackages attribute indicates all the packages of a module that are
 *  exported or opened by the Module attribute, as well as all the packages of the service implementations
 *  recorded in the Module attribute. The ModulePackages attribute may also indicate packages in the module
 *  that are neither exported nor opened nor contain service implementations. Each constant_pool entry in the
 *  table must be a CONSTANT_Package_info structure (section 4.4.12) representing a package in the current
 *  module. See section 4.7.26 of the JVM specification.
 *
 *  The NestMembers attribute records the classes and interfaces that are authorized to claim
 *  membership in the nest hosted by the current class or interface.  Each constant_pool entry in the
 *  table must be a CONSTANT_Class_info structure (section 4.4.1) representing a class or interface which
 *  is a member of the nest hosted by the current class or interface.  See section 4.7.29 of the
 *  JVM specification.
 *
 *  The PermittedSubclasses attribute is a variable-length attribute in the attributes table of a ClassFile
 *  structure (section 4.1). The PermittedSubclasses attribute records the classes and interfaces that are
 *  authorized to directly extend or implement the current class or interface (section 5.3.5). Each
 *  constant_pool entry in the table must be a CONSTANT_Class_info structure (section 4.4.1) representing
 *  a class or interface which is authorized to directly extend or implement the current class or interface.
 *  See section 4.7.31 of the JVM specification.
 */
class SgAsmJvmIndexTableAttr: public SgAsmJvmAttribute {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Type of JVM attribute.
     *
     * Attribute will have list of indices. */
    enum AttributeType {
        ATTR_NONE,
        ATTR_Exceptions,         // 4.7.5
        ATTR_ModulePackages,     // 4.7.26
        ATTR_NestMembers   ,     // 4.7.29
        ATTR_PermittedSubclasses // 4.7.31
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: attribute_type
     *
     *  Indicates which JVM attribute is represented by this class instance. */
    [[using Rosebud: rosetta]]
    unsigned attribute_type = SgAsmJvmIndexTableAttr::ATTR_NONE;

    /** Property: table.
     *
     *  Each value in the table must be a valid index into the constant_pool table. */
    [[using Rosebud: rosetta, mutators(), large]]
    std::vector<uint16_t> table;

 public:
    /** Initialize the IndexTableAttr attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing.  It shall set its parent. */
    SgAsmJvmIndexTableAttr(SgAsmJvmAttributeTable* table, unsigned type);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmIndexTableAttr* parse(SgAsmJvmConstantPool* pool) override;

    /** Write the local variable table to a binary file. */
    virtual void unparse(std::ostream&) const override;

    /** Print some debugging information. */
    virtual void dump(FILE*, const char *prefix, ssize_t idx) const override;
};
