#include <iostream>
// ROSE analyses
#include "rose.h"
#include "VariableRenaming.h"

#include "Labeler.h"

namespace Experimental {

class RoseRDAnalysis {
 public:
  static void generateRoseRDDotFiles(SPRAY::Labeler* labeler, SgProject* root);
 private:
  static void generateRoseRDDotFile(VariableRenaming* varRen,std::string filename);
  static void generateRoseRDDotFile2(SPRAY::Labeler* labeler, VariableRenaming* varRen,std::string filename);
  static SgNode* normalizeAstPointer(SgNode* node);
  static SgNode* normalizeAstPointer2(SgNode* node);
};

}
