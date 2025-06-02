
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
