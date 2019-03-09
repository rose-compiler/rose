#ifndef COMPASS_AUXILIARY_INFO_PREREQUISITE_H
#define COMPASS_AUXILIARY_INFO_PREREQUISITE_H

  // The prerequisite for getting the SgProject
  class AuxiliaryInfoPrerequisite: public Prerequisite {

// This will not be compiled unless binary support is turned on, Liao 2019/2/13
#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT)
    VirtualBinCFG::AuxiliaryInformation* info;
#endif
  public:
    AuxiliaryInfoPrerequisite(): Prerequisite("Auxiliary"){
#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT)
    info =NULL; 
#endif
}
    void run(SgProject* project) {
      if (done) return;

#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT)
      info = new VirtualBinCFG::AuxiliaryInformation(project);
#endif
      done = true;
    }

    PrerequisiteList getPrerequisites() const {
      return PrerequisiteList();
    }

#if defined(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT)
    VirtualBinCFG::AuxiliaryInformation* getAuxiliaryInfo() const {
      ROSE_ASSERT (done);
      return info;
    }
#endif
  };

extern AuxiliaryInfoPrerequisite auxiliaryInfoPrerequisite;

#endif
