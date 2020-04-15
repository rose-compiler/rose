#ifndef PARPRO_OPTIONS_H
#define PARPRO_OPTIONS_H

#include "Options.h"
#include <vector>
#include <string>

struct ParProOptions : public Options {
  int seed;
  std::string generateAutomata;
  int numAutomata;
  std::string numSyncsRange;
  std::string numCirclesRange;
  std::string circlesLengthRange;
  std::string numIntersectionsRange;
  std::string automataDotInput;
  bool keepSystems;
  std::string useComponents;
  std::string fixedSubsets;
  int numRandomComponents;
  bool parallelCompositionOnly;
  int numComponentsLtl;
  int minimumComponents;
  int differentComponentSubsets;
  std::string ltlMode;
  int mineNumVerifiable;
  int mineNumFalsifiable;
  int miningsPerSubset;
  std::string ltlPropertiesOutput;
  std::string promelaOutput;
  bool promelaOutputOnly;
  bool outputWithResults;
  bool outputWithAnnotations;
  std::string verificationEngine;
};

#endif
