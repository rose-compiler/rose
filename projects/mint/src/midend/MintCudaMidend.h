
#ifndef _MINTCUDAMIDEND_H
#define _MINTCUDAMIDEND_H

#include "rose.h"

#include "rewrite.h"            //ast rewrite mechanism: insert, remove, replace
//#include "OmpAttribute.h"       //openmp pragmas are defined

//#include "arrayProcessing/MintArrayInterface.h"
//#include "memoryManagement/CudaMemoryManagement.h"

#include "../types/MintTypes.h"
#include "LoweringToCuda.h"  
#include "unparser.h"           //parses types into strings 


#include <iostream>
#include <string>
#include <vector>
#include <map>

#define DEBUG 1 

using namespace std;
using namespace OmpSupport;
using namespace SageBuilder;
using namespace SageInterface;


class MintCudaMidend
{
public:

  //inserts the cuda related header files 
  static int insertHeaders(SgProject*); 
  
  static void lowerMinttoCuda(SgSourceFile* file);


private: 

  static void transOmpMaster(SgNode * node);

  static void transOmpSingle(SgNode * node);

  static void transOmpFlushToCudaBarrier(SgNode * node);

  static void transOmpBarrierToCudaBarrier(SgNode * node);

  static void mintPragmasFrontendProcessing(SgSourceFile* file);

  //static void processDataTransferPragmas(SgSourceFile* file,
  //					 MintHostSymToDevInitMap_t& hostToDevVars);

  static void replaceMintParallelWithOmpParallel(SgSourceFile* file);
  static void replaceMintForWithOmpFor(SgSourceFile* file);

  static void processDataTransferPragmas(SgNode* node,
  					 MintHostSymToDevInitMap_t& hostToDevVars);

  static void processLoopsInParallelRegion(SgNode* parallelRegionNode, 
					   MintHostSymToDevInitMap_t hostToDevVars, 
					   ASTtools::VarSymSet_t& deviceSyms, 
					   MintSymSizesMap_t& trfSizes,
					   std::set<SgInitializedName*>& readOnlyVars,
					   const SgVariableSymbol* dev_struct);

  static void transOmpParallel(SgNode* node, 
			       MintHostSymToDevInitMap_t& hostToDevVars);

  static SgVariableSymbol* packArgsInStruct(SgNode* node, 
						 const MintHostSymToDevInitMap_t hostToDevVars);
};

#endif // _MINTCUDAMIDEND_H
