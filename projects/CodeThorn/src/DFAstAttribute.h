#ifndef DFASTATTRIBUTE_H
#define DFASTATTRIBUTE_H

#include <string>
using std::string;

enum DFInfoSelector { DFISEL_DEFAULT, 
					  //DFISEL_FUNCTION_CALL,
					  DFISEL_FUNCTION_CALLRETURN,
					  //DFISEL_FUNCTION_ENTRY,
					  DFISEL_FUNCTION_EXIT,
					  //DFISEL_BLOCK_BEGIN,
					  //DFIDEL_BLOCK_END
};

class DFAstAttribute : public AstAttribute {
 public:
  DFAstAttribute();
  string virtual toString();
  void setDFInfoSelector(DFInfoSelector flowInfoSelector);
  DFInfoSelector getDFInfoSelector();
  ~DFAstAttribute();
 private:
  DFInfoSelector _dfiSelector;
};

#endif
