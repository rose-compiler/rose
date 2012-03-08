/*
  A wrapper for the ROSE's constant folding optimization 
  
  It is very important for Mint to be able 
  eliminate all the constants in the index expression 
  to perform the on-chip memory optimizations. 

  We perform constant folding specificially on 
  the array index expressions. 

*/

#include "MintConstantFolding.h"
#include "../../midend/arrayProcessing/MintArrayInterface.h"

using namespace std;

MintConstantFolding::MintConstantFolding()
{


}

MintConstantFolding::~MintConstantFolding()
{


}

void MintConstantFolding::constantFoldingOptimization(SgNode* node, bool internalTestingAgainstFrontend)
{

  ROSE_ASSERT(node);

  cout << "  INFO:Mint: Applying constant folding optimization" << endl ; 

  //calling ROSE's constant folding optimizer first 
  ConstantFolding::constantFoldingOptimization(node, internalTestingAgainstFrontend);

  //if ROSE cannot figure out the constants in 
  //an expression like this one: A[i + 2 - 3 ]
  //then we call our own constant folding optimizer

  //constant fold the array index expressions  
  constantFoldingOnArrayIndexExpressions(node);

  //constant fold the upper bound of loop range
  //constantFoldingOnLoopRange(node);

}

//bool canWeFold(SgExpression* index, bool minusSign = false)
bool canWeFold(SgExpression* index, bool * minusSign)
{
  //we count the (-) signs affecting the index variable 
  //to understand if we should add a minus sign to the variable 
  //after folding or not
  
  int sign = 0;
  bool folding =true;

  Rose_STL_Container<SgNode*> constList = NodeQuery::querySubTree(index, V_SgIntVal);
  
  //we need at least 2 constants to fold
  if(constList.size() <= 1)
    {
      folding = false; 
    }

  Rose_STL_Container<SgNode*> indexVarExp = NodeQuery::querySubTree(index, V_SgVarRefExp);
  if(indexVarExp.size() == 0 ) //e.g A[1 + 2 - 3]
    {
      //ROSE handled this 
      folding = false; 
    }
  else if(indexVarExp.size() > 1) //e.g A[i + 2 - 2 + j - 1]
    {//cannot handle this case, it is not so common anyways
      folding = false; 
    }
  else //size() == 1 //e.g A[i + 2 - 2 - 1]
    {
      Rose_STL_Container<SgNode*> expList = NodeQuery::querySubTree(index, V_SgExpression);		      
      Rose_STL_Container<SgNode*>::iterator exp;
      
      for(exp=expList.begin(); exp != expList.end() ; exp++)
	{
	  SgExpression* expression= isSgExpression(*exp);

	  //All the binary operations should be -/+
	  if(isSgBinaryOp(expression))
	    {	      
	      if( !( isSgAddOp(expression) || isSgSubtractOp(expression) ) )
		{//we want all operators either - or +
		  folding = false;
		  break; 
		}	      
	      else if(isSgSubtractOp(expression)) 
		{//check if the binary operation changes the sign of the varref
		  SgExpression* rhs = isSgBinaryOp(expression)->get_rhs_operand();
		  ROSE_ASSERT(rhs);
		  Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(rhs, V_SgVarRefExp);		      
		  if(varList.size() > 0)
		    sign++;
		}
	    }
	  else if(isSgMinusOp(expression) ){	

	    SgExpression* rhs = isSgMinusOp(expression)->get_operand();
	    ROSE_ASSERT(rhs);
	    Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(rhs, V_SgVarRefExp);		      
	    if(varList.size() > 0)
	      sign++;
	    
	    }
	  else if(isSgIntVal(expression) || isSgVarRefExp(expression))
	    {
	      //do nothing
	    }
	  else 
	    {
	      folding = false; 
	      break;
	    }
	}      
    }

  if(sign % 2 != 0)
    *minusSign = true; 

  return folding;

}
void MintConstantFolding::constantFoldingOnArrayIndexExpressions(SgNode* node)
{
  //Assumptions: constant array indices are always integer
  /* we replace var ref exp with 0 and then put that back to the index var
     after constant folding 
     e.g let's say we have an expression A[i + 2 - 3]
     1.step we replace i with 0 (we previously check that all the operations are either -/+)
     if there is only one occurance of i in the index expression 
     2.step we call ROSE's constant folding function
     3.step we create new expression using the constant and i
     4.step replace it with the old index expression
     Tricky part is to figure out the sign of i.
  */
  
  MintInitNameMapExpList_t arrRefList;
  MintArrayInterface::getArrayReferenceList(node, arrRefList); 

  MintInitNameMapExpList_t::iterator it; 
  
  for(it = arrRefList.begin(); it != arrRefList.end(); it++)
    {
      SgInitializedName* array = it->first;
      ROSE_ASSERT(array);
      std::vector<SgExpression*> expList = it->second;       

      std::vector<SgExpression*>::iterator arrayNode;
      
      for(arrayNode = expList.begin(); arrayNode != expList.end() ; arrayNode++)
	{
	  ROSE_ASSERT(*arrayNode);	  

	  SgExpression* arrRefWithIndices = isSgExpression(*arrayNode);

	  SgExpression* arrayExp; 
	  vector<SgExpression*>  subscripts; //first index is i if E[j][i]
	  
	  if(MintArrayInterface::isArrayReference(arrRefWithIndices, &arrayExp, &subscripts))
	    {
	      std::vector<SgExpression*>::reverse_iterator sub;
	      
	      int subNo = subscripts.size() ;
 
	      for(sub= subscripts.rbegin(); sub != subscripts.rend(); sub++)
		{		  
		  subNo--;

		  //SgExpression* orig_index = *sub;
		  SgExpression* index = *sub;

		  bool minusSign = false; 
		  bool folding = canWeFold(index, &minusSign);

		  if(folding){
		    
		    Rose_STL_Container<SgNode*> indexVarExp = NodeQuery::querySubTree(index, V_SgVarRefExp);
		    ROSE_ASSERT(indexVarExp.size() ==1);
		    
		    SgExpression* indexVar = isSgExpression(*(indexVarExp.begin()));
		    SgExpression* orig_IndexVar = copyExpression(indexVar);
		    
		    replaceExpression(indexVar, buildIntVal(0));

		    ConstantFolding::constantFoldingOptimization(arrRefWithIndices);		      			
		    
 		    //TODO: need to figure out the sign of index var -/+
		    //insert back the index var i to the index expression -/+ i + constant index exp
		    //we computed that and stored its sign in minusSign variable 

		    SgExpression* newArrayExp; 
		    vector<SgExpression*>  newSubscripts; //first index is i if E[j][i]
		    
		    if(MintArrayInterface::isArrayReference(arrRefWithIndices, &newArrayExp, &newSubscripts))
		      {
			vector<SgExpression*>::iterator it= newSubscripts.begin();
			SgExpression* newIndex = *(it+subNo);
			
			ROSE_ASSERT(newIndex);
			
			if(isSgIntVal(newIndex))
			  {
			    int folded_val =  isSgIntVal(newIndex) ->get_value();
			    SgExpression* newExp = NULL;
			    
			    if(folded_val > 0 ){
			      if(minusSign)
				newExp = buildSubtractOp( buildIntVal(folded_val), orig_IndexVar);
			      else
				newExp = buildAddOp(orig_IndexVar , buildIntVal(folded_val));
			    }
			    else if (folded_val == 0 )
			      {
				newExp = orig_IndexVar;
				if(minusSign)
				  newExp = buildMinusOp(orig_IndexVar);				  
			      }
			    else {
			      if(minusSign)
				newExp = buildSubtractOp(buildMinusOp(orig_IndexVar) , buildIntVal(-folded_val));			 
			      else
				newExp = buildSubtractOp(orig_IndexVar , buildIntVal(-folded_val));
			    }
			    replaceExpression(newIndex, newExp);
			  }			
		      }
		  }//end of folding 

		}//end of for subscript
	      
	    }//end of if array exp
	  
	}//end of for references of an array

    }//end of array ref list 
}
