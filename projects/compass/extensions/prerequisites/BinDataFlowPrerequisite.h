#ifndef COMPASS_BIN_DATA_FLOW_PREREQUISITE_H
#define COMPASS_BIN_DATA_FLOW_PREREQUISITE_H

#include "AuxiliaryInfoPrerequisite.h"

  class BinDataFlowPrerequisite: public Prerequisite {
    RoseBin_DataFlowAnalysis* dfanalysis;
    RoseBin_Graph* graph;
  public:
  BinDataFlowPrerequisite(): Prerequisite("BinDataFlow"), dfanalysis(NULL) {}
    void run(SgProject* project) {
      SgAsmFile* file = project->get_file(0).get_binaryFile();
      if (file==NULL)
        done =true;
      if (done) return;

      bool forward = true;
      bool interprocedural = false;
      bool edges = true;
      bool mergedEdges = true;

      VirtualBinCFG::AuxiliaryInformation* info = auxiliaryInfoPrerequisite.getAuxiliaryInfo();

      std::string dfgFileName = "dfg.dot";
      graph = new RoseBin_DotGraph(info);
      ROSE_ASSERT(graph);

      const SgAsmInterpretationPtrList& interps = file->get_interpretations();
      ROSE_ASSERT (interps.size() == 1);
      dfanalysis = new RoseBin_DataFlowAnalysis(interps[0]->get_global_block(), forward, new RoseObj(), info);
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
