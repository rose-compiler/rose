/*
  On Chip Memory Optimizer

  reduces global memory accesses with 

  the utilization of registers and shared memory


*/

#include "GhostCellHandler.h"
#include "../OptimizerInterface/CudaOptimizerInterface.h"
#include "ASTtools.hh"
#include "../../midend/mintTools/MintOptions.h"
#include "../../midend/arrayProcessing/MintArrayInterface.h"
#include "../programAnalysis/StencilAnalysis.h"

#include <string>
#include "OmpAttribute.h"

using namespace OmpSupport; //for replaceVariableRef
using namespace std;

GhostCellHandler::GhostCellHandler()
{


}

GhostCellHandler::~GhostCellHandler()
{


}

void GhostCellHandler::defineGhostCellVariables(SgFunctionDefinition* func_def, 
						const SgInitializedName* candidateVar, 
						const int order, const bool corner_yz)
{

  SgBasicBlock* kernel_body = func_def ->get_body();

  SgVariableSymbol* border_sym = MintArrayInterface::getSymbolFromName(kernel_body, BORDER_GLOBAL_INDEX_DIFF);
  
  if(border_sym != NULL)
    return; // This is the second candidate for shared memory, we don't need to declare new variables
  
  SgDeclarationStatement* src_location = NULL;

  string candidateVarName = candidateVar->get_name().str();

  SgVariableSymbol* gidz_sym = MintArrayInterface::getSymbolFromName(kernel_body, GIDZ);
  SgVariableSymbol* gidy_sym = MintArrayInterface::getSymbolFromName(kernel_body, GIDY);
  SgVariableSymbol* gidx_sym = MintArrayInterface::getSymbolFromName(kernel_body, GIDX);

  SgScopeStatement* scope = gidx_sym -> get_scope();

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

  string width_str = "width" + candidateVarName;
  string slice_str = "slice" + candidateVarName;

  //borderGlobalIndexDiff = (borderIdy - idy0) * Nplus32 + (borderIdx-idx) ;
  if(MintOptions::GetInstance()->useSameIndex()){
    width_str = "_width";
    slice_str = "_slice";
  }
  //  int borderIdx=idx, borderIdy =0, borderGlobalIndexDiff=0;
  SgVariableDeclaration* borderIdx = buildVariableDeclaration("_borderIdx", buildIntType(),
							      buildAssignInitializer(buildVarRefExp(IDX, scope)),scope);

  SgVariableDeclaration* borderIdy = buildVariableDeclaration("_borderIdy", buildIntType(),
							      buildAssignInitializer(buildIntVal(0)),scope);
  SgVariableDeclaration* borderGlobalIndexDiff = buildVariableDeclaration(BORDER_GLOBAL_INDEX_DIFF, buildIntType(),
									  buildAssignInitializer(buildIntVal(0)),scope);

  SgVariableDeclaration* borderGlobalIndexDiff_up = 
    buildVariableDeclaration(BORDER_GLOBAL_INDEX_DIFF+"_up", buildIntType(),
			     buildAssignInitializer( buildSubtractOp(buildVarRefExp(borderGlobalIndexDiff), 
								     buildVarRefExp(slice_str, scope))), scope);

  SgVariableDeclaration* borderGlobalIndexDiff_down = 
    buildVariableDeclaration(BORDER_GLOBAL_INDEX_DIFF+ "_down", buildIntType(),
			     buildAssignInitializer( buildAddOp(buildVarRefExp(borderGlobalIndexDiff), 
								buildVarRefExp(slice_str, scope))), scope);

  SgVarRefExp* borderIdxVarRef = buildVarRefExp(borderIdx);
  SgVarRefExp* borderIdyVarRef = buildVarRefExp(borderIdy);

  //  borderIdx = (threadIdx.y == 1 ) ? 0  : borderIdx;
  SgExpression* rhs1 = buildConditionalExp(buildEqualityOp(buildVarRefExp(THREADIDXY, scope), buildIntVal(1)),
					  buildIntVal(0), borderIdxVarRef);

  SgStatement* initBorderId1 = buildAssignStatement(borderIdxVarRef, rhs1);

  //borderIdx = (threadIdx.y == 2 ) ? 16+1  : borderIdx;
  SgExpression* rhs2 = buildConditionalExp(buildEqualityOp(buildVarRefExp(THREADIDXY, scope), buildIntVal(2)),
					  buildAddOp(buildVarRefExp(BLOCKDIM_X), buildIntVal(1)), borderIdxVarRef);

  SgStatement* initBorderId2 = buildAssignStatement(borderIdxVarRef, rhs2);

  if(corner_yz)
  {
    insertStatementAfter( src_location ,borderGlobalIndexDiff_down);
    insertStatementAfter( src_location ,borderGlobalIndexDiff_up);
  }

  insertStatementAfter( src_location ,initBorderId2);
  insertStatementAfter( src_location ,initBorderId1);
  insertStatementAfter( src_location ,borderGlobalIndexDiff);
  insertStatementAfter( src_location ,borderIdy);
  insertStatementAfter( src_location ,borderIdx);

  //borderIdy = (threadIdx.y == 3) ? 16+1 : borderIdy ;
  rhs1 = buildConditionalExp(buildEqualityOp(buildVarRefExp(THREADIDXY, scope), buildIntVal(3)),
			     buildAddOp(buildVarRefExp(BLOCKDIM_Y), buildIntVal(1)), borderIdyVarRef);

  initBorderId1 = buildAssignStatement(borderIdyVarRef, rhs1);

  insertStatementAfter( initBorderId2,initBorderId1);
  //borderIdy = (threadIdx.y == 1 || threadIdx.y == 2) ? idx : borderIdy ;
  rhs2 = buildConditionalExp(buildOrOp(buildEqualityOp(buildVarRefExp(THREADIDXY, scope), buildIntVal(1)),
				       buildEqualityOp(buildVarRefExp(THREADIDXY, scope), buildIntVal(2))),
			     buildVarRefExp("_idx"), borderIdyVarRef);

  initBorderId2 = buildAssignStatement(borderIdyVarRef, rhs2);


  insertStatementAfter( initBorderId1,initBorderId2);



  SgStatement* diff = 
    buildAssignStatement(buildVarRefExp(borderGlobalIndexDiff), 
			 buildAddOp(buildSubtractOp(borderIdxVarRef, buildVarRefExp("_idx", scope)),
						    buildMultiplyOp(buildVarRefExp(width_str, scope),
								    buildSubtractOp(borderIdyVarRef, buildVarRefExp("_idy",scope)))));
  insertStatementAfter(initBorderId2 ,diff);

}

void loadCornerGhostCells(SgFunctionDefinition* kernel, 
			  SgInitializedName* candidateVar, 
			  SgInitializedName* sh_block, 
			  SgScopeStatement* varScope, bool unrollWithoutRegister)
{
  /*
  if(threadIdx.x == 0 && threadIdx.y == 0)
  {
    Uold = Uold + indexUold -1 - widthUold ; 
    _sh_block[_ctr][0][0]= Uold[0];
    _sh_block[_ctr][0][BLOCKDIM_X + 1]= Uold[BLOCKDIM_X + 1];
    _sh_block[_ctr][BLOCKDIM_Y + 1][0]= Uold[widthUold * (BLOCKDIM_Y + 1)];
    _sh_block[_ctr][BLOCKDIM_Y + 1][BLOCKDIM_X + 1]= Uold[widthUold * (BLOCKDIM_Y + 1) + BLOCKDIM_X + 1];
    Uold = Uold - indexUold + 1 + widthUold ; 
  }*/
  string ctr = "_ctr";
  if(!unrollWithoutRegister)
    ctr = IDZ; 

  SgVariableSymbol* sym = CudaOptimizerInterface::getSymbolFromName(kernel->get_body(),"_idx");
  ROSE_ASSERT(sym);

  SgScopeStatement* scope = sym->get_scope();

  SgBasicBlock* if_body = buildBasicBlock();

  SgVarRefExp* candidateVarExp = buildVarRefExp(candidateVar, scope);
  string candidateVar_str = candidateVar->get_name().str(); 

  string width_str = "width" + candidateVar_str;
  string index_str = "index" + candidateVar_str;

  //borderGlobalIndexDiff = (borderIdy - idy0) * Nplus32 + (borderIdx-idx) ;
  if(MintOptions::GetInstance()->useSameIndex()){
    width_str = "_width";
    index_str = "_index3D";
  }
  SgExpression* rhs =  buildSubtractOp(buildAddOp(candidateVarExp, 
						  buildVarRefExp(index_str, scope)), 
				       buildAddOp(buildIntVal(1), 
						  buildVarRefExp(width_str, scope))); 

  SgExprStatement* offset = buildAssignStatement(candidateVarExp, rhs);
			
  SgExprStatement* offset2 = buildAssignStatement(candidateVarExp, 
						    buildAddOp(buildSubtractOp(candidateVarExp, 
									       buildVarRefExp(index_str, scope)), 
							       buildAddOp(buildIntVal(1), 
									  buildVarRefExp(width_str, scope)))); 
  //_sh_block[_ctr][0][0]= Uold[0];
  SgExpression* sharedPntrRef = 
    CudaOptimizerInterface::createSharedMemoryReference(kernel, 
							sh_block, buildIntVal(0), buildIntVal(0), 
							buildVarRefExp(ctr, scope));

  SgExprStatement* corner = buildAssignStatement(sharedPntrRef, 
						    buildPntrArrRefExp( buildVarRefExp(candidateVar, scope), 
									buildIntVal(0)));
  
  appendStatement( offset, if_body);

  appendStatement( corner, if_body);

  // _sh_block[_ctr][0][BLOCKDIM_X + 1]= Uold[BLOCKDIM_X + 1];
  sharedPntrRef = 
    CudaOptimizerInterface::createSharedMemoryReference(kernel, 
							sh_block, buildAddOp(buildVarRefExp("blockDim.x", scope), buildIntVal(1)), 
							buildIntVal(0), buildVarRefExp(ctr, scope));
  
  corner = buildAssignStatement(sharedPntrRef, buildPntrArrRefExp( buildVarRefExp(candidateVar, scope), 
								    buildAddOp(buildVarRefExp("blockDim.x", scope), buildIntVal(1))));
  ROSE_ASSERT(corner);
  appendStatement( corner, if_body);

  // _sh_block[_ctr][BLOCKDIM_Y + 1][0]= Uold[widthUold * (BLOCKDIM_Y + 1)];
  sharedPntrRef = 
    CudaOptimizerInterface::createSharedMemoryReference(kernel, sh_block, buildIntVal(0), 
						  buildAddOp(buildVarRefExp("blockDim.y", scope), buildIntVal(1)), 
						  buildVarRefExp(ctr, scope));

  
  corner = buildAssignStatement(sharedPntrRef, 
				 buildPntrArrRefExp( buildVarRefExp(candidateVar, scope), 
						     buildMultiplyOp(buildVarRefExp(width_str, scope),
								     buildAddOp(buildVarRefExp("blockDim.y", scope), buildIntVal(1)))));
  ROSE_ASSERT(corner);
  appendStatement( corner, if_body);

  //_sh_block[_ctr][BLOCKDIM_Y + 1][BLOCKDIM_X + 1]= Uold[widthUold * (BLOCKDIM_Y + 1) + BLOCKDIM_X + 1];

  sharedPntrRef = 
    CudaOptimizerInterface::createSharedMemoryReference(kernel, 
							sh_block, buildAddOp(buildVarRefExp("blockDim.x", scope), buildIntVal(1)), 
							buildAddOp(buildVarRefExp("blockDim.y", scope), buildIntVal(1)), 
							buildVarRefExp(ctr, scope));
  
  corner = buildAssignStatement(sharedPntrRef, 
				buildPntrArrRefExp( buildVarRefExp(candidateVar, scope), 
						    buildAddOp(buildMultiplyOp(buildVarRefExp(width_str, scope),
									       buildAddOp(buildVarRefExp("blockDim.y", scope), 
											  buildIntVal(1))), 
							       buildAddOp(buildVarRefExp("blockDim.x", scope), buildIntVal(1)))));
				 
  ROSE_ASSERT(corner);
  appendStatement( corner, if_body);
  appendStatement( offset2, if_body);

  //if(threadIdx.x == 0 && threadIdx.y == 0)
  SgExpression* cond = buildAndOp(buildEqualityOp(buildVarRefExp("threadIdx.x", scope), buildIntVal(0)), 
				  buildEqualityOp(buildVarRefExp("threadIdx.y", scope), buildIntVal(0)));

  ROSE_ASSERT(cond);
  ROSE_ASSERT(if_body);

  SgIfStmt* if_stmt = buildIfStmt(cond,  if_body, NULL);

  ROSE_ASSERT(if_stmt);
  prependStatement(if_stmt, varScope);

}

void GhostCellHandler::loadGhostCellsIntoSharedMemory(SgFunctionDefinition* kernel, 
						     SgInitializedName* candidateVar,
						     SgInitializedName* sharedRef,
						     bool corner_yz, int order, 
						     bool unrollWithoutRegister,
						     SgStatement* sourcePosOutside, 
						      SgScopeStatement* varScope) 
			
{

  /* depending on the order, we need to load the ghost cell into shared memory 
     with an if statement. We already tested if type of stencil is suitable for 
     this ghost cell optimization. Otherwise, use the other way of loading ghosts. 

     if(threadIdx.y < order*4 )
       block[ctr][borderIdy][borderIdx] = Uold[index + borderGlobalIndexDiff ];
     
       >               if(threadIdx.x == 0 && threadIdx.y == 0)
       >                 {
       >                   Uold = Uold + indexUold -1 - widthUold ; 
       >                   _sh_block[_ctr][0][0]= Uold[0];
       >                   _sh_block[_ctr][0][BLOCKDIM_X + 1]= Uold[BLOCKDIM_X + 1];
       >                   _sh_block[_ctr][BLOCKDIM_Y + 1][0]= Uold[widthUold * (BLOCKDIM_Y + 1)];
       >                   _sh_block[_ctr][BLOCKDIM_Y + 1][BLOCKDIM_X + 1]= Uold[widthUold * (BLOCKDIM_Y + 1) + BLOCKDIM_X + 1];
       >                   Uold = Uold - indexUold + 1 + widthUold ; 
       >                 }
  */

  ROSE_ASSERT(sourcePosOutside);
  ROSE_ASSERT(varScope);
  SgStatement* varScopeStmt = isSgStatement(varScope);
  ROSE_ASSERT(varScopeStmt);

  //SgBasicBlock* kernel_body = kernel->get_body();

  int dim = MintArrayInterface::getDimension(candidateVar);

  string candidate_name = candidateVar->get_name().str();

  /*
  if(insertSync)
    {
      //insert a sync point 
      SgStatement *syncthreads= buildFunctionCallStmt("__syncthreads", buildVoidType(), NULL, varScope);  
      ROSE_ASSERT(syncthreads);
      prependStatement(syncthreads, varScope);      
    }
  */

  //create the shared memory expression sh[][borderIdy][borderIdx]
  SgExpression* first=NULL, *second=NULL, *third=NULL;

  first = buildVarRefExp("_borderIdx", varScope);
  ROSE_ASSERT(first);
 
  if(dim >= 2){
    second = buildVarRefExp("_borderIdy", varScope);
    ROSE_ASSERT(second);
    
    if(dim >= 3 && corner_yz && !unrollWithoutRegister){
      third  = buildVarRefExp("_idz", varScope);
      ROSE_ASSERT(third);
    }
    else if (dim >= 3 && unrollWithoutRegister)
    {
      third =  buildVarRefExp("_ctr", varScope);
      ROSE_ASSERT(third);
    }
  }

  SgExpression* sharedPntrRef = 
    CudaOptimizerInterface::createSharedMemoryReference(kernel, 
							sharedRef, first, second, third); 

  SgVarRefExp* offset =buildVarRefExp(BORDER_GLOBAL_INDEX_DIFF, varScope);
  ROSE_ASSERT(offset);

  SgExpression* arrRefWithIndices = isSgExpression(CudaOptimizerInterface::
						   createGlobalMemoryRefWithOffset(varScope, candidateVar, offset));
				
  //if we are loading up and center, they should be outside of the unroll loop
  if(third != NULL && corner_yz)
  {

    loadThisGhostCell(candidateVar, arrRefWithIndices, sharedPntrRef, kernel , sourcePosOutside, order);

    SgExpression* third_up = NULL, *third_down = NULL ; 

    if(!unrollWithoutRegister){
	third_up =  buildVarRefExp("_idz - 1", varScope);	
	third_down =  buildVarRefExp("_idz + 1", varScope);	
    }
    else {
	third_up =  buildVarRefExp("_up", varScope);	
	third_down =  buildVarRefExp("_down", varScope);	
    }

    //up 
    //if (threadIdx.y < 4 * 1)                                                                                                          
    //  _sh_block[_up][_borderIdy][_borderIdx] = Uold[indexUold + _borderGlobalIndexDiff - sliceUold];                         
          
    SgExpression* sharedPntrRef2 = 
      CudaOptimizerInterface::createSharedMemoryReference(kernel, 
							  sharedRef, first, second, third_up); 

    SgVarRefExp* offset2 =buildVarRefExp(BORDER_GLOBAL_INDEX_DIFF +"_up", varScope);
    
    SgExpression* arrRefWithIndices2 = isSgExpression(CudaOptimizerInterface::
						     createGlobalMemoryRefWithOffset(varScope, candidateVar, offset2));
    
    //outside of the loop 
    loadThisGhostCell(candidateVar, arrRefWithIndices2, sharedPntrRef2, kernel, sourcePosOutside, order);

    sharedPntrRef2 = 
      CudaOptimizerInterface::createSharedMemoryReference(kernel, 
							  sharedRef, first, second, third_down); 

    offset2 =buildVarRefExp(BORDER_GLOBAL_INDEX_DIFF +"_down", varScope);

    arrRefWithIndices2 = isSgExpression(CudaOptimizerInterface::
					createGlobalMemoryRefWithOffset(varScope, candidateVar, offset2));
    
    loadThisGhostCell(candidateVar, arrRefWithIndices2, sharedPntrRef2, kernel, varScopeStmt, order);
      
    //and 4 corners 
    loadCornerGhostCells(kernel, candidateVar, sharedRef, varScope, unrollWithoutRegister); 

  }//otherwise, they should be inside of the loop
  else 
    loadThisGhostCell(candidateVar, arrRefWithIndices, sharedPntrRef,kernel, varScopeStmt, order);


}

void GhostCellHandler::loadThisGhostCell(const SgInitializedName* candidateVar, 
					SgExpression* rhs, SgExpression* lhs, 
					SgFunctionDefinition* kernel, 
					SgStatement* sourcePos, int order)
{
  
  SgBasicBlock* kernel_body = kernel->get_body();

  SgStatement* true_body = buildAssignStatement(lhs, rhs);
  
  //if(threadIdx.y < order*4 )
  SgExpression* if_cond = buildLessThanOp(buildVarRefExp("threadIdx.y", kernel_body) , 
					 buildMultiplyOp(buildIntVal(4), buildIntVal(order)));

  ROSE_ASSERT(if_cond);

  if(if_cond != NULL) //it might a center point, so it is not a ghost cell
    {
      SgExprStatement* cond_stmt = SageBuilder::buildExprStatement (if_cond);
  
      ROSE_ASSERT(cond_stmt);
      ROSE_ASSERT(true_body);

      SgIfStmt* ifIdz_y_x = buildIfStmt(cond_stmt, true_body, NULL );
      
      ROSE_ASSERT(ifIdz_y_x);
  
      if(isSgScopeStatement(sourcePos))
	prependStatement(ifIdz_y_x, isSgScopeStatement(sourcePos));
      else {
	insertStatementAfter(sourcePos, ifIdz_y_x);  //insert s2 after s1
      }
    }
  
}

int getGhostCellCategory(std::vector<SgExpression*> subscripts)
{
  //categorizes the ghost cells access patterns so that we can merge 
  //their loads into few if-statement

  //case 1: [k][j][i-c]
  //case 2: [k][j][i+c]
  //case 3: [k][j-1][i]
  //case 4: [k][j+1][i]
  //case 5: [k-1][j][i]
  //case 6: [k+1][j][i]
  //std::vector<SgExpression*> singles[6]; 

  //case 1: [k][j-c][i-c] 
  //case 2: [k][j-c][i+c]
  //case 3: [k][j+c][i-c]
  //case 4: [k][j+c][i+c]
  //case 5: [k-c][j][i-c] 
  //case 6: [k-c][j][i+c] 
  //case 7: [k+c][j][i-c] 
  //case 8: [k+c][j][i+c] 
  //case 9: [k-c][j][i+c] ...
  //std::vector<SgExpression*> doubles[12]; 

  //case 1: [k-c][j-c][i-c] 000
  //case 2: [k-c][j-c][i+c] 001
  //case 3: [k-c][j+c][i-c] 010
  //case 4: [k-c][j+c][i+c] 011
  //case 5: [k+c][j-c][i-c] 100 ...
  //std::vector<SgExpression*> triples[8]; 

  int indexNo = 0;
  int constCount = 0;
  bool signs[3]; //false -, true +
  bool whichXYZ[3]; //which dimension x y z ?

  vector<SgExpression*>::const_iterator it ; 
  
  for(it = subscripts.begin() ; it != subscripts.end() ; it++)
    {
      
      SgExpression* index = isSgExpression(*it);
      
      Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(index, V_SgIntVal);
      Rose_STL_Container<SgNode*> addList = NodeQuery::querySubTree(index, V_SgAddOp);
      
      signs[indexNo] = false;
      whichXYZ[indexNo] = false;
      
      if(varList.size() == 1)
	{
	  constCount++;
	  if(addList.size() == 1)
	    signs[indexNo] = true;
	  
	  whichXYZ[indexNo] = true;
	}
      
      indexNo++;
    }

  if (constCount == 1 )//singles
    {
      if(whichXYZ[0]) //x +-
	{
	  return signs[0] ? 0 : 1;
	}
      else if(whichXYZ[1]) //y +- 
	{
	  return signs[1] ? 2 : 3;
	}
      else if(whichXYZ[2]) //z +- 
	{
	  //we already load up and down planes
	  //TODO: Verify this 
	  return -1; 
	  //return signs[2] ? 4 : 5;
	}
    }
  else if(constCount == 2) //doubles
    {
      if(whichXYZ[0] && whichXYZ[1]) //x and y
	{
	  if(!signs[0] && !signs[1]) //--
	    return 6;
	  else if(signs[0] && !signs[1]) //x +, y -
	    return 7;
	  else if(!signs[0] && signs[1])
	    return 8;
	  else if(signs[0] && signs[1])
	    return 9;
	}
      if(whichXYZ[0] && whichXYZ[2]) //x and z
	{
	  if(!signs[0] && !signs[2]) //--
	    return 10;
	  else if(signs[0] && !signs[2]) //x +, z -
	    return 11;
	  else if(!signs[0] && signs[2])
	    return 12;
	  else if(signs[0] && signs[2])
	    return 13;
	}
      if(whichXYZ[1] && whichXYZ[2]) //y and z
	{
	  if(!signs[1] && !signs[2]) //--
	    return 14;
	  else if(signs[1] && !signs[2]) //y +, z -
	    return 15;
	  else if(!signs[1] && signs[2])
	    return 16;
	  else if(signs[1] && signs[2])
	    return 17;
	}
    }
  else if (constCount == 3) //triples
    {
      if(!signs[0] && !signs[1] && !signs[2]) //all minus
	return 18;      
      else if( signs[0] && !signs[1] && !signs[2]) //001 
	return 19;      
      else if(!signs[0] && signs[1] && !signs[2]) //010
	return 20;      
      else if( signs[0] && signs[1] && !signs[2]) //011
	return 21;
      else if(!signs[0] && !signs[1] && signs[2]) //100
	return 22;      
      else if( signs[0] && !signs[1] && signs[2]) //101
	return 23;      
      else if( !signs[0] && signs[1] && signs[2]) //110
	return 24;      
      else if( signs[0] && signs[1] && signs[2]) //111
	return 25;      
    }
  return -1;
}

void GhostCellHandler::loadGhostCellsIntoSharedMemory(SgFunctionDefinition* kernel, 
						      const SgInitializedName* candidateVar,
						      SgInitializedName* sharedRef,
						      const set<SgExpression*>& ghostCells,
						      bool corner_yz, int order, 
						      bool unrollWithoutRegister,
						      SgStatement* sourcePosOutside, 
						      SgScopeStatement* varScope) 
			
{
  ROSE_ASSERT(sourcePosOutside);
  ROSE_ASSERT(varScope);
  SgStatement* varScopeStmt = isSgStatement(varScope);
  ROSE_ASSERT(varScopeStmt);

  SgBasicBlock* kernel_body = kernel->get_body();

  size_t dim = MintArrayInterface::getDimension(candidateVar);

  /*
  if(insertSync && ghostCells.size() > 0 )
    {
      SgStatement *syncthreads= buildFunctionCallStmt("__syncthreads", buildVoidType(), NULL, varScope);
      
      ROSE_ASSERT(syncthreads);
      prependStatement(syncthreads, varScope);      
    }
  */

  //we group ghost cell loads into couple if-conditional statements
  //depending on which thread should load it
  //ghost cell loads will go to the body of the if-cond
  std::vector<SgIfStmt*> ifCondList[2];
  ifCondList[0].reserve(26); //for loads outside of the loop
  ifCondList[1].reserve(26); //for loads inside of the loop
  
  //there 26 different patterns in 3D, 8 different in 2D, see category function
  for(int tmp = 0; tmp < 26 ; tmp++){
    ifCondList[0].push_back(NULL);
    ifCondList[1].push_back(NULL);
  }

  //Ghost cell loads into shared memory
  std::set<SgExpression*>::const_iterator ref;

  for(ref = ghostCells.begin(); ref != ghostCells.end(); ref++)
    {

      SgExpression* arrayExp;

      vector<SgExpression*>  subscripts; //first index is k if E[j][i][k]

      SgExpression* arrRefWithIndices = isSgExpression(*ref);

      ROSE_ASSERT(arrRefWithIndices);

      if(MintArrayInterface::isArrayReference(arrRefWithIndices, &arrayExp, &subscripts))
	{
	  //SgInitializedName *arrayName = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayExp));

	  //Didem: March 03, 2011 
	  //we don't need to check the array name because ghost cells ref list only contains 
	  //references of the candidate array
	  //we already checked the shareable references before calling this function
	  //string var_name= arrayName->get_name().str();	
	  //ROSE_ASSERT(var_name == candidate_name);
	  //ROSE_ASSERT(subscripts.size()== dim);

	  //check if array name matches, and the indices are in the shared memory range
	  if(subscripts.size()  == dim) // && var_name == candidate_name && isShareableReference(subscripts, corner_yz))
	    {
	      //get the index references for:  E[gidy + 1 ][gidx] first: gidx, second: gidy + 1
	      ROSE_ASSERT(subscripts.size()>= 1);	      
	      vector<SgExpression*>:: iterator it = subscripts.begin();
	      
	      SgExpression* first=NULL, *second=NULL, *third=NULL;
	      
	      first = copyExpression(*it);

	      if(subscripts.size() >= 2){
		second = copyExpression(*(++it));
		ROSE_ASSERT(second);
		second = CudaOptimizerInterface::convertGlobalMemoryIndexIntoLocalIndex(second, IDY, kernel_body);
		ROSE_ASSERT(second);

		if(subscripts.size() >= 3 && corner_yz && !unrollWithoutRegister){
		  third = copyExpression(*(++it));
		  ROSE_ASSERT(third);
		  third = CudaOptimizerInterface::convertGlobalMemoryIndexIntoLocalIndex(third, IDZ, kernel_body);
		  ROSE_ASSERT(third);
		}
		else if (subscripts.size() >= 3 && unrollWithoutRegister)
		  {
		    third = copyExpression(*(++it));
		    ROSE_ASSERT(third);
		    third = CudaOptimizerInterface::convertGlobalMemoryIndexIntoLocalIndex(third, "_ctr", kernel_body);
		    ROSE_ASSERT(third);
		  }
	      }

	      first  = CudaOptimizerInterface::convertGlobalMemoryIndexIntoLocalIndex(first, IDX, kernel_body);
	    	      
	      ROSE_ASSERT(first);

	      SgExpression* sharedPntrRef = 
		CudaOptimizerInterface::createSharedMemoryReference(kernel, 
								    sharedRef, first, second, third); 

	      ROSE_ASSERT(sharedPntrRef);

	      //if we are loading up and center, they should be outside the unroll loop
	      if(third != NULL && corner_yz)
		{
		  string thirdIndex = third->unparseToString();
		  if(thirdIndex == "_up" || thirdIndex == "_ctr" ) 
		    {
		      loadThisGhostCell(candidateVar, copyExpression(arrRefWithIndices), sharedPntrRef, 
					subscripts, kernel, sourcePosOutside, order, ifCondList[0]);
		    }
		  else //when it is down, then it is inside of the loop
		    loadThisGhostCell(candidateVar, copyExpression(arrRefWithIndices), sharedPntrRef, 
				      subscripts, kernel, varScopeStmt, order, ifCondList[1]);
		}//otherwise, they should be inside of the loop
	      else 
		loadThisGhostCell(candidateVar, copyExpression(arrRefWithIndices), sharedPntrRef, 
				  subscripts, kernel, varScopeStmt, order, ifCondList[1]);
	    }
	}
    }

  ifCondList[0].clear();
  ifCondList[1].clear();
}


void GhostCellHandler::loadThisGhostCell(const SgInitializedName* candidateVar, 
					SgExpression* rhs, SgExpression* lhs, 
					const std::vector<SgExpression*>& subscripts,
					SgFunctionDefinition* kernel, 
					SgStatement* sourcePos, 
					const int order, std::vector<SgIfStmt*>& ifCondList)
{
  /*
    Handles ghost cells loads into shared memory    
    if (_idx == BLOCKDIM_X + 2 && _idy == BLOCKDIM_Y + 2) {
    _sh_block_data[_idy + 1][_idx + 2] = data[indexdata + 2 + widthdata];
    }
    if (_idx == BLOCKDIM_X + 2 && _idy == BLOCKDIM_Y + 2) {
    _sh_block_data[_idy + 3][_idx + 2] = data[indexdata + 2 + widthdata * 3];
    }
    if (_idx == BLOCKDIM_X + 2 && _idy == BLOCKDIM_Y + 2) {
    _sh_block_data[_idy + 2][_idx + 1] = data[indexdata + 1 + widthdata * 2];
    }
  */
  //added in March 04, 2011
  //we have conditionals of if statements that are the same, 
  //so we should merge if-bodies into one, and have only one condition  
  //and many statements in the if-body. 
  //set<string> ifConditionList;

  SgBasicBlock* kernel_body = kernel->get_body();
  SgStatement* if_body = buildAssignStatement(lhs, rhs);
  
  SgBasicBlock* true_body = SageBuilder::buildBasicBlock ();

  appendStatement(if_body, true_body);
 
  SgExpression* if_cond = NULL;

  int categoryNo = getGhostCellCategory(subscripts);

  if(categoryNo >=0)
    {
      if(ifCondList[categoryNo] == NULL)
	{
	  if_cond = buildGhostCellLoadConditionExp(kernel_body, subscripts, order);
	  
	  if(if_cond != NULL) //it might a center point, so it is not a ghost cell
	    {
	      SgExprStatement* cond_stmt = SageBuilder::buildExprStatement (if_cond);
	      ROSE_ASSERT(cond_stmt);
	      
	      
	      ROSE_ASSERT(true_body);
	      SgIfStmt* ifIdz_y_x = buildIfStmt(cond_stmt, true_body, NULL );
	      
	      ROSE_ASSERT(ifIdz_y_x);
	      
	      if(isSgScopeStatement(sourcePos))
		prependStatement(ifIdz_y_x, isSgScopeStatement(sourcePos));
	      else {
		insertStatementAfter(sourcePos, ifIdz_y_x);  //insert s2 after s1
	      }
	      
	      //for the first time we have this if-cond, add to the list
	      ifCondList[categoryNo] = ifIdz_y_x ;
	    }
	}
      else{ //we already have this condition, just prepend the if-body 

	SgIfStmt* if_stmt= ifCondList.at(categoryNo);
	ROSE_ASSERT(if_stmt);

	SgStatement* orig_body = if_stmt->get_true_body();
	ROSE_ASSERT(orig_body);

	ROSE_ASSERT(isSgBasicBlock(orig_body));

	appendStatement(if_body,isSgBasicBlock( orig_body));
      }
    }//it might a center point, so it is not a ghost cell
}



SgExpression* GhostCellHandler::buildGhostCellLoadConditionExp(SgScopeStatement* kernel_body,
							       const std::vector<SgExpression*> subscripts,
							       int order)
{

  //can we categorize them in advance, north,south, east and west 
  //NORTH sh_block[0][idx] = Epev[gidy-1][gidx]
  //SOUTH sh_block[BLOCKDIM_Y+1][idx] = Epev[gidy+1][gidx]  
  //EAST sh_block[idy][0] = Epev[gidy][gidx-1]  
  //WEST sh_block[idy][BLOCKDIM_X + 1] = Eprev[gidy][gidx+1]
  
  //we want either one add or subtract operation in the index expression
  //no complex indexing is supported for now. 

  SgExpression* if_cond= NULL;

  SgExpression* idz_y_x, *blockDimz_y_x, *globalIndex_y_x;
  int indexNo = 0 ; 
  std::vector<SgExpression*>::const_iterator it;

  for(it= subscripts.begin(); it != subscripts.end(); it++)
    {
      indexNo++;
      ROSE_ASSERT(*it);
      
      SgExpression* exp= isSgExpression(*it);
      
      Rose_STL_Container<SgNode*> constList = NodeQuery::querySubTree(exp, V_SgIntVal);
      
      if( constList.size()==1)
	{
	  if(indexNo == 1)
	    {
	      idz_y_x = buildVarRefExp(IDX, kernel_body);      
	      blockDimz_y_x = buildVarRefExp(TILE_X, kernel_body);
	      globalIndex_y_x = buildEqualityOp(buildVarRefExp(GIDX, kernel_body), buildVarRefExp(UPPERBOUND_X, kernel_body)); 
	    }
	  else if(indexNo == 2) 
	    {
	      idz_y_x = buildVarRefExp(IDY, kernel_body);      
	      blockDimz_y_x = buildVarRefExp(TILE_Y, kernel_body);
	      globalIndex_y_x = buildEqualityOp(buildVarRefExp(GIDY, kernel_body), buildVarRefExp(UPPERBOUND_Y, kernel_body)); 
	    }
	  else if(indexNo == 3)
	    {
	      idz_y_x = buildVarRefExp(IDZ, kernel_body);
	      blockDimz_y_x = buildIntVal(1);	  
	    }

	  Rose_STL_Container<SgNode*> opList1 = NodeQuery::querySubTree(exp, V_SgAddOp);
	  Rose_STL_Container<SgNode*> opList2 = NodeQuery::querySubTree(exp, V_SgSubtractOp);
	  
	  ROSE_ASSERT(opList1.size()== 1 || opList2.size() == 1);
	  
	  SgExpression* if_cond2 = NULL;
	  
	  if(opList1.size() == 1)
	    {
	      //WEST sh_block[idy][idx + 1] = Eprev[gidy][gidx+1]  
	      //SOUTH sh_block[idy+1][idx] = Epev[gidy+1][gidx]  	 if(_idy == blockDim.y) 
	      if(order-1 != 0 && indexNo !=3 )
		{
		  SgExpression* cond = buildEqualityOp(idz_y_x, buildAddOp(blockDimz_y_x, buildIntVal(order-1)));
		  if_cond2 = buildOrOp(cond, globalIndex_y_x );
		}
	      else if (indexNo != 3)
		{
		  SgExpression* cond = buildEqualityOp(idz_y_x, blockDimz_y_x);
		  if_cond2 = buildOrOp(cond, globalIndex_y_x);
		}
	      else
		{
		  if_cond2 = buildEqualityOp(idz_y_x, blockDimz_y_x);
		}
	    }
	  else if (opList2.size() == 1)
	    {
	      //EAST sh_block[idy][idx-1] = Epev[gidy][gidx-1]  if(_idx == 1)
	      //NORTH sh_block[idy-1][idx] = Epev[gidy-1][gidx]   if (_idy == 1) 
	      if(indexNo != 3)
		if_cond2 = buildEqualityOp(idz_y_x, buildIntVal(order));
	      else 
		if_cond2 = buildEqualityOp(idz_y_x, buildIntVal(1));
	    }		 
	  
	  ROSE_ASSERT(if_cond2);
	  
	  if(if_cond == NULL)
	    if_cond = if_cond2;
	  else 
	    if_cond = buildAndOp(if_cond, if_cond2);
	  
	  ROSE_ASSERT(if_cond);
	}
      else{
	//this is the case when we access the center E[i][j], not to the neighbors
	ROSE_ASSERT(constList.size()==0);
      }
    }

  return if_cond;
}
