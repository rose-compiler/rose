#include <vector>
                                          
/** Property class representing CIL Extra Sections (II.25.4.5). 
 *  
 * \note parsing/unparsing is handled by the using class (i.e., SgAsmCilMethoDef)
 */                                          
class SgAsmCilMethodData : public SgAsmCilNode
{
public:
  enum 
  {
    CorILMethod_Sect_EHTable    = 0x01,
    CorILMethod_Sect_OptILTable = 0x02,
    CorILMethod_Sect_FatFormat  = 0x40,
    CorILMethod_Sect_MoreSects  = 0x40, 
  };
  
private:
  [[using Rosebud: rosetta]]
  uint64_t kind = 0;
  
  [[using Rosebud: rosetta]]
  std::uint32_t dataSize = 0;
  
  [[using Rosebud: rosetta, accessors(get_Clauses), mutators(), large]]
  std::vector<SgAsmCilExceptionData*> clauses;

public:
  bool isExceptionSection() const
  {
    return (p_kind & CorILMethod_Sect_EHTable) == CorILMethod_Sect_EHTable;
  }
  
  bool isOptILTable() const
  {
    const bool res = (p_kind & CorILMethod_Sect_OptILTable) == CorILMethod_Sect_OptILTable;
    
    ASSERT_require(!res);
    return res;
  }
  
  bool usesFatFormat() const
  {
    return (p_kind & CorILMethod_Sect_FatFormat) == CorILMethod_Sect_FatFormat;
  }
  
  bool hasMoreSections() const
  {
    return (p_kind & CorILMethod_Sect_MoreSects) == CorILMethod_Sect_MoreSects;
  }
};
  
