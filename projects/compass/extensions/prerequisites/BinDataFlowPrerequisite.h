#ifndef COMPASS_BIN_DATA_FLOW_PREREQUISITE_H
#define COMPASS_BIN_DATA_FLOW_PREREQUISITE_H

#include "AuxiliaryInfoPrerequisite.h"

  class BinDataFlowPrerequisite: public Prerequisite {
    RoseBin_DataFlowAnalysis* dfanalysis;
    RoseBin_Graph* graph;
  public:
  BinDataFlowPrerequisite(): Prerequisite("BinDataFlow"), dfanalysis(NULL) {}
    void run(SgProject* project) {

   // DQ (9/3/2008): Using new SgBinaryFile IR node.
   // SgAsmFile* file = project->get_file(0).get_binaryFile();
      SgBinaryFile* binaryFile = isSgBinaryFile(project->get_fileList()[0]);
      SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;

      if (file==NULL)
        done =true;
      if (done) return;

      bool forward = true;
      bool interprocedural = false;
      bool edges = true;
      bool mergedEdges = true;

      VirtualBinCFG::AuxiliaryInformation* info = auxiliaryInfoPrerequisite.getAuxiliaryInfo();

      std::string dfgFileName = "dfg.dot";
      graph = new RoseBin_DotGraph();
      ROSE_ASSERT(graph);

      GraphAlgorithms* algo = new GraphAlgorithms(info);
      SgAsmInterpretation* interp = SageInterface::getMainInterpretation(file);
      dfanalysis = new RoseBin_DataFlowAnalysis(interp->get_global_block(), forward, new RoseObj(), algo);
      dfanalysis->init(interprocedural, edges);
      dfanalysis->run(graph, dfgFileName, mergedEdges);

      done = true;
    }

    PrerequisiteList getPrerequisites() const {
      PrerequisiteList prerequisiteList;
      prerequisiteList.push_back(&auxiliaryInfoPrerequisite);
      return prerequisiteList;
    }

    RoseBin_DataFlowAnalysis* getBinDataFlowInfo() const {
      ROSE_ASSERT (done);
      return dfanalysis;
    }
    RoseBin_Graph* getGraph() const {
      ROSE_ASSERT (done);
      return graph;
    }
  };

extern BinDataFlowPrerequisite binDataFlowPrerequisite;

#endif
