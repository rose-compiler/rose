

#ifndef __BinCompass_printAsmInstruction__
#define __BinCompass_printAsmInstruction__


#include "../binCompassAnalysisInterface.h"

class PrintAsmInstruction: public BC_AnalysisInterface {

// DQ (4/23/2009): Need to explicitly define the template parameters.
// '__gnu_cxx::_Hashtable_iterator<std::pair<const std::basic_string<char >, int>, std::basic_string<char >, __gnu_cxx::hash<std::basic_string<char > >, std::_Select1st<std::pair<const std::basic_string<char >, int> >, std::equal_to<std::basic_string<char > >, std::allocator<int> >' 
// to non-scalar type 
// '__gnu_cxx::_Hashtable_const_iterator<std::pair<const std::basic_string<char >, int>, std::basic_string<char >, hash_string, std::_Select1st<std::pair<const std::basic_string<char >, int> >, eqstr_string, std::allocator<int> >'

  rose_hash::hash_map<std::string, int,rose_hash::hash_string,rose_hash::eqstr_string> instMap;


 public:
  PrintAsmInstruction() {
    instMap.clear();
  }
  ~PrintAsmInstruction() {}

  void visit(SgNode* node);
  void init(SgNode* node);
  void finish(SgNode* node);

};

#endif

