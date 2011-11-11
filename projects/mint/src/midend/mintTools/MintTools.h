

#include "rose.h"

#include <vector>
#include <iostream>
#include <string>
#include <sstream>
//#include "../mintPragmas/MintPragmas.h"

#include "../../types/MintTypes.h"

using namespace std;
using namespace SageInterface;

class MintTools
{

 public:

  static string generateArgName(const SgStatement* stmt);
  static string generateFuncName(const SgStatement* stmt);
  static string generateGridDimName(const SgStatement* stmt);
  static string generateBlockDimName(const SgStatement* stmt);
  static string generateCopyParamName(const SgStatement* stmt);
  static string generateDeviceVarName(const SgStatement* stmt);


  static void setOutputFileName(SgFile* cur_file);
  static void forLoopIndexRanges(SgNode* node, MintInitNameExpMap_t& index_ranges);

  static SgOmpParallelStatement* getEnclosingParallelRegion(SgNode* n,bool includingself=false);

  static SgInitializedName* findInitName(SgNode* node, string var_name);
  static void printAllExpressions(SgNode* node);
  static void printAllStatements(SgNode* node);


};
