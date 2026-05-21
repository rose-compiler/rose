
/** CIL MethodDef node (II.22.26). */
class SgAsmCilMethodDef: public SgAsmCilMetadata {  
  public:
    using BodyState = int;
    
    enum : BodyState {    
      // good states > 0
      BDY_NOT_AVAILABLE               = 3, ///< RVA is 0 -> body is null
      BDY_FULLY_DECODED               = 2, ///< decoded successfully
      BDY_RUNTIME_SUPPORTED           = 1, ///< body is runtime supported (looking for specimen)
      
      // default == 0
      BDY_NOT_PROCESSED               = 0, ///< method has not been processed 
    
      // fail states < 0
      BDY_INVALID_HEADER_BYTE         = -1,  ///< method header leading byte cannot be read
      BDY_INVALID_HEADER_KIND         = -2,  ///< method header kind is neither tiny nor fat
      BDY_INVALID_HEADER_ALIGN        = -3,  ///< fat header is improperly aligned 
      BDY_INVALID_CODE_LENGTH         = -4,  ///< could not read all bytes as specified by header
      BDY_INVALID_INSTRUCTION_LENGTH  = -5,  ///< decoding instructions exceeded body length
      BDY_INVALID_INSTRUCTION         = -6,  ///< encountered unknown instruction
      BDY_INVALID_SECTION_ALIGN       = -7,  ///< extra sections are misaligned
      BDY_INVALID_SECTION_HEADER      = -8,  ///< extra section could not be read completely or is inconsistent
      BDY_INVALID_CLAUSE_KIND         = -9,  ///< clauses section length and kind do not match
      BDY_INVALID_CLAUSE_LENGTH       = -10, ///< clause could not be read completely
    };
        
    /// MethodAttribute Flags (II.23.1.10)    
    using MethodFlags = std::uint16_t;
    
    enum : MethodFlags {  
      FLG_MEMBERACCESS_MASK   = 0x0007, ///< These 3 bits contain one of the following values:
        FLG_COMPILER_CONTROLLED = 0x0000, ///< Member not referenceable
        FLG_PRIVATE             = 0x0001, ///< Accessible only by the parent type
        FLG_FAM_AND_ASSEM       = 0x0002, ///< Accessible by sub-types only in this Assembly
        FLG_ASSEM               = 0x0003, ///< Accessibly by anyone in the Assembly
      FLG_FAMILY              = 0x0004, ///< Accessible only by type and sub-types
      FLG_FAM_OR_ASSEM        = 0x0005, ///< Accessibly by sub-types anywhere, plus anyone in assembly
      FLG_PUBLIC              = 0x0006, ///< Accessibly by anyone who has visibility to this scope
      FLG_STATIC              = 0x0010, ///< Defined on type, else per instance
      FLG_FINAL               = 0x0020, ///< Method cannot be overridden
      FLG_VIRTUAL             = 0x0040, ///< Method is virtual
      FLG_HIDEBYSIG           = 0x0080, ///< Method hides by name+sig, else just by name
      FLG_VTABLE_LAYOUT_MASK  = 0x0100, ///< Use this mask to retrieve vtable attributes. This bit contains one of the following values:
        FLG_REUSE_SLOT          = 0x0000, ///< Method reuses existing slot in vtable
        FLG_NEW_SLOT            = 0x0100, ///< Method always gets a new slot in the vtable
      FLG_STRICT              = 0x0200, ///< Method can only be overriden if also accessible
      FLG_ABSTRACT            = 0x0400, ///< Method does not provide an implementation
      FLG_SPECIAL_NAME        = 0x0800, ///< Method is special
      // interop attributes    
      FLG_P_INVOKE_IMPL       = 0x2000, ///< Implementation is forwarded through PInvoke
      FLG_UNMANAGED_EXPORT    = 0x0008, ///< Reserved: shall be zero for conforming implementations
      // additional flags    
      FLG_RT_SPECIAL_NAME     = 0x1000, ///< CLI provides special behavior, depending upon the name of the method
      FLG_HAS_SECURITY        = 0x4000, ///< Method has security associate with it
      FLG_REQUIRE_SEC_OBJECT  = 0x8000, ///< Method calls another method containing security code.
    };
    
  private:  
    
    /** Property: RVA.
     *
     *  A 4-byte constant. */
    [[using Rosebud: rosetta]]
    uint32_t RVA = 0;

    /** Property: ImplFlags.
     *
     *  A 2-byte bitmask of type MethodImplAttributes. */
    [[using Rosebud: rosetta]]
    uint16_t ImplFlags = 0;

    /** Property: Flags.
     *
     *  A 2-byte bitmask of type MethodAttributes. */
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

    /** Property: ParamList.
     *
     *  An index into the Param table. */
    [[using Rosebud: rosetta]]
    uint32_t ParamList = 0;


    /** Property: body.
     *
     *  A block containing an instruction sequence (either x86 or CIL). */
    [[using Rosebud: rosetta]]
    SgAsmBlock* body = 0;

    /** Property: initLocals.
     *
     *  Returns true if locals are initialized. */
    [[using Rosebud: rosetta]]
    bool initLocals = 0;

    /** Property: stackSize.
     *
     *  Returns the max stack size required for this method. */
    [[using Rosebud: rosetta]]
    uint32_t stackSize = 0;

    /** Property: hasMoreSections.
     *
     *  Returns true if this method has more sections in addition to the body. */
    [[using Rosebud: rosetta]]
    bool hasMoreSections = 0;
    
    //
    // additional fields

    /** Property: localVarSigTok (II.24.4.3).
     *
     *  localVarSigTok is extracted from fat method headers; 0 when a
     *  tiny header is used. */
    [[using Rosebud: rosetta]]
    uint32_t localVarSigTok = 0;

    /** Property: methodData (II.24.4.5).
     *
     *  Collection of data sections (see hasMoreSections) */    
    [[using Rosebud: rosetta, accessors(get_methodData), mutators(), large]]
    std::vector<SgAsmCilMethodData*> methodData;
    
    /** Property: status (ROSE extension)
     *
     *  status indicates whether the data could be decoded.
     *  tiny header is used. */
    [[using Rosebud: rosetta]]
    int bodyState = BDY_NOT_PROCESSED;
  

public:
    void parse(const std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing);
    void unparse(std::vector<uint8_t>& buf, size_t& index, uint64_t uses4byteIndexing) const;
    void dump(std::ostream&) const;

    const std::uint8_t* get_Name_string() const;
    const std::uint8_t* get_Signature_blob() const;
    const SgAsmCilMetadata* get_ParamList_object() const;
};
