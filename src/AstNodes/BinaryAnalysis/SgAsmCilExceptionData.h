
                                          
/** Property class representing CIL Exception clauses (II.25.4.6). 
 *  
 * \note parsing/unparsing is handled by the using class (i.e., SgAsmCilMethoDef)
 */                                          
class SgAsmCilExceptionData : public SgAsmCilNode
{
public:
  enum 
  { 
    COR_ILEXCEPTION_CLAUSE_EXCEPTION = 0x0000,
    COR_ILEXCEPTION_CLAUSE_FILTER    = 0x0001,
    COR_ILEXCEPTION_CLAUSE_FINALLY   = 0x0002,
    COR_ILEXCEPTION_CLAUSE_FAULT     = 0x0004,
    COR_ILEXCEPTION_CLAUSE_MASK      = ( COR_ILEXCEPTION_CLAUSE_EXCEPTION
                                       | COR_ILEXCEPTION_CLAUSE_FILTER
                                       | COR_ILEXCEPTION_CLAUSE_FINALLY
                                       | COR_ILEXCEPTION_CLAUSE_FAULT
                                       ),
  };
  
private:
  [[using Rosebud: rosetta]]
  std::uint32_t flags = 0;
  
  [[using Rosebud: rosetta]]
  std::uint32_t tryOffset = 0;
                                               
  [[using Rosebud: rosetta]]
  std::uint32_t tryLength = 0;

  [[using Rosebud: rosetta]]
  std::uint32_t handlerOffset = 0;
  
  [[using Rosebud: rosetta]]
  std::uint32_t handlerLength = 0;
  
  [[using Rosebud: rosetta]]
  std::uint32_t classTokenOrFilterOffset = 0;
  
public:
  /**
   * Convenience functions to query the flags property.
   * \{
   */
  bool isException() const { return (get_flags() & COR_ILEXCEPTION_CLAUSE_MASK) == COR_ILEXCEPTION_CLAUSE_EXCEPTION; }
  bool isFilter()    const { return (get_flags() & COR_ILEXCEPTION_CLAUSE_MASK) == COR_ILEXCEPTION_CLAUSE_FILTER; }
  bool isFinally()   const { return (get_flags() & COR_ILEXCEPTION_CLAUSE_MASK) == COR_ILEXCEPTION_CLAUSE_FINALLY; }
  bool isFault()     const { return (get_flags() & COR_ILEXCEPTION_CLAUSE_MASK) == COR_ILEXCEPTION_CLAUSE_FAULT; }
  /** \} */  
};
