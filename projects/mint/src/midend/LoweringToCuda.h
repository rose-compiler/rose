/*!
 * Translation (directive lowering) support for OpenMP 3.0 C/C++
 *
 *  Like other OpenMP related work within ROSE, 
 *  all things go to the OmpSupport namespace to avoid conflicts
 * Liao, 8/10/2009
 */
#ifndef LOWERINGTOCUDA_H
#define LOWERINGTOCUDA_H

#include "CudaOutliner.h"
#include "../types/MintTypes.h"

//#include "ASTtools.hh"
//#include "VarSym.hh"
#include <map>
#include <vector>
class LoweringToCuda
{

 public:
  static SgFunctionDeclaration* transOmpFor(SgNode* node, 
					    MintHostSymToDevInitMap_t hostToDevVars,
					    ASTtools::VarSymSet_t& deviceSyms,
					    MintForClauses_t&,
					    const SgVariableSymbol* dev_struct);


  static bool isBoundaryConditionLoop(SgNode*);
  static bool isStencilLoop(SgNode*);

  //static void linearizeArrays(SgNode* node);

 private:

  static void unpackCudaPitchedPtrs(SgFunctionDeclaration* func,
				    const MintHostSymToDevInitMap_t hostToDevVars,
				    const SgVariableSymbol* dev_struct,
				    const ASTtools::VarSymSet_t deviceSyms);
  
  static void getArrayRefNameAndIndexList(SgNode* node, SgVarRefExp* arrRefExp, 
					  std::set<SgExpression*>& indexList );
 
  static SgVariableDeclaration* threadIndexCalculations(SgBasicBlock* outerBlock, SgForStatement* , int loopNo, bool isStencil, MintForClauses_t clauseList);
 
  static void  nestedForLoops(SgFunctionDeclaration* kernel, MintForClauses_t clauseList);

  static SgBasicBlock* processLoops(SgForStatement* cur_loop, SgBasicBlock* outerBlock, int loopNo, bool isStencil,
				    MintForClauses_t);

  static SgVariableDeclaration* generateCudaGetLastErrorStatement(string suffix, SgScopeStatement* scope);

  static SgStatement* getCudaErrorStringStatement(SgVarRefExp* errVar,
						  SgScopeStatement* scope, 
						  string func_name);

  static SgFunctionDeclaration* generateOutlinedTask(SgNode* node, 
						     ASTtools::VarSymSet_t& syms,
						     std::set<SgInitializedName*>& readOnlyVars,
						     MintHostSymToDevInitMap_t hostToDevVars,
						     const SgVariableSymbol* dev_struct,
						     ASTtools::VarSymSet_t& syms_withStruct);
  static void addInvYBlockNumDecl(SgFunctionDeclaration* func);  

};
#endif //CUDA_LOWERING_H

