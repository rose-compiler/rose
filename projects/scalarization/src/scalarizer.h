#ifndef _SCALARIZER_H
#define _SCALARIZER_H

namespace scalarizer 
{
  bool enable_debug = false;
  bool keep_going = false;
  bool isFortran = false;

  std::vector<std::string> getFortranTargetnameList(SgNode* root);
  std::vector<std::string> getTargetnameList(SgNode* root);
  void transformType(SgVariableSymbol* sym, SgType* newType);
  void transformArrayType(SgBasicBlock* funcBody, SgVariableSymbol* sym, SgType* newType);

}  // end scalarizer namespace

#endif //_SCALARIZER_H
