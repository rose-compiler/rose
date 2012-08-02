/*
 * CudaOptimizer.cpp
 *
 *  Created on: August 4, 2010
 *      Author: didem
 */
#include "CudaOptimizer.h"
#include "KernelMergeInterface.h"
#include "ASTtools.hh"
#include "./LoopUnroll/LoopUnrollOptimizer.h"
#include "./programAnalysis/MintConstantFolding.h"
#include "../midend/mintTools/MintOptions.h"
#include "../midend/arrayProcessing/MintArrayInterface.h"
#include "OnChipMemoryOptimizer/OnChipMemoryOpt.h"
#include <string>
#include "OmpAttribute.h"

#include "OptimizerInterface/CudaOptimizerInterface.h"
#include "programAnalysis/StencilAnalysis.h"

#define DEBUG_MODE 1

using namespace std;


CudaOptimizer::CudaOptimizer()
{
	// TODO Auto-generated constructor stub

}

CudaOptimizer::~CudaOptimizer()
{
	// TODO Auto-generated destructor stub
}

void CudaOptimizer::optimize(SgSourceFile* file)
{
 
  Rose_STL_Container<SgNode*> kernelList = NodeQuery::querySubTree(file, V_SgFunctionDefinition);
  Rose_STL_Container<SgNode*>::iterator kernel ;

  for (kernel = kernelList.begin() ; kernel != kernelList.end();  kernel++)
    {      
      SgFunctionDefinition* kernel_def = isSgFunctionDefinition(*kernel);

      SgFunctionDeclaration* kernel_decl = kernel_def -> get_declaration();

      string func_name = kernel_decl->get_name().getString() ;
      
      if(kernel_decl ->get_functionModifier().isCudaKernel())
	{

#ifdef DEBUG_MODE
	  cout << "  INFO:Mint:Found a kernel with name "<< func_name <<  " to optimize!"<< endl << endl;
#endif
	  //optimize(kernel_decl);
	}
    }
}


int CudaOptimizer::getNonStencilArrayRefCount(std::vector<SgExpression*> expList)
{
  //counts the references of an array where all the references are to the i,j 
  //not to the neighbours of i,j
  //For example: E[i][j], E[i][j], E[i-1][j] returns 2 because last one is stencil

  int count = 0 ;

  std::vector<SgExpression*>::iterator it;

  for(it = expList.begin(); it != expList.end() ; it++)
    {
      SgExpression* exp = (*it);

      SgExpression* arrayExp; 
      vector<SgExpression*>  subscripts; //first index is i if E[j][i]
      
      if(MintArrayInterface::isArrayReference(exp, &arrayExp, &subscripts))
	{
	  std::vector<SgExpression*>::iterator sub; 

	  bool center= true; 

	  for(sub = subscripts.begin(); sub != subscripts.end(); sub++)
	    {
	      if(! isSgVarRefExp(*sub))
		center = false ;
	    }
	  if(center)
	    count++;
	}
    }
  return count; 
}


SgType* getArrayElementType(const MintInitNameMapExpList_t &arrRefList)
{
  SgType* rt = NULL ;
  MintInitNameMapExpList_t::const_iterator it; 
  it = arrRefList.begin();
 // for(it = arrRefList.begin(); it != arrRefList.end(); it++)
 // {
    const SgInitializedName* array = it->first;
    ROSE_ASSERT(array != NULL);
    rt =array->get_type()->findBaseType();
  //}
  ROSE_ASSERT (rt != NULL);
  return rt;  
}
 
void CudaOptimizer::findCandidateVarForSharedMem(MintInitNameMapExpList_t arrRefList,
						 SgInitializedName* &candidateVar, 
						 SgInitializedName* prevCandidate, 
						 int &countMaxStencilRef,int &countNonStencilRef )
{
  countMaxStencilRef = 0;
  countNonStencilRef = 0;
  
  MintInitNameMapExpList_t::iterator it; 
  //find the candidate for shared memory 
  //go through all the array list to find the candidates

  //arrRefList contains the (array, expList)
  //ex: A -> A[i][j], A[i+1][j], A[i][j-1] ...

  for(it = arrRefList.begin(); it != arrRefList.end(); it++)
    {
      SgInitializedName* array = it->first;
      ROSE_ASSERT(array);
      
      std::vector<SgExpression*> expList = it->second;
      
      //TODO: we need a better mechanism to find the count of stencil references 
      //it is not enough to say that every non-[i][j] reference is stencil
      int countNonStencil = getNonStencilArrayRefCount(expList);
      int countStencilRef = expList.size() - countNonStencil;

      //need to check if candidate is the same as prevCandidate
      //because we may be looking for the second candidate 
      if(prevCandidate == NULL || prevCandidate->get_name().str() != array->get_name().str())
	{
	  //TODO: we need to make more roboust function for isStencilArray
	  if(MintArrayInterface::isStencilArray(expList))
	    {	
	      //these ones are canditates for shared memory
	      if(countMaxStencilRef < countStencilRef)
		{
		  candidateVar = array;
		  countMaxStencilRef = countStencilRef;
		  countNonStencilRef = countNonStencil ; //do we need this?
		}
	    }	
	}
    }

  if(candidateVar != NULL)
    cout << "  INFO:Mint: Candidate variable for shared memory opt: ("<< candidateVar->get_name().str() << ")"<< endl;
}


void CudaOptimizer::applyRegisterOpt(SgFunctionDeclaration* kernel,
				     DefUseAnalysis* defuse, 
				     std::set<SgInitializedName*> readOnlyVars,
				     MintInitNameMapExpList_t arrRefList,
				     SgInitializedName* candidateVar4Shared, 
				     SgInitializedName* sec_candidateVar4Shared, 
				     bool optUnrollZ) 
{
    MintInitNameMapExpList_t::iterator it; 
 
   //perform register optimizations 
    for(it = arrRefList.begin(); it != arrRefList.end(); it++)
     {
       SgInitializedName* array = it->first;
       ROSE_ASSERT(array);
       std::vector<SgExpression*> expList = it->second;       
       int countNonStencil = getNonStencilArrayRefCount(expList);
              
	//TODO: should I put a limit about how many variables 
	//should be put into the registers?
       if(countNonStencil > 0 )
	 {
	   string arrName = array->get_name().str ();
	   string arrName_sh = (candidateVar4Shared != NULL) ? candidateVar4Shared->get_name().str() : " ";  
	   string arrName_sh_sec = (sec_candidateVar4Shared != NULL) ? sec_candidateVar4Shared->get_name().str() : " ";  

	   //if we slide the planes, we perform the register opt to that variable later
	   //TODO: Check if that includes the shared memory opt only ? Yes, shared should be on to skip this opt
	   if((arrName == arrName_sh || arrName == arrName_sh_sec) && optUnrollZ ){
	     continue;
	   }
	   else
	    {
	      cout << "  INFO:Mint: Candidate variable for register opt ("<< array->get_name().str() << ")";
	      cout << " with # of refs : "<< countNonStencil<< endl ; 	      
	      cout << "  INFO:Mint: Applying register optimization to array ("<< arrName << ")"<< endl;
	      OnChipMemoryOpt::registerOptimizer(readOnlyVars, kernel, array);
	    }
	 }
     }
}

void CudaOptimizer::applyRegisterOpt(SgFunctionDeclaration* kernel,
				     std::set<SgInitializedName*> readOnlyVars,
				     const MintArrFreqPairList_t& candidateVarsShared,
				     const MintArrFreqPairList_t& candidateVarsReg,
				     const bool optUnrollZ) 
{
  //perform register optimizations, go through each element in the candidate list
  //the list is in decending order based on the number of references
  
  bool optShared = MintOptions::GetInstance()->isSharedOpt();

  MintArrFreqPairList_t::const_iterator it;   
  for(it = candidateVarsReg.begin(); it != candidateVarsReg.end(); it++)
    {
      MintArrFreqPair_t apair = (*it); 
      
      SgInitializedName* array = apair.first;
      ROSE_ASSERT(array);
      
      int freq = apair.second; 
      
      MintArrFreqPairList_t::const_iterator it2; 

      bool alsoShared = false;

      for(it2 = candidateVarsShared.begin(); it2 != candidateVarsShared.end(); it2++){
	  SgInitializedName* name = (*it2).first;

	  if(name == array){
	    alsoShared = true; break;
	  }
	}

      //if we slide the planes, we perform the register opt to that variable later
      //but if shared is ON, unroll is false, then we still perform register opt first
      //TODO: Check if that includes the shared memory opt only ? Yes, shared should be ON to skip this opt      
       if( optShared && alsoShared  && optUnrollZ ){
	continue;
      }

      //TODO: should I put a limit about how many variables should be put into the registers?
      if(freq > 0 ) //should I set it to 1?
	{
	  cout << "  INFO:Mint: Candidate variable for register opt ("<< array->get_name().str() << ")";
	  cout << " with # of refs : "<< freq<< endl ; 	      
	  cout << "  INFO:Mint: Applying register optimization to array ("<< array->get_name().str() << ")"<< endl;
	  OnChipMemoryOpt::registerOptimizer(readOnlyVars, kernel, array);
	}

      //We want to put that variable in register, if we apply shared memory optimization
      else if(alsoShared && optShared )
	{
	  cout << "  INFO:Mint: Candidate variable for register opt ("<< array->get_name().str() << ")";
	  cout << " with # of refs : "<< freq<< endl ; 	      
	  cout << "  INFO:Mint: Applying register optimization to array ("<< array->get_name().str() << ")"<< endl;
	  OnChipMemoryOpt::registerOptimizer(readOnlyVars, kernel, array, NULL, true);
	}

    }//end of for 
}


void CudaOptimizer::applySharedMemoryOpt(SgFunctionDeclaration* kernel,				     
					 const std::set<SgInitializedName*> readOnlyVars,
					 const MintArrFreqPairList_t& candidateVarsShared,
					 const MintForClauses_t& clauseList)
{
  //perform shared memory optimizations, go through each element in the candidate list
  //the list is in decending order based on the number of references
  //the list contains array name and how many planes it needs

  SgBasicBlock* kernel_body = kernel->get_definition()->get_body();

  int common_order =0 ; 

  //find the common order otherwise, we need to declare separate idx and idy, that's not good.
  MintArrFreqPairList_t::const_iterator it;   

  for(it = candidateVarsShared.begin(); it != candidateVarsShared.end(); it++)
    {
      MintArrFreqPair_t apair = (*it); 
      
      SgInitializedName* candidate = apair.first;
      ROSE_ASSERT(candidate);

      //we already computed, how many planes we will need 
      int num_planes = apair.second; 

      int dimension = MintArrayInterface::getDimension(candidate);

      if(dimension > 3 || dimension < 2)
	continue;

      int order = StencilAnalysis::performHigherOrderAnalysis(kernel_body, candidate, num_planes);

      //TODO: should we return if the order is 0 or too high ? 
      //what do you think? Yes, we should use registers if the order is 0, because there is no sharing. 
      
      if(order == 0){
	cout << "  INFO:Mint: Order of this array is " << order ;
	cout << ". No sharing, no need to use shared memory " << endl;
	continue ;
      }

      common_order = order > common_order ? order : common_order ;
    }

  common_order = common_order > MAX_ORDER ? MAX_ORDER : common_order ;
  cout << "  INFO:Mint: Common order of this kernel " << common_order << endl ;

  bool first = true; 
  for(it = candidateVarsShared.begin(); it != candidateVarsShared.end(); it++)
    {
      MintArrFreqPair_t apair = (*it); 
      
      SgInitializedName* candidate = apair.first;
      ROSE_ASSERT(candidate);
      
      //we already computed, how many planes we will need 
      int num_planes = apair.second; 

      int dimension = MintArrayInterface::getDimension(candidate);

      if(dimension > 3 || dimension < 2)
	continue;
      
      //TODO <---------------------------------------check this if it should be true or false? when only -opt:shared is set (not register)
      //bool regSharedSame = true; //refCountMaxRg > 0 ? true : false; we already inserted the register 

      cout << "  INFO:Mint: Applying shared memory optimization to array ("<< candidate->get_name().str() <<") ";
      cout << "using " << num_planes << " planes " << endl; 

      OnChipMemoryOpt::sharedMemoryOptimizer(kernel, readOnlyVars, candidate, clauseList, num_planes, first, common_order);

      first = false; 

    }//end of for 

  if(candidateVarsShared.size () > 0)
    OnChipMemoryOpt::insertSynchPoints(kernel);

}


void CudaOptimizer::applyLoopAggregationOpt(SgFunctionDeclaration* kernel,				     
					    const std::set<SgInitializedName*> readOnlyVars,
					    const MintArrFreqPairList_t& candidateVarsShared,
					    const MintArrFreqPairList_t& candidateVarsReg,
					    const MintForClauses_t& clauseList)
{
  //perform shared memory optimizations, go through each element in the candidate list
  //the list is in decending order based on the number of references
  //the list contains array name and how many planes it needs

  SgBasicBlock* kernel_body = kernel->get_definition()->get_body();

  int common_order =0 ; 

  //find the common order otherwise, we need to declare separate idx and idy, that's not good.
  MintArrFreqPairList_t::const_iterator it;

  for(it = candidateVarsShared.begin(); it != candidateVarsShared.end(); it++)
    {
      MintArrFreqPair_t apair = (*it); 
      
      SgInitializedName* candidate = apair.first;
      ROSE_ASSERT(candidate);

      //we already computed, how many planes we will need 
      int num_planes = apair.second; 

      int dimension = MintArrayInterface::getDimension(candidate);

      if(dimension > 3 || dimension < 2)
	continue;

      int order = StencilAnalysis::performHigherOrderAnalysis(kernel_body, candidate, num_planes);

      //TODO: should we return if the order is 0 or too high ?                                                
      //what do you think? Yes, we should use registers if the order is 0, because there is no sharing.                
                                                                                            
      if(order == 0){
	cout << "  INFO:Mint: Order of this array is " << order ;
	cout << ". No sharing, no need to use shared memory " << endl;
	continue ;
      }

      common_order = order > common_order ? order : common_order ;
    }

  common_order = common_order > MAX_ORDER ? MAX_ORDER : common_order ;
  cout << "  INFO:Mint: Common order of this kernel " << common_order << endl ;

  bool first = true; 
  for(it = candidateVarsShared.begin(); it != candidateVarsShared.end(); it++)
    {
      MintArrFreqPair_t apair = (*it); 
      
      SgInitializedName* candidate = apair.first;
      ROSE_ASSERT(candidate);
      
      //we already computed, how many planes we will need 
      int num_planes = apair.second; 

      int dimension = MintArrayInterface::getDimension(candidate);

      if(dimension > 3 || dimension < 2)
	continue;
      
      //TODO <--check this if it should be true or false? when only -opt:shared is set (not register)
      //bool regSharedSame = true; //refCountMaxRg > 0 ? true : false; we already inserted the register 

      cout << "  INFO:Mint: Applying Loop Aggregation optimization to array ("<<candidate->get_name().str();
      cout << " using " << num_planes << " planes )" << endl; 
      slidingRowOptimizer(kernel, readOnlyVars, candidate, clauseList, num_planes, common_order, first);
      first = false; 

    }//end of for 

  if(candidateVarsShared.size () > 0)
    OnChipMemoryOpt::insertSynchPoints(kernel);

}

void CudaOptimizer::optimize(SgFunctionDeclaration* kernel, MintForClauses_t clauseList)
{  
  //optimization steps 
  //1.step: check the optimization options
  //2.step: make an usedef analysis (currently, cannot use this effectively)
  //3.step: get array reference counts 
  //4.step: find which arrays are part of stencil accesses
  //5.step: the array with the stencil access should go to shared memory
  //6.step: if an array accessed many times but it is not stencil, then put that into registers.

  /*************Step 1 : check optimization options  **************************************/
 
  // we unroll short loops so that they can benefit from shared mem opt.
 
  bool optUnroll = MintOptions::GetInstance()->isUnrollOpt();
  if(optUnroll) //call unroll loop and constant folding if command line option is set
    {
      //bool loop_unrolled = LoopUnrollOptimizer::unrollShortLoops(kernel);
      //We have added a wrapper around the ROSE's constant folding opt
      //because ROSE's doesn't work properly
      MintConstantFolding::constantFoldingOptimization(kernel,false);
    }
 
  //optimization options
  bool optShared = MintOptions::GetInstance()->isSharedOpt();
  bool optRegister = MintOptions::GetInstance()->isRegisterOpt();

  //check kernel configuration parameters
  if (clauseList.tileDim.z != 1 && (clauseList.tileDim.z != clauseList.chunksize.z))
    {
      cout << "  WARNING:Mint: cannot apply shared memory optimization because tile(z) != chunksize(z) " << endl;
      optShared = false;
    }
  if(clauseList.chunksize.y != 1 || clauseList.chunksize.x != 1)
    {
      cout << endl;
      cout << "  WARNING:Mint: chunking optimization in y-dim or x-dim NOT implemented yet" << endl;
      cout << "  Set chunksize to 1 in x and y-dim to turn on on-chip memory optimizations" << endl << endl;      
      return;
    }

  bool optUnrollZ = clauseList.chunksize.z == 1 ? false : true ; 
  //bool optUnrollY = clauseList.chunksize.y == 1 ? false : true ;  //not implemented yet
  
  /*************end of Step 1 **************************************************************/


  /*************Step 2: find accesses frequencies and candidate arrays for optimizations ***/

  SgBasicBlock* kernel_body = kernel->get_definition()->get_body();
  ROSE_ASSERT(kernel_body);

  //commented out, ROSE's defuse analysis doesn't help us
  //find all use-def chain list in the kernel
  //DefUseAnalysis* defuse = new DefUseAnalysis(SageInterface::getProject());
  //defuse->start_traversal_of_one_function(kernel->get_definition());

  std::set<SgInitializedName*> readOnlyVars;
  SageInterface::collectReadOnlyVariables(kernel_body,readOnlyVars);
  
  //find all the array references in the kernel, group them under array names
  MintInitNameMapExpList_t arrRefList;
  MintArrayInterface::getArrayReferenceList(isSgNode(kernel_body), arrRefList); 

#ifdef VERBOSE_2
  cout << "  INFO:Mint: Found "<< arrRefList.size() <<" arrays in the kernel " << kernel->get_name().getString() << endl;
  cout << "  INFO:Mint: Searching candidate variables for on-chip memory optimization"<< endl;
#endif 

  SgType* element_type = getArrayElementType(arrRefList);
  ROSE_ASSERT(element_type);

  //we need to call this with order parameter, not here, we need to max order . 
  int num_planes = StencilAnalysis::howManyPlanesInSharedMemory(clauseList, element_type);
  
  MintArrFreqPairList_t candidateVarsShared; 
  MintArrFreqPairList_t candidateVarsReg;

  //finds the frequency of references for each array and 
  //categorize them as up/down, center, off-center
  StencilAnalysis::computeAccessFreq(arrRefList, num_planes, candidateVarsShared, candidateVarsReg);

  arrRefList.clear();

  /*************end of Step 2 **************************************************************/

  /*************Step 3: apply register optimizations first *********************************/
  if(optRegister){
    //If a variable is a candidate for shared memory and optUnrollZ is set 
    //then we hold on the register optimization for that variable until we apply loop aggregation opt
    cout << "  INFO:Mint: Register optimization is ON"<<endl;
    applyRegisterOpt(kernel, readOnlyVars, candidateVarsShared, candidateVarsReg, optUnrollZ);
  
  }
  /*************end of Step 3 **************************************************************/

  /*************Step 4: apply shared memory optimizations *********************************/
  if(optShared && ! (optRegister && optUnrollZ))
    {
      cout << endl<<"  INFO:Mint: Shared memory optimization is ON"<<endl;
      applySharedMemoryOpt(kernel, readOnlyVars, candidateVarsShared, clauseList);
    }


  if(optShared && optRegister && optUnrollZ)
    {
      applyLoopAggregationOpt(kernel, readOnlyVars, candidateVarsShared, candidateVarsReg, clauseList);
    }

  cout << "  INFO:Mint: Exiting optimizer"<< endl;

}


bool isSwappable(SgForStatement* cur_loop, SgIfStmt* cur_if)
{
  SgBasicBlock* loop_body = isSgBasicBlock(cur_loop -> get_loop_body());

  SgStatement* first_stmt = getFirstStatement(loop_body, true); //true: including compiler generated ones
  
  while(isSgBasicBlock(first_stmt)){

    first_stmt = getFirstStatement(isSgBasicBlock(first_stmt), true);//true: includeing compiler generated ones
  }

  if(isSgIfStmt(first_stmt )  && first_stmt ->unparseToString() == cur_if->unparseToString()) //we need more tests here
    return true;

  return false; 
}

void swapForWithIf(SgForStatement* tmp_loop, SgIfStmt* tmp_if, SgBasicBlock* bb)
{
  SgForStatement* cur_loop = deepCopy(tmp_loop);

  SgIfStmt* cur_if = deepCopy(tmp_if);

  SgStatement* if_body = cur_if->get_true_body();

  SgStatementPtrList &init = cur_loop ->get_init_stmt();

  SgStatement* newInitStmt = init.front();

  ROSE_ASSERT(newInitStmt != NULL);

  SgStatement* newTest = isSgStatement(cur_loop->get_test());

  ROSE_ASSERT(newTest != NULL);

  SgExpression* newIncrement = isSgExpression(cur_loop->get_increment());

  SgForStatement* newForStmt = buildForStatement(newInitStmt, newTest, newIncrement, if_body);

  SgStatement* newCondition = cur_if -> get_conditional();

  SgStatement* if_true_body = isSgStatement(newForStmt);

  SgIfStmt* newIfStmt = buildIfStmt( newCondition,if_true_body, NULL);

  ROSE_ASSERT(newIfStmt);

  appendStatement(newIfStmt, bb);

}
void CudaOptimizer::swapLoopAndIf(SgFunctionDeclaration* kernel, 
				  MintForClauses_t clauseList)
{
  //step 1: only if nesting level > 0 then consider swapping 
  //we only handle two cases

  //case 1: 
  // for ()            --- > if ()    
  //   for ()          --- >   for ()
  //     if ()         --- >     for ()
  //      {  }         --- >        { } 

  //case 2:
  // for ()            --- > if ()
  //   if ()           --- >   for () 
  //     if ()         --- >      for () 
  //      {  }         --- >        { }
 
  SgBasicBlock* kernel_body = kernel->get_definition()->get_body();

  if(clauseList.nested > 1 )
  {
    //case 1: two loops, 1 if 
    if(clauseList.chunksize.z != 1 && clauseList.chunksize.y != 1 )
    {
      //int swappable =  2 ; 
      //there has to be a for loop with gidz iterator
      Rose_STL_Container<SgNode*> forloops = NodeQuery::querySubTree(kernel_body, V_SgForStatement);      
      
      Rose_STL_Container<SgNode*>::reverse_iterator loopIt;

      for(loopIt = forloops.rbegin() ; loopIt != forloops.rend() ; loopIt++ )
      {	
	SgForStatement* cur_loop = isSgForStatement(*loopIt); 
            
	SgInitializedName* index_var = getLoopIndexVariable(cur_loop);
	
	string index_var_str = index_var->get_name().str();
	
	if(index_var_str == GIDZ && index_var_str == GIDY ) 
	{

	  SgBasicBlock* loop_body = isSgBasicBlock(cur_loop->get_loop_body());

	  Rose_STL_Container<SgNode*> ifs = NodeQuery::querySubTree(loop_body, V_SgIfStmt);

	  SgIfStmt* cur_if = isSgIfStmt(*(ifs.begin()));

	  SgBasicBlock* bb = buildBasicBlock();

	  insertStatementBefore(cur_loop, bb);

	  SgForStatement* for_stmt = deepCopy(cur_loop);
	  ROSE_ASSERT(for_stmt);

	  //add the loop into the new basic block                                                                                           
	  appendStatement(for_stmt, bb);

	  //if(true && swappable-- != 0 )
	  if(isSwappable(cur_loop, cur_if))
	  {
	    swapForWithIf(cur_loop, cur_if, bb); 
	  }
	  else {
	    //return 
	  }
	}
      } //end of for loopIt
    }  //end of case 1

    //only one for loop but there may be 1 or 2 ifs 
    if ( (clauseList.chunksize.z != 1 && clauseList.chunksize.y == 1) 
       || (clauseList.chunksize.z == 1 && clauseList.chunksize.y != 1 ))
    {

      int swappable = clauseList.nested - 1 ; 

      while(swappable-- > 0 )
      {
	//there has to be a for loop with gidz iterator
	Rose_STL_Container<SgNode*> forloops = NodeQuery::querySubTree(kernel_body, V_SgForStatement);      
	SgForStatement* cur_loop = isSgForStatement(*(forloops.begin())); 
	
	ROSE_ASSERT(cur_loop);
	
	SgInitializedName* index_var = getLoopIndexVariable(cur_loop);
	string index_var_str = index_var->get_name().str();
	
	if((index_var_str == GIDZ && clauseList.chunksize.z != 1) || 
	   (index_var_str == GIDY && clauseList.chunksize.y != 1) )
	{

	    SgBasicBlock* loop_body = isSgBasicBlock(cur_loop->get_loop_body());
	    
	    Rose_STL_Container<SgNode*> ifs = NodeQuery::querySubTree(loop_body, V_SgIfStmt);
	    
	    SgIfStmt* cur_if = isSgIfStmt(*(ifs.begin()));
	    
	    //add the loop into the new basic block                                                                                   
	    if(isSwappable(cur_loop, cur_if))
	    {	      
	      SgBasicBlock* bb = buildBasicBlock();
	      
	      insertStatementBefore(cur_loop, bb);
	      
	      swapForWithIf( cur_loop, cur_if , bb);
	      
	      removeStatement(cur_loop);
	      
	    }
	    else 
	      swappable = 0 ;
	}
      }//end of while
	
    } //end of case 2 
  }//end of if 
}



void CudaOptimizer::slidingRowOptimizer(SgFunctionDeclaration* kernel, 
					const std::set<SgInitializedName*>& readOnlyVars,
					SgInitializedName* candidateVar, 
					MintForClauses_t clauseList,
					int num_planes, int order, bool first)
{


  SgFunctionDefinition* func_def = kernel->get_definition();
  SgBasicBlock* kernel_body = func_def ->get_body();
  //if() opt:register flag is set? assumes it is already set 
  bool loadEvenCountZero = true;

  SgVariableSymbol* upper_sym = CudaOptimizerInterface::getSymbolFromName(kernel_body, "_upper" + GIDZ);
  //find the scope to declare the variable     
  SgScopeStatement* computeScope = CudaOptimizerInterface::findScopeForVarDeclaration(func_def, candidateVar);
  ROSE_ASSERT(computeScope);

  SgScopeStatement* upper_scope = NULL;
  if(upper_sym != NULL)
    upper_scope = upper_sym->get_scope();
  else 
    upper_scope = computeScope ; 

  ROSE_ASSERT(upper_scope);

  //load down into the register idz + 1
  SgInitializedName* downReg = OnChipMemoryOpt::registerOptimizer(readOnlyVars, kernel, candidateVar, 
								  computeScope, loadEvenCountZero, 1);
  ROSE_ASSERT(downReg);
 
  if (MintOptions::GetInstance()->isSharedOpt()){
    //bool regSameShared = true;
    //call shared memory optimization only if it is asked
    //int num_planes = 1;
    //int order = StencilAnalysis::performHigherOrderAnalysis(kernel_body, candidateVar, num_planes);
    OnChipMemoryOpt::sharedMemoryOptimizer(kernel, readOnlyVars, candidateVar, clauseList, num_planes, first, order);
  }


  //load center 
  SgInitializedName* centerReg = OnChipMemoryOpt::registerOptimizer(readOnlyVars, kernel, candidateVar, 
								    upper_scope, loadEvenCountZero);
  ROSE_ASSERT(centerReg);

  //load up idz - 1 
  SgInitializedName* upReg = OnChipMemoryOpt::registerOptimizer(readOnlyVars, kernel, candidateVar, 
								upper_scope, loadEvenCountZero, -1);
  ROSE_ASSERT(upReg);
  
  SgVarRefExp* upRef = buildVarRefExp(upReg, upper_scope);
  SgVarRefExp* downRef = buildVarRefExp(downReg, downReg->get_scope());
  SgVarRefExp* centerRef = buildVarRefExp(centerReg, upper_scope);

  SgStatement* swap1 = buildAssignStatement(upRef, centerRef);
  SgStatement* swap2 = buildAssignStatement(centerRef, downRef);

  //SgStatement* loopBody = for_loop->get_loop_body();
  //SgBasicBlock* loopBlock = isSgBasicBlock(loopBody);
  //ROSE_ASSERT(loopBlock);
  appendStatement(swap1, computeScope);
  appendStatement(swap2, computeScope);

  SgStatement *syncthreads= buildFunctionCallStmt("__syncthreads", buildVoidType(), NULL, computeScope);

  ROSE_ASSERT(syncthreads);
  appendStatement(syncthreads, computeScope );

  //step4:
  //checking if the variable is read-only 
  /*  if(!isReadOnly(readOnlyVars,candidateVar))
    {
      SgExprStatement * writeBack= buildAssignStatement(glmem , regExp);
      insertStatementAfter(inner_scope, writeBack);
    }
  */
}


void CudaOptimizer::getArrayReferenceCounts(SgBasicBlock* kernel_body, 
					    std::map<SgInitializedName*, int>& varCount_map)
{
  
  Rose_STL_Container<SgNode*> arrList = NodeQuery::querySubTree(kernel_body, V_SgPntrArrRefExp);  
  Rose_STL_Container<SgNode*>::iterator arr;
  
  for(arr = arrList.begin(); arr != arrList.end(); arr++)
    {
      SgExpression* arrayName; 
      vector<SgExpression*>  subscripts; //first index is i if E[j][i]
      
      //index list are from right to left 
      bool yes = MintArrayInterface::isArrayReference(isSgExpression(*arr), &arrayName, &subscripts);
      assert(yes);

      SgInitializedName *i_name = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayName));

      //this is the first time the var appears
      if(varCount_map.find(i_name) == varCount_map.end())
	varCount_map[i_name] = 0;
      
      varCount_map[i_name]++;

      int arrayDim = subscripts.size() ;
      
      arr = arr + arrayDim - 1;
    }
}
