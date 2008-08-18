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

      dfanalysis = new RoseBin_DataFlowAnalysis(file->get_global_block(), forward, new RoseObj(), info);
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
