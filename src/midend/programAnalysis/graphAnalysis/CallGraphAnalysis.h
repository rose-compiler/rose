


#ifndef __SB_CallAnalysis__
#define __SB_CallAnalysis__


class SB_CallGraph {
 private:
  RoseBin_CallGraphAnalysis* bin_callanalysis;
  RoseSrc_CallGraphAnalysis* src_callanalysis;

 public:
  SB_CallGraph() {
    bin_callanalysis=NULL;
    src_callanalysis=NULL;
  }
  virtual ~SB_CallGraph() {}

  void createCallGraph(SgProject* project);

  RoseBin_CallGraphAnalysis* getBinaryCallGraph() {return bin_callanalysis;}
  RoseSrc_CallGraphAnalysis* getSourceCallGraph() {return src_callanalysis;}


};

#endif
