  // The prerequisite for getting the SgProject
  class AuxiliaryInfoPrerequisite: public Prerequisite {
    VirtualBinCFG::AuxiliaryInformation* info;
  public:
    AuxiliaryInfoPrerequisite(): Prerequisite("Auxiliary"), info(NULL) {}
    void run(SgProject* project) {
      if (done) return;

//      SgAsmFile* file = project->get_file(0).get_binaryFile();
      info = new VirtualBinCFG::AuxiliaryInformation(project);
      done = true;
    }

    PrerequisiteList getPrerequisites() const {
      return PrerequisiteList();
    }

    VirtualBinCFG::AuxiliaryInformation* getAuxiliaryInfo() const {
      ROSE_ASSERT (done);
      return info;
    }
  };

extern AuxiliaryInfoPrerequisite auxiliaryInfoPrerequisite;
