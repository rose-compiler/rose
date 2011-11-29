#ifndef _MINTPRAGMAS_
#define _MINTPRAGMAS_

#include "rose.h"

#include "rewrite.h"            //ast rewrite mechanism: insert, remove, replace
#include "OmpAttribute.h"       //openmp pragmas are defined

#include "../../types/MintTypes.h"

#include "../arrayProcessing/MintArrayInterface.h"
#include "../memoryManagement/CudaMemoryManagement.h"

#include "unparser.h"           //parses types into strings 


#include <iostream>
#include <string>
#include <vector>
#include <map>


using namespace std;
using namespace OmpSupport;
using namespace SageBuilder;
using namespace SageInterface;


class MintPragmas
{
public:
  static MintForClauses_t getForParameters(SgNode* node);

  static bool isMintPragma(SgNode* node);

  static bool isTransferToFromDevicePragma(SgNode* node, bool verbose=false);
  static bool isForLoopPragma(SgNode* node, bool verbose=false);
  static bool isParallelRegionPragma(SgNode* node);
  static bool isSinglePragma(SgNode* node);

  static MintTrfParams_t getTransferParameters(SgNode* node, 
					       const MintHostSymToDevInitMap_t );

  static void processForLoopMintPragmas(SgOmpForStatement* omp_for, 
					MintForClauses_t& clauses);
  
};

#endif // _MINTPRAGMAS_
