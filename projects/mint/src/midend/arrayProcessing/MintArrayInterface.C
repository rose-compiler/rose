
#include "MintArrayInterface.h"

#include "../mintTools/MintTools.h"
#include "../mintTools/MintOptions.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
//! Check if an expression is an array access. If so, return its name and subscripts if requested.
// Based on AstInterface::IsArrayAccess()                                                                                                 

bool MintArrayInterface::isArrayReference(SgExpression* ref, 
					  SgExpression** arrayName/*=NULL*/, 
					  vector<SgExpression*>* subscripts/*=NULL*/)
{
  SgExpression* arrayRef=NULL;

  if (ref->variantT() == V_SgPntrArrRefExp) {
    if (subscripts != 0 || arrayName != 0) {
      SgExpression* n = ref;
      while (true) {
	SgPntrArrRefExp *arr = isSgPntrArrRefExp(n);
	if (arr == 0)
	  break;
	n = arr->get_lhs_operand();
	// store left hand for possible reference exp to array variable                                                      
	if (arrayName!= 0)
	  arrayRef = n;
	// right hand stores subscripts                                                                                      
	if (subscripts != 0){
	  subscripts->push_back(arr->get_rhs_operand());
	  //cout << "sub: " << (arr->get_rhs_operand())->unparseToString() << endl;
	}
      } // end while                                       
      if  (arrayName !=NULL)
        {
          *arrayName = arrayRef;
        }
    }
    return true;
  }
  return false;
}

int MintArrayInterface::getDimension(const SgInitializedName* var, SgType* var_type)
{
  if(var_type == NULL)
    var_type = var->get_type();
  
  if(isSgArrayType (var_type))
    {
      return getDimensionCount(isSgArrayType(var_type)) ;
    }
  else if(isSgPointerType(var_type)) // float **                                                                                                                                         
    {
      int dim=0;
      while(true)
        {
          dim++;
          var_type = isSgPointerType(var_type)->get_base_type();

          ROSE_ASSERT(var_type);

          if(!isSgPointerType(var_type))
            return dim; // reached base type, we can return                                                                                                                              
        }
    }
  else if(isSgTypedefType(var_type))
    {

      SgType* base_type = isSgTypedefType(var_type)->get_base_type();

      return getDimension(var, base_type);

    }
  return 0;
}
void MintArrayInterface::linearizeArrays(SgSourceFile* file)
{
 
  Rose_STL_Container<SgNode*> kernelList = NodeQuery::querySubTree(file, V_SgFunctionDefinition);
  Rose_STL_Container<SgNode*>::iterator kernel ;

  for (kernel = kernelList.begin() ; kernel != kernelList.end();  kernel++)
    {      
      SgFunctionDefinition* kernel_def = isSgFunctionDefinition(*kernel);

      SgFunctionDeclaration* kernel_decl = kernel_def -> get_declaration();
    
      if(kernel_decl ->get_functionModifier().isCudaKernel())
	{
	  linearizeArrays(kernel_decl);
	}
    }

}

SgVariableSymbol* MintArrayInterface::getSymbolFromName(SgBasicBlock* block, string varStr)
{

  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(block, V_SgScopeStatement);

  NodeQuerySynthesizedAttributeType::const_iterator it = vars.begin();


  for(it= vars.begin(); it != vars.end(); it++)
    {
      SgScopeStatement* outer_scope = isSgScopeStatement(*(it));

      SgVariableSymbol* sym = outer_scope->lookup_var_symbol(varStr);

      if(sym!= NULL)
        return sym;
    }

  return NULL;
}

/*
void MintArrayInterface::findAllArrayRefExpressions(SgFunctionDeclaration* kernel, 
						    std::vector<SgExpression*>& arrRefList)
{

  //Step 1: Find all the pointer array reference expressions like E[i][j] 

  ROSE_ASSERT(kernel);

  SgBasicBlock* kernel_body = kernel->get_definition()->get_body();

  Rose_STL_Container<SgNode*> arrList = NodeQuery::querySubTree(kernel_body, V_SgPntrArrRefExp);

  Rose_STL_Container<SgNode*>::iterator arr;

  std::map<SgVariableSymbol*, string> sizeList; 

  for(arr = arrList.begin(); arr != arrList.end(); arr++)
    {
      SgExpression* exp = isSgExpression(*arr);
	
      if(isSgPntrArrRefExp(exp))
	{
	  //Step2 : Find arrayy reference name and index expressions
	  SgExpression* arrayName; 
	  vector<SgExpression*>  subscripts; //first index is i if E[j][i]
	  
	  //index list are from right to left 
	  bool yes = MintArrayInterface::isArrayReference(isSgExpression(*arr), &arrayName, &subscripts);
	  assert(yes == true);

	  //do not linearize the shared memory variables 
	  //TODO:med use the shared memory type to differentiate it 
	  //rather than its name. if the type is __shared__ then exclude that ref
	  
	  SgVarRefExp* arrRef = isSgVarRefExp(deepCopyNode(arrayName));
	  
	  SgVariableSymbol* sym= isSgVarRefExp(arrRef)->get_symbol();
	  
	  ROSE_ASSERT(sym);
	  
	  string arr_str = sym->get_name().str() ; 
	  
	  //step 3: if the array dimension is greater than 1, then convert the reference to 1-dim array
	  if(subscripts.size() > 1  && arr_str.find(SHARED_BLOCK) == -1 ) {

	    arrRefList.push_back(exp);

	    arr += subscripts.size() - 1 ; //skip sub array refences
	  }
	  
	}
    }
}
*/

void createGlobalIndexExpForAllArrays(SgScopeStatement* kernel_body,
				      MintInitNameMapExpList_t refList, 
				      SgDeclarationStatement* src_location)
{
  //we need to exclude the ones for shared memory
  //step 4 create an index expression for each distinct array 
  //e.g. _gidx + _gidy * widthUnew  in 2D
  //e.g. _gidx + _gidy * widthUnew + _gidz * sliceUnew in 3D

  ROSE_ASSERT(src_location);

  SgScopeStatement* indexScope= src_location->get_scope();

  SgVariableSymbol* gidz_sym = MintArrayInterface::getSymbolFromName(isSgBasicBlock(kernel_body), GIDZ);
  SgVariableSymbol* gidy_sym = MintArrayInterface::getSymbolFromName(isSgBasicBlock(kernel_body), GIDY);

  MintInitNameMapExpList_t::iterator arr; 

  //for each array, create an index expression
  for(arr= refList.begin(); arr!= refList.end(); arr++)
    {
      SgInitializedName* iname = arr->first;

      int dim = MintArrayInterface::getDimension(iname);

      ROSE_ASSERT(dim <= 3);

      if(dim == 3 && gidz_sym == NULL)
	continue; //must be a boundary condition, we don't use index opt. optimization for those

      if(dim == 2 && gidy_sym == NULL)
	continue; //must be a boundary condition, we don't use index opt. optimization for those

      string arrStr = iname->get_name().str();

      //_gidx + _gidy * widthUnew  in 2D
      //_gidx + _gidy * widthUnew + _gidz * sliceUnew in 3D
      SgExpression* indexExp = NULL;
      indexExp = buildVarRefExp(GIDX, indexScope); 

      if(dim >= 2)
	indexExp = buildAddOp(indexExp, buildMultiplyOp(buildVarRefExp(GIDY, indexScope), 
							buildVarRefExp("width"+arrStr, kernel_body)));
      if(dim == 3 ){
	indexExp = buildAddOp(indexExp, buildMultiplyOp(buildVarRefExp(GIDZ, indexScope), 
							buildVarRefExp("slice"+arrStr, kernel_body)));
      }
      SgAssignInitializer* initIndex = buildAssignInitializer(indexExp);

      SgVariableDeclaration* index = buildVariableDeclaration("index" + arrStr, 
							      buildIntType(), initIndex, indexScope);

      //step 5 insert index expression in the kernel
      ROSE_ASSERT(index);
      insertStatementAfter(src_location, index );

      //step 6 check if there is a loop, if there is we need to update the index 
      //but we only update if the loop makes changes in the array reference 
      std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(kernel_body,V_SgForStatement);
      
      if(loopNest.size() > 0)
	{
	  SgForStatement* loop = *(loopNest.begin());
	  SgBasicBlock* loop_body = isSgBasicBlock(loop->get_loop_body());
	  SgStatement* update_stmt = buildAssignStatement(buildVarRefExp(index),indexExp);
	  ROSE_ASSERT(update_stmt);
	  prependStatement(update_stmt, loop_body);
	}      
    }
} 



void createOneGlobalIndexExpForAllArrays(SgScopeStatement* kernel_body,
					 MintInitNameMapExpList_t refList, 
					 SgDeclarationStatement* src_location)
{
  //We declare one index variable for all arrays: one for 1D, one for 2D, one for 3D
  //Need a compiler flag for this

  //e.g. _gidx + _gidy * widthUnew  in 2D
  //e.g. _gidx + _gidy * widthUnew + _gidz * sliceUnew in 3D

  ROSE_ASSERT(src_location);

  bool threeD = false; 
  bool twoD = false; 
  bool oneD = false;

  SgScopeStatement* indexScope= src_location->get_scope();

  SgVariableSymbol* gidz_sym = MintArrayInterface::getSymbolFromName(isSgBasicBlock(kernel_body), GIDZ);
  SgVariableSymbol* gidy_sym = MintArrayInterface::getSymbolFromName(isSgBasicBlock(kernel_body), GIDY);
  SgVariableSymbol* gidx_sym = MintArrayInterface::getSymbolFromName(isSgBasicBlock(kernel_body), GIDX);

  MintInitNameMapExpList_t::iterator arr; 

  //for each array, we don't create an index expression
  for(arr= refList.begin(); arr!= refList.end(); arr++)
    {
      SgInitializedName* iname = arr->first;

      int dim = MintArrayInterface::getDimension(iname);

      ROSE_ASSERT(dim <= 3);

      if(dim == 3 && (gidz_sym == NULL || threeD == true))
	continue; //must be a boundary condition, we don't use index opt. optimization for those

      if(dim == 2 && (gidy_sym == NULL || twoD == true))
	continue; //must be a boundary condition, we don't use index opt. optimization for those

      if(dim == 1 && (gidx_sym == NULL || oneD == true))
	continue; //must be a boundary condition, we don't use index opt. optimization for those

      string arrStr = iname->get_name().str();

      //_gidx + _gidy * widthUnew  in 2D
      //_gidx + _gidy * widthUnew + _gidz * sliceUnew in 3D
      SgExpression* indexExp = NULL;
      indexExp = buildVarRefExp(GIDX, indexScope); 

      string index_str = "index";

      if(dim ==1){
	  index_str = "_" +index_str + "1D";
	  oneD = true; 
	}
      else if(dim == 2){
	index_str = "_" + index_str + "2D";
	twoD = true; 
	indexExp = buildAddOp(indexExp, buildMultiplyOp(buildVarRefExp(GIDY, indexScope), 
							buildVarRefExp("_width", kernel_body)));
      }
      else if(dim == 3 ){
	indexExp = buildAddOp(indexExp, buildMultiplyOp(buildVarRefExp(GIDY, indexScope), 
							buildVarRefExp("_width", kernel_body)));
	indexExp = buildAddOp(indexExp, buildMultiplyOp(buildVarRefExp(GIDZ, indexScope), 
							buildVarRefExp("_slice", kernel_body)));
	index_str = "_"+index_str + "3D";
	threeD = true;
      }
      SgAssignInitializer* initIndex = buildAssignInitializer(indexExp);

      SgVariableDeclaration* index = buildVariableDeclaration(index_str,
							      buildIntType(), initIndex, indexScope);

      //step 5 insert index expression in the kernel
      ROSE_ASSERT(index);
      insertStatementAfter(src_location, index );

      //step 6 check if there is a loop, if there is we need to update the index 
      //but we only update if the loop makes changes in the array reference 
      std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(kernel_body,V_SgForStatement);
      
      if(loopNest.size() > 0)
	{
	  SgForStatement* loop = *(loopNest.begin());
	  SgBasicBlock* loop_body = isSgBasicBlock(loop->get_loop_body());
	  SgStatement* update_stmt = buildAssignStatement(buildVarRefExp(index),indexExp);
	  ROSE_ASSERT(update_stmt);
	  prependStatement(update_stmt, loop_body);
	}      
    }
} 


bool canWeSimplfyIndexExpression(vector<SgExpression*>  subscripts)
{
  //valid means that we can use common subexpression elimination.

  //simplfy means that we can simplfy the index expression (subexpression elimination) 
  //by rewriting it in terms of index_arrname + offset (some offset)
  //false: we cannot figure out the offset,then we leave it as it is. 

  bool valid = false ;
  int indexNo = 0 ; 
  
  vector<SgExpression*>::iterator it;	

  for(it = subscripts.begin(); it != subscripts.end(); it++)
    {
      //looking for minus, plus a small constant: i-c, j+c
      SgExpression* index = isSgExpression(*it);
      ROSE_ASSERT(index);
      
      indexNo++;
      
      Rose_STL_Container<SgNode*> expListInIndex = NodeQuery::querySubTree(index, V_SgExpression);      
      for(Rose_STL_Container<SgNode*>::iterator one_exp = expListInIndex.begin(); one_exp != expListInIndex.end(); one_exp++)
	{
	  SgExpression* tmp_exp = isSgExpression(*one_exp);
	  
	  //these are only allowable expressions that appear in the index expression
	  //we are interested in [i +|- c ]
	  if( isSgAddOp(tmp_exp) || isSgSubtractOp(tmp_exp) || isSgIntVal(tmp_exp) || isSgVarRefExp(tmp_exp)){
	      valid = true; 
	    }
	  else
	    {
	      return false;
	    } 	    
	}

      //looking for minus, plus 1s: i-1, j+1
      Rose_STL_Container<SgNode*> constExp = NodeQuery::querySubTree(index, V_SgIntVal);      
      Rose_STL_Container<SgNode*> indexVarExp = NodeQuery::querySubTree(index, V_SgVarRefExp);
      Rose_STL_Container<SgNode*> binaryOp = NodeQuery::querySubTree(index, V_SgBinaryOp);
     
      //we allow only one constant in the index expression 
      //because we already apply constant folding 
      
      if(constExp.size() > 1)
	return false; 
      
      else if(binaryOp.size() > 1)
	return false; 
      
      else if(indexVarExp.size() < 1 || indexVarExp.size() > 2)
	return false;
      
      else if(indexVarExp.size() == 2)
	{//one of them should idx, y, z other can be anything 
	  Rose_STL_Container<SgNode*>::iterator it = indexVarExp.begin();

	  SgExpression* first = isSgExpression(*(it));
	  SgExpression* second = isSgExpression(*(++it));

	  string first_str = first->unparseToString();
	  string second_str = second->unparseToString();

	  if(indexNo == 1) //gidx
	    {
	      if(first_str.find(GIDX) == string::npos && second_str.find(GIDX) == string::npos)
		return false;
	    }
	  else if (indexNo == 2)
	    {
	      if(first_str.find(GIDY) == string::npos && second_str.find(GIDY) == string::npos)
		return false; 
	    }
	  else if (indexNo == 3)
	    {
	      if(first_str.find(GIDZ) == string::npos && second_str.find(GIDZ) == string::npos)
		return false; 
	    }
	  else
	    return false;

	}
      else // there is a least one variable 
	{
	  SgExpression* varExp = isSgExpression(*(indexVarExp.begin())); 
	  string varExp_str = varExp->unparseToString();

	  if(indexNo == 1) //gidx
	    {
	      if(varExp_str.find(GIDX) == string::npos)
		return false;
	    }
	  else if (indexNo == 2)
	    {
	      if(varExp_str.find(GIDY) == string::npos)
		return false; 
	    }
	  else if (indexNo == 3)
	    {
	      if(varExp_str.find(GIDZ) == string::npos)
		return false; 
	    }
	  else
	    return false;
	}

    }//end of indices of a single array reference

  return valid;

}

SgExpression* MintArrayInterface::linearizeThisArrayRefs(SgNode* arrayNode, 
							 SgBasicBlock* kernel_body, 
							 SgScopeStatement* indexScope, 
							 SgInitializedName* arr_iname, 
							 bool useSameIndex) 
					
{
	//an instance of an array reference E[k][j+1][i]
	SgExpression* arrRefWithIndices = isSgExpression(arrayNode);

	SgExpression* arrayName;

	//index list are from right to left //first index is i if E[j][i]                                                                                                                                     
	vector<SgExpression*>  subscripts; 

	//get each subscripts of an array reference
	bool yes = MintArrayInterface::isArrayReference(arrRefWithIndices, &arrayName, &subscripts);
	ROSE_ASSERT(yes);

	if(subscripts.size() <= 1)
	  return NULL; //no need to flatten

	//step 7
	//simplfy means that we can simplfy the index expression (subexpression elimination) 
	//by rewriting it in terms of index + some offset instead of computing the offset from address 0.
	//if simplfy is false, then we cannot figure out the offset. We need to use offset from the address zero.

	bool simplfy = canWeSimplfyIndexExpression(subscripts);
	
	string arr_str = arr_iname -> get_name().str();
	string index_str = "index" + arr_str;
	string width_str = "width" + arr_str;
	string slice_str = "slice" + arr_str;

	if(useSameIndex)
	  {
	    width_str = "_width";
	    slice_str = "_slice";

	    if(subscripts.size() == 3){
	      index_str = "_index3D";
	    }
	    else if(subscripts.size() == 2){
		index_str = "_index2D";
	      }
	    else if(subscripts.size() == 1){
		index_str = "_index1D";
	      }
	  }

	//base offset, we need to find the relative offset 
	//index expression can be [k +|- c]
	SgExpression* indexExp = buildVarRefExp(index_str, indexScope);	

	if(simplfy) //continue step 7
	  {
	    int indexNo=0;
	    
	    vector<SgExpression*>::iterator index_itr = subscripts.begin();
	    
	    for(index_itr = subscripts.begin(); index_itr != subscripts.end(); index_itr++)
	      {
		//We assume that if there is binary operation, then it has to be 
		//plus/minus i,j,k with a constant
		//Otherwise it is just i,j,k
		SgExpression* index = *index_itr;
		ROSE_ASSERT(index);

		//looking for minus, plus 1s: i-1, j+1 or just i,j,k
		Rose_STL_Container<SgNode*> constExp = NodeQuery::querySubTree(index, V_SgIntVal);      
		Rose_STL_Container<SgNode*> indexVarExp = NodeQuery::querySubTree(index, V_SgVarRefExp);
		Rose_STL_Container<SgNode*> subtractExp = NodeQuery::querySubTree(index, V_SgSubtractOp);
		Rose_STL_Container<SgNode*> addExp = NodeQuery::querySubTree(index, V_SgAddOp);
		
		indexNo++;
		
		if(subtractExp.size() == 1 || addExp.size() == 1) // it is addOp or subtractOp
		  {
		    //e.g. [k][j-1][i+1] becomes [index - 1* width + 1]
		    //optimize it further if the constant is 1 or 0.
		    if(indexVarExp.size() == 1)
		      {
			ROSE_ASSERT(constExp.size() == 1);
			
			SgIntVal* constVal = isSgIntVal(*(constExp.begin()));
			int constIntVal = constVal->get_value();
			SgExpression* offset = NULL;
			
			if(indexNo == 1) //gidx
			  {
			    offset = buildIntVal(constIntVal);
			  }
			else if (indexNo==2) //idy
			  {
			    offset = buildVarRefExp(width_str, kernel_body) ; 
			    if(constIntVal != 1)
			      offset = buildMultiplyOp(offset, buildIntVal(constIntVal));
			  }
			else if(indexNo==3) //idz
			  {
			    offset = buildVarRefExp(slice_str, kernel_body) ; 
			    if(constIntVal != 1)
			      offset = buildMultiplyOp(offset, buildIntVal(constIntVal));
			  }
			if(constIntVal != 0 )
			  {
			    ROSE_ASSERT(offset);
			    if(subtractExp.size() == 1)
			      indexExp = buildSubtractOp(indexExp, offset);
			    else if (addExp.size() == 1)
			      indexExp = buildAddOp(indexExp, offset);
			  }
		      }
		    else if (indexVarExp.size() == 2)
		      { //Added in (3 March 2011) to handle boundary loads
			//they are typically A[z][y][x + borderOffset] so it is worth to optimize them
			
			Rose_STL_Container<SgNode*>::iterator it = indexVarExp.begin();	      
			SgVarRefExp* first = isSgVarRefExp(*it);
			SgVarRefExp* second = isSgVarRefExp(*(++it));
			
			ROSE_ASSERT(first);
			ROSE_ASSERT(second);
			
			string secondRef = second->unparseToString();
			
			SgExpression* offset =copyExpression(first);
			
			if(secondRef.find(GIDX) == string::npos && secondRef.find(GIDY) && string::npos && secondRef.find(GIDZ) == string::npos)
			  {//borderOffset is the second var 
			    offset = copyExpression(second);
			  }
			
			if (indexNo==2) //idy
			  {
			    offset = buildMultiplyOp(offset,buildVarRefExp(width_str, kernel_body)) ; 
			  }
			if(indexNo==3) //idz
			  {
			    offset = buildMultiplyOp(offset, buildVarRefExp(slice_str, kernel_body)) ; 
			  }
			
			ROSE_ASSERT(offset);
			if(subtractExp.size() == 1)
			  indexExp = buildSubtractOp(indexExp, offset);
			else if (addExp.size() == 1)
			  indexExp = buildAddOp(indexExp, offset);
			
		      }
		    else {
		      ROSE_ABORT();
		    }
		  }
		else 
		  {
		    ROSE_ASSERT(constExp.size() == 0);
		    //do nothing because it is in the base index
		  }		
	      } //end of subscript loop 
	    
	  }//end of simplfy = true 
	
	else  //step 8
	  {
		
	    //index expression cannot be simplfied 
	    //e.g A[0][n-1][i-j+1][0+3-i]
	    //output:
	    //e.g. _gidx + _gidy * widthUnew  in 2D
	    //e.g. _gidx + _gidy * widthUnew + _gidz * sliceUnew in 3D
	    
	    vector<SgExpression*>::iterator index_itr = subscripts.begin();
	    
	    //first index is i if E[j][i]           
	    ROSE_ASSERT(*index_itr);
	    
	    //first index
	    indexExp = deepCopy(*index_itr);
	    
	    if(subscripts.size() >= 2) //second index
	      {
		index_itr++; 
		ROSE_ASSERT(*index_itr);
		
		SgVarRefExp* sizeExp = buildVarRefExp(width_str, kernel_body);   
		ROSE_ASSERT(sizeExp);
		
		indexExp = buildAddOp(indexExp, buildMultiplyOp( deepCopy(*index_itr), sizeExp));
	      }
	    if (subscripts.size() == 3) //third index
	      {
		index_itr++; 
		ROSE_ASSERT(*index_itr);
		
		SgVarRefExp* sizeExp2 = buildVarRefExp(slice_str, kernel_body);
		ROSE_ASSERT(sizeExp2);
		
		indexExp = buildAddOp(indexExp, buildMultiplyOp( deepCopy(*index_itr), sizeExp2)); 
		
	      }
	    
	    ROSE_ASSERT(subscripts.size() <= 3);
	    
	  }//end of !simplfy
	
	return indexExp;

}//end of indexList of one array


void MintArrayInterface::linearizeArrays(SgFunctionDeclaration* kernel)
{
  //1.step : find all the array references in the kernel
  //2.step : group the array references accoding to the distinct array names
  //don't forget to skip the ones with shared memory (added in march 1st 2011)
  //3.step : find the insertion location in the code
  //4.step : create index expression for each array
  //5.step : insert index expression in that location
  //6.step : check if there is loop, if there is we need to update index exp in the loop
  //7.step : replace index expression in the array references, it may require +- some offset.   
  //8.step : there are some expression that we cannot do common-subexp elimination (simplfy them)
  //such as E[0][n-1]

  ROSE_ASSERT(kernel);
  
  SgBasicBlock* kernel_body = kernel->get_definition()->get_body();
  ROSE_ASSERT(kernel_body);

  //step 1-2: find all the array references in the kernel body and group them according to the array name
  MintInitNameMapExpList_t refList;
  
  bool sharedMemRef = false;
  getArrayReferenceList(kernel_body, refList , sharedMemRef); 

  //step 3 Find the location where we are goind to insert "index" variable
  SgDeclarationStatement* src_location = NULL;
  SgVariableSymbol* gidz_sym = MintArrayInterface::getSymbolFromName(kernel_body, GIDZ);
  SgVariableSymbol* gidy_sym = MintArrayInterface::getSymbolFromName(kernel_body, GIDY);
  SgVariableSymbol* gidx_sym = MintArrayInterface::getSymbolFromName(kernel_body, GIDX);

  if(gidz_sym != NULL){
    SgInitializedName* gidz_name = gidz_sym->get_declaration();
    ROSE_ASSERT(gidz_name);
    src_location = gidz_name ->get_declaration();

  }
  else if (gidy_sym != NULL)
    {
      SgInitializedName* gidy_name = gidy_sym->get_declaration();
      ROSE_ASSERT(gidy_name);
      src_location = gidy_name ->get_declaration();
    }
  else if(gidx_sym != NULL) 
    {
      SgInitializedName* gidx_name = gidx_sym->get_declaration();
      ROSE_ASSERT(gidx_name);
      src_location = gidx_name ->get_declaration();
    }
  ROSE_ASSERT(src_location);
  
  SgScopeStatement* indexScope= src_location->get_scope();

  //steps 4-5-6 
  //e.g. int index = x + y * width + z * slice;
  bool useSameIndex = MintOptions::GetInstance()->useSameIndex();

  if(useSameIndex)
    createOneGlobalIndexExpForAllArrays(kernel_body, refList, src_location);
  else //this one will create an index expression distinct to each array
    createGlobalIndexExpForAllArrays(kernel_body, refList, src_location);

  //contains all the (old, new) expressions. Old will be replaced with new. 
  //MintExp2ExpList_t replaceList; 
  //int count = 0;
    
  //SgExpression* newArrRefWithIndices=NULL ;
  //SgExpression* arrRefWithIndices = NULL;
 
  MintInitNameMapExpList_t::iterator arr_ref;

  //iterating over all the arrays and their reference list 
  for(arr_ref = refList.begin(); arr_ref != refList.end(); arr_ref++)
  {
    
    SgInitializedName* arr_iname = arr_ref->first;
    ROSE_ASSERT(arr_iname);

    //list of all the references of an array in the kernel
    std::vector<SgExpression*> indexList = arr_ref->second;
    
    std::vector<SgExpression*>::reverse_iterator arrayNode;

    //iterating over all the references of an array 
    for(arrayNode = indexList.rbegin(); arrayNode != indexList.rend() ; arrayNode++)
      {
	ROSE_ASSERT(*arrayNode);

	//an instance of an array reference E[k][j+1][i]
	SgExpression* arrRefWithIndices = isSgExpression(*arrayNode);

	SgExpression* indexExp = linearizeThisArrayRefs(*arrayNode, kernel_body, indexScope, arr_iname, useSameIndex);
	
	if(indexExp != NULL)
	  {
	    SgVarRefExp* arrRef = buildVarRefExp(arr_iname, kernel_body);
	    ROSE_ASSERT(arrRef);
	    ROSE_ASSERT(indexExp);
	    
	    SgExpression* newArrRefWithIndices = buildPntrArrRefExp(deepCopy(arrRef), indexExp);
	    
	    ROSE_ASSERT(newArrRefWithIndices);

	    //replace old reference with subscripted array access with flattened index reference
	    replaceExpression(arrRefWithIndices, newArrRefWithIndices);

	    //cout << arrRefWithIndices -> unparseToString() << endl; 
	    //cout << newArrRefWithIndices -> unparseToString() << endl; 
	    /*
	    if(replaceList.find(arrRefWithIndices) == replaceList.end())
	      {
		if(count < 10)
		  {
		    cout << arrRefWithIndices << "   " ;
		    cout << arrRefWithIndices -> unparseToString() << endl;
		    cout << newArrRefWithIndices << "   " ; 
		    cout << newArrRefWithIndices -> unparseToString() << endl; 
		  }
		replaceList[arrRefWithIndices] = newArrRefWithIndices;
		count++;
	      }
	    else 
	      {		
		cout << arrRefWithIndices << "   " ;
		cout << arrRefWithIndices -> unparseToString() << endl;
		cout << newArrRefWithIndices << "   " ; 
		cout << newArrRefWithIndices -> unparseToString() << endl; 
		
		cout << "Opps !!!, they share the address, not good" << endl;
		}*/

	    /*
	    SgTreeCopy treeCopy;
	    
	    SgExpression *newVar = isSgExpression( newArrRefWithIndices->copy( treeCopy ) );
	    ROSE_ASSERT ( newVar );
	    
	    SgExpression *parent = isSgExpression( arrRefWithIndices->get_parent() );
	    ROSE_ASSERT ( parent );
	    
	    // replace the node in the AST                                                                                                                                                                
	    newVar->set_parent( parent );
	      
	    int k = parent->replace_expression( arrRefWithIndices, newVar );
	    ROSE_ASSERT ( k == 1 );
	    */	    
	    //delete arrRefWithIndices; 
	  }
      }
    
  }//end of array list (each array has a reference list 

  /*  
  for(MintExp2ExpList_t::iterator old = replaceList.begin(); old!= replaceList.end() ; old++)
    {
      SgExpression* oldExp = (*old).first;
      SgExpression* newExp = (*old).second;

      if(oldExp != NULL && newExp != NULL)
	{
	  replaceExpression(oldExp, newExp);
	  count--;	  
	}
    }
  */

}//end of linearize function 


bool MintArrayInterface::isStencilArray(std::vector<SgExpression*> expList) 			
{

//takes all the references of an array in a exp list 
//returns if the accesses are to the north, south, east or west
//Assumes arrays are not represented in 1-dim. (do I handle only 2D? )
//Example :expList contains  E[i][j], E[i][j+1], E[i][j-1] then return "yes" it is stencil
//        :expList contains  E[i][j], E[i][j], E[i][j] then return NO.
//we need a better way to determine if an accesses is strided access.
  
  std::vector<SgExpression*>::iterator it; 

  for(it = expList.begin(); it != expList.end(); it++)
    {
      SgExpression* exp = (*it);

      Rose_STL_Container<SgNode*> arrList = NodeQuery::querySubTree(exp, V_SgPntrArrRefExp);
  
      Rose_STL_Container<SgNode*>::iterator arr;
  
      for(arr = arrList.begin(); arr != arrList.end(); arr++)
	{
	  
	  SgExpression* arrayName; 
	  vector<SgExpression*>  subscripts; //first index is i if E[j][i]
	  
	  //index list are from right to left 
	  bool yes = MintArrayInterface::isArrayReference(isSgExpression(*arr), &arrayName, &subscripts);
	  assert(yes);
	  
	  vector<SgExpression*>::iterator indexExp;

	  for(indexExp = subscripts.begin(); indexExp != subscripts.end(); indexExp++)
	    {
	      //looking for minus, plus 1s: i-1, j+1
	      Rose_STL_Container<SgNode*> constExp = NodeQuery::querySubTree(*indexExp, V_SgIntVal);
	      
	      Rose_STL_Container<SgNode*> indexVarExp = NodeQuery::querySubTree(*indexExp, V_SgVarRefExp);

	      //should I check if it is 0 ?
	      if(constExp.size() > 0 && indexVarExp.size()> 0){
		return true;
	      }
	    }

	  int arrayDim = subscripts.size() ;
	  
	  arr = arr + arrayDim - 1;
	  
	}
    }
  return false;
}

void MintArrayInterface::getArrayReferenceList(SgNode* kernel_body, 
					       MintInitNameMapExpList_t& refList,
					       bool sharedMemRef) 
{
  //sharedMemRef= true, then return shared memory references as well
  //finds all the array references and returns them in a vector
  //groups the array references according to their init name
  //returns a map of expression vector where array ref name is the key
  //For example: E-> E[i][j], E[i+1][j], E[i][j]
  //             R-> R[i][j]

  Rose_STL_Container<SgNode*> arrList = NodeQuery::querySubTree(kernel_body, V_SgPntrArrRefExp);
  
  Rose_STL_Container<SgNode*>::iterator arr;
  
  if(sharedMemRef)
    {
      for(arr = arrList.begin(); arr != arrList.end(); arr++)
	{
	  
	  SgExpression* arrayName; 
	  vector<SgExpression*>  subscripts; //first index is i if E[j][i]
	  
	  //index list are from right to left 
	  bool yes = MintArrayInterface::isArrayReference(isSgExpression(*arr), &arrayName, &subscripts);
	  assert(yes);
	  
	  SgInitializedName *i_name = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayName));
	  
	  ROSE_ASSERT(i_name);
	  	  
	  //this is the first time the var appears
	  if(refList.find(i_name) == refList.end())
	    {
	      
	      std::vector<SgExpression*> exp;
	      exp.push_back(isSgExpression(*arr));
	      
	      pair< MintInitNameMapExpList_t ::iterator, bool> ret;
	      
	      ret = refList.insert(make_pair(i_name, exp));
	      
	    }
	  else
	    {
	      refList[i_name].push_back(isSgExpression(*arr));
	    }
	  
	  int arrayDim = subscripts.size() ;
	  arr = arr + arrayDim - 1;
	}
    } // end of if 
  else //exclude shared memory references
    {
      for(arr = arrList.begin(); arr != arrList.end(); arr++)
	{
	  
	  SgExpression* arrayName; 
	  vector<SgExpression*>  subscripts; //first index is i if E[j][i]
	  
	  //index list are from right to left 
	  bool yes = MintArrayInterface::isArrayReference(isSgExpression(*arr), &arrayName, &subscripts);
	  assert(yes);
	  
	  SgInitializedName *i_name = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayName));
	  
	  ROSE_ASSERT(i_name);
	  
	  //if( ! ( i_name->get_declarationModifier().get_storageModifier().isCudaShared()))

	  //it is not reliable, that's why I check SHARED_BLOCK later on 
	  if( ! ( i_name->get_storageModifier().isCudaShared())) 
	    {
	      string i_name_str = i_name->get_name().str();

	      //if(i_name_str.find(SHARED_BLOCK) == - 1) 
	      if(i_name_str.find(SHARED_BLOCK) == string::npos) 
		 {
		   //this is the first time the var appears
		   if(refList.find(i_name) == refList.end())
		     {
		       
		       std::vector<SgExpression*> exp;
		       exp.push_back(isSgExpression(*arr));
		       
		       pair< MintInitNameMapExpList_t ::iterator, bool> ret;
		       
		       ret = refList.insert(make_pair(i_name, exp));		  
		     }
		   else
		     {
		       refList[i_name].push_back(isSgExpression(*arr));
		     }
		 }	      
	    }
	  int arrayDim = subscripts.size() ;
	  arr = arr + arrayDim - 1;	  
	}
    }
}
