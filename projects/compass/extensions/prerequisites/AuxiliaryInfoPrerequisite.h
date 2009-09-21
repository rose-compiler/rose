#ifndef COMPASS_AUXILIARY_INFO_PREREQUISITE_H
#define COMPASS_AUXILIARY_INFO_PREREQUISITE_H

  // The prerequisite for getting the SgProject
  class AuxiliaryInfoPrerequisite: public Prerequisite {
    VirtualBinCFG::AuxiliaryInformation* info;
  public:
    AuxiliaryInfoPrerequisite(): Prerequisite("Auxiliary"), info(NULL) {}
    void run(SgProject* project) {
      if (done) return;

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

#endif
