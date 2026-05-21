/** CIL Field node (II.22.15). */
class SgAsmCilField: public SgAsmCilMetadata {
  public:
    using FieldFlags = std::uint16_t;
    
    enum : FieldFlags {
      FLG_FIELD_ACCESS_MASK    = 0x0007, ///< These 3 bits contain one of the following values:
        FLG_COMPILER_CONTROLLED  = 0x0000, ///< Member not referenceable
        FLG_PRIVATE              = 0x0001, ///< Accessible only by the parent type
        FLG_FAMANDASSEM          = 0x0002, ///< Accessible by sub-types only in this Assembly
        FLG_ASSEMBLY             = 0x0003, ///< Accessibly by anyone in the Assembly
        FLG_FAMILY               = 0x0004, ///< Accessible only by type and sub-types
        FLG_FAMORASSEM           = 0x0005, ///< Accessibly by sub-types anywhere, plus anyone in assembly
        FLG_PUBLIC               = 0x0006, ///< Accessibly by anyone who has visibility to this scope field contract attributes
      FLG_STATIC               = 0x0010, ///< Defined on type, else per instance
      FLG_INIT_ONLY            = 0x0020, ///< Field can only be initialized, not written to after init
      FLG_LITERAL              = 0x0040, ///< Value is compile time constant
      FLG_NOT_SERIALIZED       = 0x0080, ///< Reserved (to indicate this field should not be serialized when type is remoted)
      FLG_SPECIAL_NAME         = 0x0200, ///< Field is special
      // interop attributes    
      FLG_PINVOKE_IMPL         = 0x2000, ///< Implementation is forwarded through PInvoke.
      // additional flags    
      FLG_RT_SPECIAL_NAME      = 0x0400, ///< CLI provides 'special' behavior, depending upon the name of the field
      FLG_HAS_FIELD_MARSHAL    = 0x1000, ///< Field has marshalling information
      FLG_HAS_DEFAULT          = 0x8000, ///< Field has default
      FLG_HAS_FIELD_RVA        = 0x0100, ///< Field has RVA      
    };
  
  private:

    /** Property: Flags.
     *
     *  A 2-byte bitmask of type FieldAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t Flags = 0;

    /** Property: Name.
     *
     *  An index into the String heap. */
    [[using Rosebud: rosetta]]
    uint32_t Name = 0;

    /** Property: Signature.
     *
     *  An index into the Blob heap. */
    [[using Rosebud: rosetta]]
    uint32_t Signature = 0;

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Signature_blob() const;
};
