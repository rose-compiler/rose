
#include "rose.h"
#include "Outliner.hh"
#include "DataTransferSizes.h"

#include "rewrite.h"            //ast rewrite mechanism: insert, remove, replace                                   
#include "OmpAttribute.h"       //openmp pragmas are defined                                                       
#include "unparser.h"  

#include "../arrayProcessing/MintArrayInterface.h"

#include <algorithm>

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;


void DataTransferSizes::printTransferSizes(MintSymSizesMap_t& trfSizes)
{
  MintSymSizesMap_t ::iterator it;

  for(it= trfSizes.begin(); it!= trfSizes.end() ; it++)
    {

      SgInitializedName* n = (it->first)->get_declaration ();
      string var_name = n->get_name().str();

      std::vector<string> strList = (it->second);
      for (std::vector<string>::iterator str= strList.begin(); str != strList.end(); str++)
	{
	  cout << var_name << " has size: " << (*str) << endl ;
	}
    }

}

void DataTransferSizes::forLoopIndexRanges(SgNode* node, MintInitNameExpMap_t& index_ranges)
{
    ROSE_ASSERT(node != NULL);
    SgOmpForStatement* target = isSgOmpForStatement(node);
    ROSE_ASSERT (target != NULL);
    SgScopeStatement* p_scope = target->get_scope();
    ROSE_ASSERT (p_scope != NULL);
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);

    // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
    SgForStatement * for_loops = isSgForStatement(body);

    //Step1: Find the indices, uppers, lowers and isIncrementals of the loops in a nested loop 
    std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(for_loops,V_SgForStatement);
  
    for (std::vector<SgForStatement* > ::iterator i = loopNest.begin(); i!= loopNest.end(); i++)
      {
	SgForStatement* cur_loop = *i;

	SgInitializedName * orig_index = NULL;
	SgExpression* orig_lower = NULL;
	SgExpression* orig_upper= NULL;
	SgExpression* orig_stride= NULL;
	
	// if the loop iteration space is incremental      
	bool isIncremental = true; 
	bool isInclusiveUpperBound = false ;
	// grab the original loop 's controlling information                                  
	// checks if loop index is not being written in the loop body
	bool is_canonical = isCanonicalForLoop (cur_loop, &orig_index, & orig_lower, 
						&orig_upper, &orig_stride, NULL, &isIncremental, &isInclusiveUpperBound);
	ROSE_ASSERT(is_canonical == true);
	
	//upper-lower + 1

	if(isIncremental){

	  SgExpression* raw_range_exp =buildSubtractOp(copyExpression(orig_upper), copyExpression(orig_lower));
	  index_ranges[orig_index] = isInclusiveUpperBound ? buildAddOp(raw_range_exp, buildIntVal(1)) : raw_range_exp ; 
	}
	else {
	  SgExpression* raw_range_exp =buildSubtractOp(copyExpression(orig_upper), copyExpression(orig_upper));
	  index_ranges[orig_index] = isInclusiveUpperBound ? buildAddOp(raw_range_exp, buildIntVal(1)) : raw_range_exp;
	    //isInclusiveUpperBound ? 
	    //buildAddOp(copyExpression(orig_lower), buildIntVal(1)):  orig_lower ; 
	}
	//cout <<"INFO-mint: Processing Loop " << " with index " << orig_index->get_name().str()  << endl;
      }
}



/*
void DataTransferSizes::forLoopIndexRanges(SgNode* node, MintInitNameExpMap_t& index_ranges)
{
    ROSE_ASSERT(node != NULL);
    SgOmpForStatement* target = isSgOmpForStatement(node);
    ROSE_ASSERT (target != NULL);
    SgScopeStatement* p_scope = target->get_scope();
    ROSE_ASSERT (p_scope != NULL);
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);

    // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
    SgForStatement * for_loops = isSgForStatement(body);

    //Step1: Find the indices, uppers, lowers and isIncrementals of the loops in a nested loop 
    std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(for_loops,V_SgForStatement);
  
    for (std::vector<SgForStatement* > ::iterator i = loopNest.begin(); i!= loopNest.end(); i++)
      {
	SgForStatement* cur_loop = *i;

	SgInitializedName * orig_index = NULL;
	SgExpression* orig_lower = NULL;
	SgExpression* orig_upper= NULL;
	SgExpression* orig_stride= NULL;
	
	// if the loop iteration space is incremental      
	bool isIncremental = true; 
	bool isInclusiveUpperBound = false ;
	// grab the original loop 's controlling information                                  
	// checks if loop index is not being written in the loop body
	bool is_canonical = isCanonicalForLoop (cur_loop, &orig_index, & orig_lower, 
						&orig_upper, &orig_stride, NULL, &isIncremental, &isInclusiveUpperBound);
	ROSE_ASSERT(is_canonical == true);
	
	if(isIncremental)
	  index_ranges[orig_index] = orig_upper;
	    //isInclusiveUpperBound ? 
	    //buildAddOp(copyExpression(orig_upper), buildIntVal(1)) : orig_upper ; 
	else 
	  index_ranges[orig_index] = orig_lower;
	    //isInclusiveUpperBound ? 
	    //buildAddOp(copyExpression(orig_lower), buildIntVal(1)):  orig_lower ; 

	//cout <<"INFO-mint: Processing Loop " << " with index " << orig_index->get_name().str()  << endl;
      }
}

*/

SgVarRefExp* DataTransferSizes::getArrayRef(SgNode* arrayNode)
{
  Rose_STL_Container<SgNode*> varList= NodeQuery::querySubTree(arrayNode, V_SgVarRefExp);
  
  for ( Rose_STL_Container<SgNode*>::iterator exp = varList.begin(); exp != varList.end(); exp++)
    {
      
      if(!isSgPntrArrRefExp(*exp)){
	
	//cout <<"Mint:findTransferSizes: expressions :" <<  (*exp)->unparseToString() << endl;
	
	SgVarRefExp* arrayRefExp = isSgVarRefExp(*exp); //this should be the array reference name 
	SgInitializedName *i_name = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayRefExp));
	SgType* var_type = i_name->get_type() ;   
	
	ROSE_ASSERT (i_name);
	//check if expression type is array, 
	if(!(isSgArrayType(var_type)) &&  !(isSgPointerType(var_type)))
	  {
	    cerr<< "getArrayRef: Something is WRONG !!!" << endl;
	  }
	
	return arrayRefExp;
      }
    }
  return NULL;
}


SgExpression* DataTransferSizes::getFirstIndexExpressionInArray(SgNode* arrayNode, string& index_exp_str)
{

  int dimNo = 0 ;

  SgExpression* index_exp;

  Rose_STL_Container<SgNode*> expressionList= NodeQuery::querySubTree(arrayNode, V_SgExpression);
  
  for ( Rose_STL_Container<SgNode*>::iterator exp = expressionList.begin(); exp != expressionList.end(); exp++)
    {
      
      if(!isSgPntrArrRefExp(*exp))
	{
	//skip the first one because it is the array reference expression

	if(dimNo == 1)
	  {
	    //this should be one of the index expression
	    index_exp = isSgExpression(*exp);

	    index_exp_str = (*exp)->unparseToString() ;

	    //cout <<"Mint:getFristIndexExpressionInArray:" <<  (index_exp)->unparseToString() << endl;

	    return index_exp;
	  }	      
	dimNo++;
      }
    }
  return NULL;
}

void DataTransferSizes::findTransferSizes(SgNode* node, 
					  MintSymSizesMap_t& trfSizes)
{
  //This function finds the iteration space of the forloops to determine 
  //dimensions of the arrays. We need this information to allocate space on the 
  //device memory and determine the transfer size. 

  //for(int j=0 ; j <= upper1 ; j++)
  // for(int i=upper2; i >=0 ; i--)
  //   E[i][j], E[j][i], E[n-1][j], E[0][j], E[j+1][i]

  //Step 1: In a nested for loop, keep loop index variable and its max value (either lower or upper )
  //Ex: returns (j,upper1) and (i,upper2)
  //Step 2: Find all the pointer array references in the loop body
  //Ex: returns E[i][j], E[i] ....
  //Step 3: For each pointer array references in the loop body, find all the expressions to find its name, and its first index variable expression
  //Ex: for E[n-1][j] it returns E[n-1][j],E[n-1] ,n-1, n, 1, j
  //Step 4: Replace all the index references with their upper values


    ROSE_ASSERT(node != NULL);
    SgOmpForStatement* target = isSgOmpForStatement(node);
    ROSE_ASSERT (target != NULL);
    SgScopeStatement* p_scope = target->get_scope();
    ROSE_ASSERT (p_scope != NULL);
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);

    // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
   // SgForStatement * for_loops = isSgForStatement(body);
    
    MintInitNameExpMap_t index_ranges;
 
    //Step 1: In a nested for loop, keep loop index variable and its max value (either lower or upper )
    forLoopIndexRanges(node, index_ranges);

    //Step 2: Find the pointer array reference expressions like E[i][j], E[i] 
    Rose_STL_Container<SgNode*> arrayList = NodeQuery::querySubTree(node, V_SgPntrArrRefExp);

    for ( Rose_STL_Container<SgNode*>::iterator arrayNode = arrayList.begin(); arrayNode != arrayList.end(); arrayNode++)
      {

	//Step 3: For each pointer array references in the loop body, 
	//find all the expressions to find its name, and its first index variable expression	

	//cout <<"Mint:findTransferSizes: Pointer Array Ref Exp:  " << (*arrayNode)->unparseToString() << endl;
	SgVarRefExp* arrayRefExp = NULL;
	SgExpression* indexExp = NULL;

	SgExpression* arrayName; 
	vector<SgExpression*>  subscripts; //first index is i if E[j][i]
	
	//index list are from right to left 
	bool yes = MintArrayInterface::isArrayReference(isSgExpression(*arrayNode), &arrayName, &subscripts);
	assert(yes == true);
	
	vector<SgExpression*>::reverse_iterator it= subscripts.rbegin();
	
	indexExp = (*it);

	//int arrayDim = subscripts.size() ;
	
	//arrayNode = arrayNode + arrayDim - 1;

	arrayRefExp = isSgVarRefExp(arrayName);
	
	ROSE_ASSERT(arrayRefExp != NULL);

	//cout << "Mint:findTransferSizes: Found Array " << arrayRefExp->unparseToString() <<  endl ;	
	string indexExpStr = indexExp -> unparseToString();

	//we are interested in the first dim size of the array

	ROSE_ASSERT(indexExp != NULL);

	string sizeExpStr = getSizeExpressionFromIndexExpression(indexExpStr, indexExp, index_ranges);

	addTransferSizeToThisSymbol(sizeExpStr, arrayRefExp, trfSizes);

      }

}


void DataTransferSizes::addTransferSizeToThisSymbol(string sizeExpStr, SgVarRefExp* arrayRefExp, 
						    MintSymSizesMap_t& trfSizes)
{
	//check if this symbol is in the list
	//if not, add the sizeExp 
	//if it is in the list, then scan all the other expression by converting them into string 
	//and making a string comparison

	const SgInitializedName * arrayName = SageInterface::convertRefToInitializedName(arrayRefExp);

	SgSymbol* tmp_sym = arrayName->get_symbol_from_symbol_table ();

	ROSE_ASSERT(tmp_sym);

	SgVariableSymbol* arraySymbol = isSgVariableSymbol(tmp_sym);
	ROSE_ASSERT(arraySymbol);
	
	MintSymSizesMap_t :: iterator var;
	
	var = trfSizes.find(arraySymbol);
	
	if(var != trfSizes.end())
	  {//found the symbol in the map 
	    
	    std::vector<string>& sizeStrings = var->second ;
	    
	    std::vector<string>::iterator str = find(sizeStrings.begin(), sizeStrings.end(), sizeExpStr);
	  
	    if(str == sizeStrings.end())
	      {
		//need to add this size
		(var->second).push_back(sizeExpStr);
	      }
	    //else: do nothing, we have already this size string in the size list
	  }
	else
	  {//not found, add this symbol and its size
	    std::vector<string> sizeStrings;
	    sizeStrings.push_back(sizeExpStr);

	    pair< MintSymSizesMap_t ::iterator, bool> ret;
	
	    ret = trfSizes.insert(make_pair( arraySymbol, sizeStrings));

	    /*if (ret.second) {
	      cout << "Insert succeeded!\n";
	    } 	    */
	  }

 }

string DataTransferSizes::getSizeExpressionFromIndexExpression(string indexExpStr,
							       SgExpression* indexExp,
							       MintInitNameExpMap_t& index_ranges)
{
  //cases :(1) E[i][j], (2) E[j+1][i], (3) E[0][j],(4) E[n+j][j],(5) E[n-1][j]
  //in the index expression node, query for SgVarRefExp 
  //if something is found,
  // compare it with the loop index variables
  // if there is a match, then replace the index var with its upper bound 
  // if there is no match, continue looking
  // no match found between all the var ref exp and loop indices, then this is an exp like this E[n-1]
  // then exp should become exp + 1
  //else, 
  //this is a constant like E[0], what do we do?
  //exp becomes exp + 1, we need at least this many rows
    
  string sizeExpStr = indexExpStr;
  
  //SgExpression* sizeExp = NULL ;
  Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(indexExp, V_SgVarRefExp);
  
  if(varList.size() == 0 ) //case 3, index is a constant, add 1 and assign to the size  
    { 
      sizeExpStr = "( " + sizeExpStr + " + 1 )" ;
      //do nothing
      //sizeExp = buildAddOp(indexExp, buildIntVal(1));
    }
  else{

    bool found = false;

    for(Rose_STL_Container<SgNode*>::iterator var = varList.begin(); var != varList.end(); var++)
      {
	
	SgVarRefExp* indexVar = isSgVarRefExp(*var);
	
	SgInitializedName *i_name = SageInterface::convertRefToInitializedName(isSgVarRefExp (indexVar));
	
	ROSE_ASSERT (i_name);
	
	string indexStr = i_name->get_name().str() ;
	
	MintInitNameExpMap_t :: iterator loop_ind;
	
	loop_ind = index_ranges.find(i_name);
	
	if(loop_ind != index_ranges.end())
	  {
	    //cout << "Mint:findTransferSizes: Loop index matched: "<<  i_name->get_name().str() << endl ;
	    
	    //replace occurances of index variable with its upper bound expression in the size  expression
	    SgExpression* upper = loop_ind -> second;
	    
	    string upperStr = upper-> unparseToString();
	    	    
	    //cout << "Before replace: sizeExpStr : " << sizeExpStr << endl ;
	    
	    size_t position = sizeExpStr.find(indexStr); // find first space

	    while ( position != string::npos ) 
	      {
		sizeExpStr.replace( position, indexStr.length(), upperStr );
		position = sizeExpStr.find(indexStr, position + upperStr.length() );
	      } 
	    
	    found = true;
	    //cout << "After replace: sizeExpStr : " << sizeExpStr << endl ;
	  }
      }
    if(!found)
        sizeExpStr = sizeExpStr + " + 1" ;

    sizeExpStr = "( " + sizeExpStr + " )";
  }
  
  //string sizeExpStr = sizeExp->unparseToString().c_str();

  
  return sizeExpStr; 
}
