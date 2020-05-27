#ifndef _SCALARIZER_H
#define _SCALARIZER_H

namespace scalarizer 
{
  bool enable_debug = false;
  bool keep_going = false;
  bool isFortran = false;

  std::map<std::string,int> getFortranTargetnameList(SgNode* root);
  std::map<std::string,int> getTargetnameList(SgNode* root);
  void transformType(SgVariableSymbol* sym, SgType* newType);
  void transformArrayType(SgBasicBlock* funcBody, SgVariableSymbol* sym, SgType* newType, int parameter);
  static int is_directive_sentinels(std::string str, SgNode* c_sgnode);
  static bool match_substr(const char* substr, const char* c_char);
  static bool isFixedSourceForm(SgNode* c_sgnode);


}  // end scalarizer namespace

#endif //_SCALARIZER_H
