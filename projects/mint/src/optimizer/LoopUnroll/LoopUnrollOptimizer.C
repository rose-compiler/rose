/*

  Loop Unrolling Optimizer
  
  Sometimes we need to perform loop unrolling in order to 
  apply shared memory optimizations. 
  This applies to small range loops, 
  typically sweeping the weight coefficient array. 
  Exaxmple application: convolution

*/

#include "LoopUnrollOptimizer.h"
#include "ASTtools.hh"
#include <string>

#include <iostream>
#include <sstream>

#include "../programAnalysis/MintConstantFolding.h"
#include "../../types/MintTypes.h"
using namespace std;

LoopUnrollOptimizer::LoopUnrollOptimizer()
{


}

LoopUnrollOptimizer::~LoopUnrollOptimizer()
{


}


bool LoopUnrollOptimizer::unrollShortLoops(SgFunctionDeclaration* kernel)
{
  //sometimes we cannot perform shared memory optimization 
  //unless we unroll the loop such as in convolution 
  //Unroll the loops starting from the innermost to outmost
  //We unroll only short loops

  bool isUnrolled = false; 

  SgBasicBlock* kernel_body = kernel->get_definition()->get_body();

  Rose_STL_Container<SgNode*> forLoops = NodeQuery::querySubTree(kernel_body, V_SgForStatement);

  Rose_STL_Container<SgNode*>::reverse_iterator it;
  //unroll from inner to outer 
  for(it=forLoops.rbegin(); it != forLoops.rend(); it++)
    {
      SgForStatement* forLoop = isSgForStatement(*it);

      ROSE_ASSERT(forLoop);

      int unrolling_factor = findUnrollingFactor(forLoop);

      isUnrolled = LoopUnrollOptimizer::unrollShortLoop(forLoop, unrolling_factor);

      //we exit if we can not unroll the inner loops
      if(!isUnrolled)
	break;
    }

  return isUnrolled;
}


int LoopUnrollOptimizer::findUnrollingFactor(SgForStatement* target_loop)
{
  // return 1 if the loop range is not a constant. 
  // conditions: loop lower bound and upper bound should be an expression 
  // based on the same variable +/- a constant
  // e.g. lb = i-2 and ub = i+2 then unrolling factor is 5 (inclusive). 

  //1.step grab the target loop's essential header info
  //2.step check the lb and ub is an expression of same variable
  //3.step grab the constants in the lb ad ub expression 
  //4.step if step 2 is true, then perform constant folding in ub-lb 

  //TODO: ROSE's constant folding is not working properly, 
  //We may not be able to find the unrolling factor because of that.
  //e.g. k < i-2 when normalized it becomes k <= i-2 -1 

  int unrolling_factor = 1;

  SgScopeStatement* scope = target_loop->get_scope();
  ROSE_ASSERT(scope != NULL);

  // grab the target loop's essential header information
  SgInitializedName* ivar = NULL;
  SgExpression* lb = NULL;
  SgExpression* ub = NULL;
  SgExpression* step = NULL;
  SgStatement* orig_body = NULL;

  //TODO: We need to normalize the loop first, because < <= makes a difference  
  if (!forLoopNormalization(target_loop))
  { // the return value is not reliable
    //cerr<<"Error in SageInterface::loopUnrolling(): target loop cannot be normalized."<<endl;
    //    dumpInfo(target_loop);
    //    return false;
  }

  //normalization may introduce -/+ 1 to the lb or ub
  //call constant folding optimization to get rid off them (not reliable)
  ConstantFolding::constantFoldingOptimization(target_loop);
 
  if (!isCanonicalForLoop(target_loop, &ivar, &lb, &ub, &step, &orig_body))
  {
    cerr<<"Error in SageInterface::loopUnrolling(): target loop is not canonical."<<endl;
    dumpInfo(target_loop);
    return false;
  }
  
  ROSE_ASSERT(ivar&& lb && ub && step);
  ROSE_ASSERT(isSgBasicBlock(orig_body));

  SgExpression* raw_range_exp=NULL;

  //extract the variable references in the loop lower and upper  bound
  Rose_STL_Container<SgNode*> varList_lb = NodeQuery::querySubTree(lb, V_SgVarRefExp);
  Rose_STL_Container<SgNode*> varList_ub = NodeQuery::querySubTree(ub, V_SgVarRefExp);

  if(varList_lb.size() > 1 || varList_ub.size() > 1) 
    { //there are two or more variables involved in the bound expression
      //it is hard to justify that loop range is a constant
      return unrolling_factor ; 
    }
  else if(varList_lb.size() == 1 && varList_ub.size() == 1)
    { 
      SgVarRefExp* lb_exp = isSgVarRefExp(*(varList_lb.begin()));
      SgVarRefExp* ub_exp = isSgVarRefExp(*(varList_ub.begin()));
     
      ROSE_ASSERT(lb_exp); 
      ROSE_ASSERT(ub_exp);

      SgInitializedName *lb_name = SageInterface::convertRefToInitializedName(lb_exp);
      SgInitializedName *ub_name = SageInterface::convertRefToInitializedName(ub_exp);
      
      ROSE_ASSERT(lb_name); 
      ROSE_ASSERT(ub_name);
     
      //variable names should be the same lb=i-2 up=i+2 
      if(lb_name->get_name().str() != ub_name->get_name().str()){
	  return unrolling_factor ; 
      }
      
      //extract the operator  in the loop lower and upper  bound
      Rose_STL_Container<SgNode*> optList_lb = NodeQuery::querySubTree(lb, V_SgBinaryOp);
      Rose_STL_Container<SgNode*> optList_ub = NodeQuery::querySubTree(ub, V_SgBinaryOp);

      //extract the constant references in the loop lower and upper  bound
      Rose_STL_Container<SgNode*> constList_lb = NodeQuery::querySubTree(lb, V_SgIntVal);
      Rose_STL_Container<SgNode*> constList_ub = NodeQuery::querySubTree(ub, V_SgIntVal);

      SgBinaryOp* binaryOperator_lb = NULL ; 
      SgBinaryOp* binaryOperator_ub = NULL ; 

      if(optList_lb.size() > 0 )
	binaryOperator_lb  = isSgBinaryOp(*(optList_lb.begin()));

      if(optList_ub.size() > 0)
	binaryOperator_ub  = isSgBinaryOp(*(optList_ub.begin()));
      
      //if the constant folding doesn't work correctly in ROSE, we can not handle the cases where constList.size > 1 
      if(constList_lb.size() > 1  || constList_ub.size() > 1 ){
	return unrolling_factor ; 
      }
      
      SgExpression* const_val_lb = buildIntVal(0);
      SgExpression* const_val_ub = buildIntVal(0);
      
      if(constList_lb.size() == 1){
	const_val_lb = isSgIntVal(*(constList_lb.begin()));	  
      }
      if(constList_ub.size() == 1){
	const_val_ub = isSgIntVal(*(constList_ub.begin()));	  
      }

      raw_range_exp =buildSubtractOp(buildAddOp(const_val_ub, buildIntVal(1)),(const_val_lb));
      
      bool minus_ub = false; 
      if(binaryOperator_ub != NULL)
	{
	  switch (binaryOperator_ub ->variantT())
	    {
	    case V_SgSubtractOp:
	      {	
		minus_ub = true; 
		break;
	      }
	    case V_SgAddOp:
	      {
		break;
	      }
	    default:
	      {//we don't handle other operations
		return unrolling_factor;
		break;
	      }
	    }
	}

      if(binaryOperator_lb != NULL)
	{
	  switch (binaryOperator_lb->variantT())
	    {
	    case V_SgSubtractOp:
	      {

		raw_range_exp =buildAddOp(buildAddOp(const_val_ub, buildIntVal(1)),(const_val_lb));

		if(minus_ub)		
		  raw_range_exp =buildAddOp(buildSubtractOp(buildIntVal(1), const_val_ub),(const_val_lb));
		//const_val_lb = buildMinusOp(const_val_lb);	
		break;
	      }
	    case V_SgAddOp:
	      {
		raw_range_exp =buildSubtractOp(buildAddOp(const_val_ub, buildIntVal(1)),(const_val_lb));

		if(minus_ub)		
		  raw_range_exp =buildSubtractOp(buildSubtractOp(buildIntVal(1), const_val_ub),(const_val_lb));
		//const_val_lb = buildMinusOp(const_val_lb);	
		break;
	      }
	    default:
	      {//we don't handle other operations
		return unrolling_factor;
		break;
	      }
	    }
	}

      ROSE_ASSERT(raw_range_exp);
    }

  else if(varList_lb.size() == 0 && varList_ub.size() == 0 ) 
    { 
      //both have a constant
      //range = ub+1-lb
      //SgExpression* raw_range_exp =buildSubtractOp(buildAddOp(ub,buildIntVal(1)),(lb));
    
      raw_range_exp =buildSubtractOp(buildAddOp(copyExpression(ub),buildIntVal(1)),
				     copyExpression(lb));
    }

  ROSE_ASSERT(raw_range_exp);
  raw_range_exp->set_need_paren(true);
  
  ROSE_ASSERT(raw_range_exp);
  
  //Didem: I don't need to create variable declaration but need to use it for constant folding otherwise
  // I cannot apply constant folding on the raw_range_exp
  string unroll_fac = "_unroll_factor";
  SgVariableDeclaration* fringe_decl = buildVariableDeclaration(unroll_fac, buildIntType(),
								buildAssignInitializer(raw_range_exp), scope);

  ROSE_ASSERT(fringe_decl);
  
  SgInitializedName* init_name = getFirstInitializedName(fringe_decl);
  
  SgInitializer* rhs_const = init_name ->get_initializer();

  ConstantFolding::constantFoldingOptimization(rhs_const);
  
  ROSE_ASSERT(rhs_const);

  SgIntVal* rhs_val = isSgIntVal(rhs_const);

  if(rhs_val != NULL)
    unrolling_factor = rhs_val->get_value();
  else {    
    //convert string to integer
    stringstream ss(rhs_const->unparseToString().c_str()); 
    
    if( (ss >> unrolling_factor).fail()){
      return -1;
    }
  }

  return unrolling_factor;  
}


// Didem, Feb 21, 2011: 
/* I had to modify the existing loop unrolling procedure in ROSE 
   because it creates fringe even though the loop range might be 
   very small and can be completely ignored. We do static analysis 
   to avoid all the loops resulting from  unrolling. 
   For example, in convolution kernels, loop bounds can be [i-2 to i+2]
   as a result, we can unroll 5 times to completely get rid off the loops.
 */


bool LoopUnrollOptimizer::unrollShortLoop(SgForStatement* target_loop, size_t unrolling_factor)
{
  //unrolls short range loops with no fringe

  ROSE_ASSERT(target_loop);

  //Handle 0 and 1, which means no unrolling at all
  //we have a upper bound as well
  if (unrolling_factor <= 1 || unrolling_factor > (size_t) SHORT_LOOP_UNROLL_FACTOR)
    return false;

  // normalize the target loop first
  if (!forLoopNormalization(target_loop));
  {// the return value is not reliable
    //    cerr<<"Error in SageInterface::loopUnrolling(): target loop cannot be normalized."<<endl;
    //    dumpInfo(target_loop);
    //    return false;
  }

  cout << "  INFO:Mint: Unrolling a short loop (unrolling factor: "<<unrolling_factor << ")"<< endl;

   //SgScopeStatement* scope = target_loop->get_scope();
  // grab the target loop's essential header information
  SgInitializedName* ivar = NULL;
  SgExpression* lb = NULL;
  SgExpression* ub = NULL;
  SgExpression* step = NULL;
  SgStatement* orig_body = NULL;

  if (!isCanonicalForLoop(target_loop, &ivar, &lb, &ub, &step, &orig_body))
  {
    cerr<<"Error in SageInterface::loopUnrolling(): target loop is not canonical."<<endl;
    dumpInfo(target_loop);
    return false;
  }
  ROSE_ASSERT(ivar&& lb && ub && step);
  ROSE_ASSERT(isSgBasicBlock(orig_body));

  //unrolled_body will have the body of the target loop * unrolling_factor many bodies
  SgBasicBlock* unrolled_body= buildBasicBlock();
  ROSE_ASSERT(unrolled_body);
  insertStatementBefore(target_loop,unrolled_body);

   SgBinaryOp* step_bin_op = isSgBinaryOp(step->get_parent());
   ROSE_ASSERT(step_bin_op != NULL);
   
   bool isPlus = false;
   if (isSgPlusAssignOp(step_bin_op))
     isPlus = true;
    else if (isSgMinusAssignOp(step_bin_op))
      isPlus = false;
    else
    {
      cerr<<"Error in SageInterface::loopUnrolling(): illegal incremental exp of a canonical loop"<<endl;
      dumpInfo(step_bin_op);
      ROSE_ASSERT(false);
    }

   // copy loop body factor unrolling_factor times, and replace reference to ivar  with ivar +/- step*[1 to factor-1]
   for (size_t i =0; i< unrolling_factor; i++)
   {
     SgBasicBlock* body = isSgBasicBlock(deepCopy(target_loop->get_loop_body())); // normalized loop has a BB body
     ROSE_ASSERT(body);
     // replace reference to ivar with ivar +/- step*i

     std::vector<SgVarRefExp*> refs = querySubTree<SgVarRefExp> (body, V_SgVarRefExp);
 
     for (std::vector<SgVarRefExp*>::iterator iter = refs.begin(); iter !=refs.end(); iter++)
     {
       SgVarRefExp* refexp = *iter;
       SgExpression* new_exp = NULL;

       if (refexp->get_symbol()==ivar->get_symbol_from_symbol_table())
       {
         //build replacement  expression if it is NULL
         if (new_exp == NULL)
         {
           if (isPlus) {//ivar +/- step * i	     
	     SgExpression* tmp = deepCopy(lb);
	     //tmp->set_need_paren(false);
	     new_exp = buildAddOp(tmp,buildMultiplyOp(deepCopy(step),buildIntVal(i)));
	     
	   }
           else{
	     SgExpression* tmp = deepCopy(lb);
	     //tmp->set_need_paren(false);
	     new_exp = buildSubtractOp(tmp,buildMultiplyOp(deepCopy(step),buildIntVal(i)));
	   }
	 }
         // replace it with the right one
         replaceExpression(refexp, new_exp);
       }
     }
     // copy body to loop body, this should be a better choice
     // to avoid redefinition of variables after unrolling (new scope is introduced to avoid this)
     
     appendStatement(body, unrolled_body);
     //appendStatement(body,isSgBasicBlock(orig_body));
    // moveStatementsBetweenBlocks(body,isSgBasicBlock(orig_body));
   }

   // remove the target loop finally
   // it is used for buffering the original loop body before in either cases
   removeStatement(target_loop);

   return true;
}
