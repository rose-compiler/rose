/** CIL TypeDef node (II.22.37). */
class SgAsmCilTypeDef: public SgAsmCilMetadata {
  public:
    /// TypeAttribute Flags (II.23.1.15)    
    using TypeFlags = std::uint32_t;
    
    enum : TypeFlags
    {
      // Visibility attributes    
      FLG_VISIBILITY_MASK           = 0x00000007, ///< Use this mask to retrieve visibility information. 
                                                  ///< These 3 bits contain one of the following values:
        FLG_NOT_PUBLIC                = 0x00000000, ///< Class has no public scope
        FLG_PUBLIC                    = 0x00000001, ///< Class has public scope
        FLG_NESTED_PUBLIC             = 0x00000002, ///< Class is nested with public visibility
        FLG_NESTED_PRIVATE            = 0x00000003, ///< Class is nested with private visibility
        FLG_NESTED_FAMILY             = 0x00000004, ///< Class is nested with family visibility
        FLG_NESTED_ASSEMBLY           = 0x00000005, ///< Class is nested with assembly visibility
        FLG_NESTED_FAM_AND_ASSEM      = 0x00000006, ///< Class is nested with family and assembly visibility
        FLG_NESTED_FAM_OR_ASSEM       = 0x00000007, ///< Class is nested with family or assembly visibility
      // class layout attributes
      FLG_LAYOUT_MASK               = 0x00000018, ///< Use this mask to retrieve class layout information. 
                                                  ///< These 2 bits contain one of the following values:
        FLG_AUTO_LAYOUT               = 0x00000000, ///< Class fields are auto-laid out
        FLG_SEQUENTIAL_LAYOUT         = 0x00000008, ///< Class fields are laid out sequentially
        FLG_EXPLICIT_LAYOUT           = 0x00000010, ///< Layout is supplied explicitly
      // class semantics attributes    
      FLG_CLASS_SEMANTICS_MASK      = 0x00000020, ///< Use this mask to retrieve class semantics information. 
                                                  ///< This bit contains one of the following values:
        FLG_CLASS                     = 0x00000000, ///< Type is a class
        FLG_INTERFACE                 = 0x00000020, ///< Type is an interface
      // special semantics in addition to class semantics    
      FLG_ABSTRACT                  = 0x00000080, ///< Class is abstract
      FLG_SEALED                    = 0x00000100, ///< Class cannot be extended
      FLG_SPECIAL_NAME              = 0x00000400, ///< Class name is special
      // implementation attributes    
      FLG_IMPORT                    = 0x00001000, ///< Class/Interface is imported
      FLG_SERIALIZABLE              = 0x00002000, ///< Reserved (Class is serializable)
      // string formatting attributes
      FLG_STRING_FORMAT_MASK        = 0x00030000, ///< Use this mask to retrieve string information for native interop. 
                                                  ///< These 2 bits contain one of the following values:
        FLG_ANSI_CLASS                = 0x00000000, ///< LPSTR is interpreted as ANSI
        FLG_UNICODE_CLASS             = 0x00010000, ///< LPSTR is interpreted as Unicode
        FLG_AUTO_CLASS                = 0x00020000, ///< LPSTR is interpreted automatically
        FLG_CUSTOM_FORMAT_CLASS       = 0x00030000, ///< A non-standard encoding specified by CustomStringFormatMask
      FLG_CUSTOM_STRING_FORMAT_MASK = 0x00C00000, ///< Use this mask to retrieve non-standard encoding information for native interop. 
                                                  ///< The meaning of the values of these 2 bits is unspecified.
      // class initialization attributes    
      FLG_BEFORE_FIELD_INIT         = 0x00100000, ///< Initialize the class before first static field access
      // additional flags    
      FLG_RT_SPECIAL_NAME           = 0x00000800, ///< CLI provides special behavior, depending upon the name of the Type
      FLG_HAS_SECURITY              = 0x00040000, ///< Type has security associate with it
      FLG_IS_TYPE_FORWARDER         = 0x00200000, ///< This ExportedType entry is a type forwarder
    };
    
  private:  

    /** Property: Flags.
     *
     *  A 4-byte bitmask of type TypeAttributes. */
    [[using Rosebud: rosetta]]
    uint32_t Flags = 0;

    /** Property: TypeName.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeName = 0;

    /** Property: TypeNamespace.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t TypeNamespace = 0;

    /** Property: Extends.
     *
     *  An index into the TypeDef, TypeRef, or TypeSpec table. */
    [[using Rosebud: rosetta]]
    uint32_t Extends = 0;

    /** Property: FieldList.
     *
     *  An index into the Field table. */
    [[using Rosebud: rosetta]]
    uint32_t FieldList = 0;

    /** Property: MethodList.
     *
     *  An index into the MethodDef table. */
    [[using Rosebud: rosetta]]
    uint32_t MethodList = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_TypeName_string() const;
    const std::uint8_t* get_TypeNamespace_string() const;
    const SgAsmCilMetadata* get_Extends_object() const;
    const SgAsmCilMetadata* get_FieldList_object() const;
    const SgAsmCilMetadata* get_MethodList_object() const;
    const SgAsmCilMetadata* get_MethodList_object(const SgAsmCilMethodDef*) const;
};
