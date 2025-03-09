/** JVM Deprecated attribute.
 *
 *  A Deprecated attribute is an optional fixed-length attribute in the attributes table of a
 *  ClassFile, field_info, or method_info structure, see section 4.7.15 of the JVM specification. */
class SgAsmJvmDeprecated: public SgAsmJvmAttribute {
public:
    /** Initialize the attribute before parsing.
     *
     *  This is the preferred constructor to use before parsing as it sets its parent. */
    explicit SgAsmJvmDeprecated(SgAsmJvmAttributeTable* table);

    /** Initialize the attribute by parsing the file. */
    virtual SgAsmJvmAttribute* parse(SgAsmJvmConstantPool* pool) override;
};
