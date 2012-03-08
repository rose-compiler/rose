/*
 * CudaOptimizer.cpp
 *
 *  Created on: August 4, 2010
 *      Author: didem
 */

#include "KernelMergeInterface.h"
#include "../midend/CudaOutliner.h"
#include "ASTtools.hh"

//#include "../types/MintTypes.h"
#include "../midend/arrayProcessing/MintArrayInterface.h"
#include <string>
#include "OmpAttribute.h"

using namespace std;


KernelMergeInterface::KernelMergeInterface()
{
	// TODO Auto-generated constructor stub

}

KernelMergeInterface::~KernelMergeInterface()
{
	// TODO Auto-generated destructor stub
}


bool KernelMergeInterface::mergeKernels(SgFunctionDeclaration* kernel1, 
					SgFunctionDeclaration* kernel2)
{
  //step0: create the function definition
  //step1 create a new function name
  //step2 combine first's kernels parameter list with the second kernel's 
  //step3 create a new function parameter list 

  SgBasicBlock* func_body = buildBasicBlock();

  SgBasicBlock* kernel_body1 = kernel1->get_definition()->get_body();
  //SgBasicBlock* kernel_body2 = kernel2->get_definition()->get_body();

  func_body = deepCopy (kernel_body1);
  /*  
  string kernel_name = CudaOutliner::generateFuncName(func_body);

  SgGlobal* g_scope = SageInterface::getGlobalScope(kernel1);
  ROSE_ASSERT(g_scope != NULL);

  ASTtools::VarSymSet_t  deviceSyms, psyms;

  //Generates the body of the outlined function         
  SgFunctionDeclaration* kernel = CudaOutliner::generateFunction (func_body, kernel_name,
                                                                  deviceSyms, psyms, psyms, g_scope);

  //this adds the kernel into the source code and its declaration outside of main      
  Outliner::insert(kernel, g_scope, func_body);
  */
  return true;
}
