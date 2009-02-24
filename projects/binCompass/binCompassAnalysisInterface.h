

#ifndef __BinCompass_AnalysisInterface__
#define __BinCompass_AnalysisInterface__



class BC_AnalysisInterface   : public AstSimpleProcessing {
 protected:
  std::string name;
  std::string output;

 public:
  BC_AnalysisInterface() {
    output=""; 
    name="";
  }
  virtual ~BC_AnalysisInterface() {}

  virtual void visit(SgNode* node) =0;
  virtual void init(SgNode* node) =0;
  virtual void finish(SgNode* node) =0;
  std::string get_output() {return output;}
  std::string get_name() {return name;}
  void set_name(std::string n) {name=n;}
};

#endif
