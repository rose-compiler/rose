

#ifndef __BinCompass_AnalysisInterface__
#define __BinCompass_AnalysisInterface__

#include "rose.h"

class BC_AnalysisInterface   : public AstSimpleProcessing {
 protected:
  std::string name;
  RoseBin_unparse_visitor* unparser;

 public:
  BC_AnalysisInterface() {}
  virtual ~BC_AnalysisInterface() {}

  virtual void visit(SgNode* node) =0;
  std::string get_name() {return name;}
  void set_name(std::string n) {name=n;}
  void set_unparser(RoseBin_unparse_visitor* up) {unparser=up;}
};

#endif
