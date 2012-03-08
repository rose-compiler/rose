/*
  On Chip Memory Optimizer
  reduces global memory accesses with 
  the utilization of registers and shared memory

*/

#include "OnChipMemoryOpt.h"
#include "../OptimizerInterface/CudaOptimizerInterface.h"
#include "ASTtools.hh"
#include "../../midend/mintTools/MintOptions.h"
#include "../../midend/arrayProcessing/MintArrayInterface.h"
#include "../programAnalysis/StencilAnalysis.h"

#include "./GhostCellHandler.h"

#include <sstream>
#include <string>
#include "OmpAttribute.h"

using namespace OmpSupport; //for replaceVariableRef
using namespace std;

OnChipMemoryOpt::OnChipMemoryOpt()
{


}

OnChipMemoryOpt::~OnChipMemoryOpt()
{


}


SgInitializedName* OnChipMemoryOpt::registerOptimizer(const std::set<SgInitializedName*>& readOnlyVars,
						      SgFunctionDeclaration* kernel, 
						      SgInitializedName* candidateVar,
						      SgScopeStatement* varScope,  //NULL
						      bool loadEvenCountZero, //false
						      int updown) //can be -1 0 1 
{
  //step1: create a variable to hold variable's global memory value 
  //step2: read the global memory reference into the register if it is read, if it is first 
  //written we do not need to do this. 
  //step3: replace all the global memory references with the register reference
  //step4: write back the register value to the global memory if the variable is not read-only

  ROSE_ASSERT(updown == -1 || updown == 0 || updown == 1);

  SgBasicBlock* kernel_body = kernel->get_definition()->get_body();

  //find the scope to declare the variable 
  if(varScope == NULL)
    varScope = CudaOptimizerInterface::findScopeForVarDeclaration(kernel->get_definition(), candidateVar);

  ROSE_ASSERT(varScope);
  //need to find a better way to find the location 
  //this is a quick fix, but didn't like it. 

  //step1: create a variable to hold variable's global memory value 
  //step2: and initialize it with the global mem access
  //float rU = U[_gidy][_gidx]

  string orig_name = candidateVar->get_name().str();
  SgType* var_type = candidateVar->get_type() ;

  SgType* base_type = var_type->findBaseType();

  SgPntrArrRefExp* glmem = CudaOptimizerInterface::createGlobalMemoryReference(varScope, candidateVar, updown);
  
  SgAssignInitializer* initReg = NULL;

  if( !CudaOptimizerInterface::isWrittenBeforeRead(kernel, candidateVar))
    initReg = buildAssignInitializer(glmem);
  
  string regVarName = REG_SUFFIX + orig_name ;

  if(updown == -1)
    regVarName = "up_" + regVarName;
  else if (updown == 1)
    regVarName = "down_" + regVarName;;

  //cout << regVarName << endl ; 

  SgVariableDeclaration* reg_decl =  buildVariableDeclaration(regVarName, base_type, initReg, varScope);

  SgVarRefExp* regExp  = buildVarRefExp(reg_decl);  
  
  //step3:
  SgInitializedName* reg_name = getFirstInitializedName(reg_decl);

  int how_many = replaceGlobalMemRefWithRegister(kernel_body, candidateVar , reg_name, updown);

  //either there is at least one reference or load even the count zero flag is set 
  ROSE_ASSERT((how_many > 0) || loadEvenCountZero); 

  if(how_many != 0 || loadEvenCountZero) {
    prependStatement(reg_decl, varScope );
    //step4:
    //checking if the variable is read-only 
    if( ! CudaOptimizerInterface::isReadOnly(readOnlyVars,candidateVar))
      {	
	//we need a new references (glmem), cannot use the same reference from the previous, otherwise their pntr will be the same.
	//or use deep copy
	//SgPntrArrRefExp* glmem = CudaOptimizerInterface::createGlobalMemoryReference(varScope, candidateVar, updown);
	SgExprStatement * writeBack= buildAssignStatement(deepCopy(glmem) , regExp);	
	appendStatement(writeBack, varScope);
      }
  }

  return reg_name;
}



bool OnChipMemoryOpt::isRegisterReplaceable(std::vector<SgExpression*> subscripts, 
					    int updown)
{
  //checks if array reference can be replace with its register counterparts
  //depending on updown we replace  [idz + 0, 1 or -1][idy][idx] with the register

  //check if subsrcipts are _gidx, _gidy or _gidz
  std::vector<SgExpression*>::iterator it;
  
  size_t count = 0;
  for(it= subscripts.begin(); it != subscripts.end(); it++)
    {      
      SgExpression* exp = isSgExpression(*it);
      Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(exp, V_SgVarRefExp);

      if(nodeList.size() != 1) //there should be only 1 variable that should be gidx,y,z
	return false;
      
      string index = exp->unparseToString();      
      
      //variable name should be one of the followings
      if(index.find(GIDZ) == string::npos && index.find(GIDY) == string::npos && index.find(GIDX) == string::npos)
	return false;
      if(index.find(GIDX) != string::npos && index != GIDX)
	return false;
      if(index.find(GIDY) != string::npos && index != GIDY)
	return false;
      if(index.find(GIDZ) != string::npos && index != GIDZ && updown==0)
	return false;

      if(index.find(GIDZ) != string::npos && updown != 0){
	//we want either one add or subtract operation in the index expression
	//no complex indexing is supported for now. 
	//A[i+2][i-4] is valid but A[i*2] is not valid
	Rose_STL_Container<SgNode*> opList1 = NodeQuery::querySubTree(exp, V_SgAddOp);
	Rose_STL_Container<SgNode*> opList2 = NodeQuery::querySubTree(exp, V_SgSubtractOp);
	
	if ( opList1.size() != 1 && updown == 1 ) //down 
	  return false;

	if ( opList2.size() != 1 && updown ==-1 ) //up  
	  return false;

	Rose_STL_Container<SgNode*> constList = NodeQuery::querySubTree(exp, V_SgIntVal);

	if(constList.size() != 1)
	  return false; 
	   
	//one of the operands in the index calculation should be constant
	SgExpression* constValue = isSgExpression(*(constList.begin()));
	    
	string constValStr = constValue ->unparseToString();
	if(constValStr == "1" )
	  count++;
	else 
	  return false;
      }
      else 
	count++; //either gidx or y
    }

  return (count == subscripts.size()) ? true : false ;

}


int OnChipMemoryOpt::replaceGlobalMemRefWithRegister(SgBasicBlock* kernel, 
						     SgInitializedName* candidateVar,
						     SgInitializedName* reg, 
						     int updown)
{
  //TODO:short
  //check also subscripts are gidx and gidy only as you check the array name
  //we can not replace E[i][j+1] with rE because rE is E[i][j]

  ROSE_ASSERT(kernel);
  ROSE_ASSERT(candidateVar);
  ROSE_ASSERT(reg);

  size_t count=0;
  size_t dim = MintArrayInterface::getDimension(candidateVar);

  string candidateVarName = candidateVar->get_name().str();

  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(kernel, V_SgPntrArrRefExp);

  Rose_STL_Container<SgNode*>::reverse_iterator arrayNode = nodeList.rbegin();
  
  for(; arrayNode != nodeList.rend(); arrayNode++)
    {
      ROSE_ASSERT(*arrayNode);

      SgExpression* arrayExp; 
      vector<SgExpression*>  subscripts; //first index is i if E[j][i]

      SgExpression* arrRefWithIndices = isSgExpression(*arrayNode);

      if(MintArrayInterface::isArrayReference(arrRefWithIndices, &arrayExp, &subscripts))
	{
	  SgInitializedName *arrayName = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayExp));

	  string var_name= arrayName->get_name().str();	

	 // bool valid = true;
	  //check if array name matches
	  if(var_name == candidateVarName && subscripts.size() == dim && isRegisterReplaceable(subscripts, updown))
	    {		
	      SgVarRefExp *newRegExp= buildVarRefExp(reg,kernel->get_scope());
	    
	      replaceExpression(arrRefWithIndices, isSgExpression(newRegExp));
	      count++;
	    }
	}
    }
  
  return count; 
}



bool OnChipMemoryOpt::isIndexPlusMinusConstantOp(SgExpression* exp, string index)
{
  //check if the expression contains 
  //j +/- c where c is a constant and j is an index variable  

  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(exp, V_SgVarRefExp);

  Rose_STL_Container<SgNode*>::reverse_iterator var ;

  ROSE_ASSERT(nodeList.size() == 1); //there should be only one variable reference
                                     //and that should be index, see the following code

  for(var= nodeList.rbegin(); var != nodeList.rend(); var++)
    {
      SgInitializedName *indexName = SageInterface::convertRefToInitializedName(isSgVarRefExp (*var));

      string var_name= indexName->get_name().str();	
      
      if(var_name != GIDY &&  var_name != GIDZ && var_name != GIDX)
	return false;
    }

  //we want either one add or subtract operation in the index expression
  //no complex indexing is supported for now. 
  //A[i+2][i-4] is valid but A[i*2] is not valid
  Rose_STL_Container<SgNode*> opList1 = NodeQuery::querySubTree(exp, V_SgAddOp);
  Rose_STL_Container<SgNode*> opList2 = NodeQuery::querySubTree(exp, V_SgSubtractOp);

  ROSE_ASSERT(opList1.size() == 1 || opList2.size() == 1);

  Rose_STL_Container<SgNode*> constList = NodeQuery::querySubTree(exp, V_SgIntVal);

  //one of the operands in the index calculation should be constant
  ROSE_ASSERT(constList.size() == 1 );
  SgExpression* constValue = isSgExpression(*(constList.begin()));

  string constValStr = constValue ->unparseToString();

  //higher orders : order 2, 4 and 6 are supported. 
  if(constValStr == "0" || constValStr == "1" || constValStr == "2" || constValStr == "3")
    return true;

  return false; 
}


void OnChipMemoryOpt::insertSynchPoints(SgFunctionDeclaration* kernel)
					
{
  //we want to add a synch point right before the first read of the shared
  //memory block because before that it is all loads into shared memory

  cout << "  INFO:Mint: inserting synchronization points " << endl;

  SgFunctionDefinition* func_def = kernel->get_definition();
  SgBasicBlock* kernel_body = func_def->get_body();
  
  Rose_STL_Container<SgNode*> list = NodeQuery::querySubTree(kernel_body, V_SgStatement);
  
  Rose_STL_Container<SgNode*>::iterator stmt;

  bool done = false; 
  for(stmt = list.begin(); stmt !=  list.end() && !done ; stmt++)
  {

    SgStatement* s = isSgStatement(*stmt);
    ROSE_ASSERT(s);

    if(! isSgScopeStatement(s)){
      
      Rose_STL_Container<SgNode*> varlist = NodeQuery::querySubTree(s, V_SgVarRefExp);

      Rose_STL_Container<SgNode*>::iterator exp;
      for(exp = varlist.begin(); exp !=  varlist.end() && !done ; exp++)
	{
	  SgVarRefExp* var = isSgVarRefExp(*exp);
	  ROSE_ASSERT(var);

	  string var_str = var->unparseToString(); 

	  if (var_str.find(SHARED_BLOCK) != string::npos)
	    {
	      std::set<SgInitializedName*> readVars;
	      std::set<SgInitializedName*> writeVars;
	      //get read and write variables for the previous statement
	      collectReadWriteVariables(s, readVars, writeVars);
	      
	      for(std::set<SgInitializedName*>::const_iterator it = readVars.begin(); it!= readVars.end(); it++)
		{
		  string var_name = (*it) -> get_name().str();
		  
		  //are we reading the value in the shared memory? if yes, then we need to add a sync point right before that
		  //this is the first time, we are reading the content of the shared memory
		  if (var_name.find(SHARED_BLOCK) != string::npos)
		    {
		      SgScopeStatement* varScope = s->get_scope();
		      
		      //insert a sync point                              
		      SgStatement *syncthreads= buildFunctionCallStmt("__syncthreads", buildVoidType(), NULL, varScope);                                                                                             
		      ROSE_ASSERT(syncthreads);                            
		      
		      insertStatementBefore(s, syncthreads);
		      
		      //we are done, can exit now. 
		      done = true; 
		      break;
		    }
		}
	    } //end of if 
	}
    }//end of !isSgBlock

  }//end of for 

}//end of function


void OnChipMemoryOpt::removeUnnecessarySynchPoints(SgFunctionDeclaration* kernel, 
						   const SgInitializedName* sh_block)
{
  //we want to remove a sync point if it is between two write statements and there is 
  //no read statments in between
  //sh_block [][] = A[][]
  //sync <-----------------remove this statement
  //sh_block [][] = A[][]

  //we want to remove sync points if one follows the other
  //....
  //synch
  //synch <--------------remove one of them
  //...

  SgFunctionDefinition* func_def = kernel->get_definition();
  SgBasicBlock* kernel_body = func_def->get_body();
  
  Rose_STL_Container<SgNode*> syncPointList = NodeQuery::querySubTree(kernel_body, V_SgFunctionCallExp);
  
  Rose_STL_Container<SgNode*>::iterator s;

  for(s = syncPointList.begin(); s != syncPointList.end(); s++)
  {
    SgFunctionCallExp* syncPoint = isSgFunctionCallExp(*s);

    ROSE_ASSERT(syncPoint);
   
    //check if it is a threads synch call
    if(syncPoint->unparseToString() == "__syncthreads()")
    {     
      int yes = 0 ; // should we remove this statement? // 1 means yes

      SgStatement* stmt = getEnclosingStatement(syncPoint);
      ROSE_ASSERT(stmt);

      //previous statement has to be a write into sh_block
      SgStatement* prev = getPreviousStatement(stmt);
      ROSE_ASSERT(prev);

      while(!isSgForStatement(prev) && !isSgIfStmt(prev) && yes== 0 )
      {
	std::set<SgInitializedName*> writeVars;
	std::set<SgInitializedName*> readVars;
	
	//get read and write variables for the previous statement
	collectReadWriteVariables(prev, readVars, writeVars);

	for(std::set<SgInitializedName*>::const_iterator it = writeVars.begin(); it!= writeVars.end(); it++)
	  {
	    //check if it is a shared memory access 
	    if ((*it) -> get_name().str() == sh_block->get_name().str()) {
	      yes = 1 ; 
	      break;
	   }
	  }
	for(std::set<SgInitializedName*>::const_iterator it = readVars.begin(); it!= readVars.end(); it++)
	  {
	    //we are reading the value in shared memory, we cannot remove the synch point that comes afterwords
	    if ((*it) -> get_name().str() == sh_block->get_name().str()) {
	      yes = -1;
	      break;
	   }
	  }
	//get the previous statement 
	prev = getPreviousStatement(prev);
      }

      SgStatement* next = getNextStatement(stmt);
	
      //yes = 1 means that we found a shared memory write, now let's look at if we have another write after the synch point
      if(yes == 1)
	{

	  while(next != NULL )
	    {
	      std::set<SgInitializedName*> writeVars;
	      std::set<SgInitializedName*> readVars;
	      
	      collectReadWriteVariables(next, readVars, writeVars);
	      
	      bool isWriteOnly  = false; 
	      for(std::set<SgInitializedName*>::const_iterator it = writeVars.begin(); it!= writeVars.end(); it++)
		{
		  if ((*it) -> get_name().str() == sh_block->get_name().str()) {
		    isWriteOnly = true; 
		    break;
		  }
		}
	      for(std::set<SgInitializedName*>::const_iterator itt = readVars.begin(); itt!= readVars.end(); itt++)
		{
		  if ((*itt) -> get_name().str() == sh_block->get_name().str()) {
		    isWriteOnly = false; 
		    break;
		  }
		}
	      if(isWriteOnly ){
		removeStatement(stmt);// it is between two shared memory writes, we can remove the synch point  
		break; //break from the while loop
	      }
	      next = getNextStatement(next);
	    }//end of while

	}//end of if
    }
  }
}


bool isOptimizableGhostCellsLoads(std::set<SgExpression*> ghostCells, 
				  MintForClauses_t clauseList, int order , int dim )
{
  //this checks if the stencil operations are suitable for ghost cell optimizations
  //Criterian
  //1.square blocks
  //ghostcells should be from 4 sides 
  //we need to add more restriction, more tests here 

  if(dim != 3)
    return false; 
  if(order != 1)
    return false; 
  if(clauseList.tileDim.x != clauseList.tileDim.y)
    return false;

  return true; 
}


void resetThreadIDs(SgBasicBlock* kernel_body, int order)
{

  SgDeclarationStatement* src_location = NULL;

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

  if(gidx_sym != NULL){
    SgStatement* stmt_x = buildAssignStatement( buildVarRefExp(IDX, scope), buildAddOp(buildVarRefExp("threadIdx.x", scope), buildIntVal(order)));
    insertStatementAfter(src_location, stmt_x);
  }
  if(gidy_sym != NULL){
    SgStatement* stmt_y = buildAssignStatement( buildVarRefExp(IDY, scope), buildAddOp(buildVarRefExp("threadIdx.y", scope), buildIntVal(order)));
    insertStatementAfter(src_location, stmt_y);
  }
  if(gidz_sym != NULL){
    SgStatement* stmt_z = buildAssignStatement( buildVarRefExp(IDZ, scope), buildIntVal(1));
    insertStatementAfter(src_location, stmt_z);
  }

}

void insertKernelConfigDefineDeclarations(SgInitializedName* sh_block, 
					  MintForClauses_t clauseList)
{
  //Adds #define TILE_X tileDim.x value into the kernel definition
  //for now we only define tile_x and tile_y
  //if we need we can add chunksize or z-dim parameters as well
  
  SgDeclarationStatement* sh_block_decl = sh_block->get_definition();

  SgStatement* stmt = getPreviousStatement(sh_block_decl);

  if(isSgFunctionDefinition(stmt))
    {
      std::stringstream tile_x; 
      std::stringstream tile_y;
      
      tile_x << "#define " <<TILE_X << " " << clauseList.tileDim.x ;
      tile_y << "#define " << TILE_Y << " " << clauseList.tileDim.y ;
      
      SageBuilder::buildCpreprocessorDefineDeclaration(sh_block_decl, tile_x.str());
      SageBuilder::buildCpreprocessorDefineDeclaration(sh_block_decl, tile_y.str());
    }
  else 
    {
      //do nothing. We already define params, this is the second time 
      //we declare a shared mem block
    }
}
void  OnChipMemoryOpt::sharedMemoryOptimizer(SgFunctionDeclaration* kernel, 
					     const std::set<SgInitializedName*>& readOnlyVars,
					     SgInitializedName* candidateVar,
					     const MintForClauses_t& clauseList, int num_planes, bool first, 
					     int order)
{
  //first = true means that this is the first shared memory variable 

  //step1: perform higher order analysis to find the order of the stencil 
  //step2: perform corner analysis to determine how many planes we need in the shared memory 
  //step3: create a shared memory block to hold the values in the shared memory
  //step4: find all the shareable references to replace their reference with the global memory references 
  //step5: load ghost cells 
  //step6: load center cell 
  //step7: replace all the global memory references with their shared counterparts. 

  SgStatement* sourcePosOutside = NULL;
  SgFunctionDefinition* func_def = kernel->get_definition();
  SgBasicBlock* kernel_body = func_def->get_body();
  //SgScopeStatement* scope = kernel_body->get_scope();

  SgVariableSymbol* index_sym = CudaOptimizerInterface::getSymbolFromName(kernel_body, GIDZ);

  if(index_sym == NULL ){
    index_sym = CudaOptimizerInterface::getSymbolFromName(kernel_body, GIDY);
    if(index_sym == NULL){
      index_sym = CudaOptimizerInterface::getSymbolFromName(kernel_body, GIDX);
    }
  }

  SgInitializedName* i_name = index_sym->get_declaration(); 
  ROSE_ASSERT(i_name);
  sourcePosOutside= i_name ->get_declaration();

  SgInitializedName* sh_block = NULL;
  
  //this flag is essential to derive information about how many planes to keep in shared memory 
  bool unrollWithoutRegisters = (clauseList.chunksize.z != 1) && (!MintOptions::GetInstance()->isRegisterOpt());

  int dimension = MintArrayInterface::getDimension(candidateVar);

  /*
  if(dimension > 3 || dimension < 2)
    return;
  int order = StencilAnalysis::performHigherOrderAnalysis(kernel_body, candidateVar);

  //TODO: should we return if the order is 0 or too high ? 
  //what do you think? Yes, we should use registers if the order is 0, because there is no sharing.
  if(order == 0){
    cout << "  INFO:Mint: Order of this array is " << order ;
    cout << ". No sharing, no need to use shared memory " << endl;
    return ;
  }
  cout << "  INFO:Mint: Order of this array is " << order << endl ;

  order = order > MAX_ORDER ? MAX_ORDER : order ;
  */

  //bool corner_xy = false;  
  bool corner_yz = false; 
  //bool corner_xz = false;

  //creates a shared memory block __shared__ float sh_block[clauseList.y + order ][ clauseList.x + order];
  if(dimension == 3 ){

    corner_yz = num_planes == 3 ? true : false; 

    //cout << "  INFO:Mint: Applying corner stencil analysis "<< endl ;
    //StencilAnalysis::performCornerStencilsAnalysis(kernel_body, candidateVar, corner_xy, corner_xz, corner_yz); 

    if(corner_yz)
      cout << "  INFO:Mint: Corner points are involved in computation." << endl ; 

    cout << "  INFO:Mint: Creating shared memory block "<< endl ; 

    if(corner_yz || unrollWithoutRegisters)
      sh_block = CudaOptimizerInterface::createSharedMemoryBlock(kernel, candidateVar, dimension, order);
    else 
      sh_block = CudaOptimizerInterface::createSharedMemoryBlock(kernel, candidateVar, dimension-1, order);
  }
  else
    sh_block = CudaOptimizerInterface::createSharedMemoryBlock(kernel, candidateVar, dimension, order);

  if(first)
    insertKernelConfigDefineDeclarations(sh_block, clauseList);

  //need to check this call
  SgScopeStatement* varScope = CudaOptimizerInterface::findScopeForVarDeclaration(func_def, candidateVar);  
  ROSE_ASSERT(varScope);

  //need to define up, center and down variables 
  unrollWithoutRegisters = (corner_yz && clauseList.chunksize.z != 1) || unrollWithoutRegisters ; 

  if(unrollWithoutRegisters )
    {
      //need to check this block

      SgVariableSymbol* upper_sym = CudaOptimizerInterface::getSymbolFromName(kernel_body, "_upper" + GIDZ);
      SgScopeStatement* upper_scope;    
      if(upper_sym != NULL)
	upper_scope = upper_sym->get_scope();
      else 
	upper_scope = varScope;

      SgVariableSymbol* down_sym = CudaOptimizerInterface::getSymbolFromName(kernel_body, "_down");
      
      if(down_sym != NULL) {// is it second time?
	SgInitializedName* i_name = down_sym->get_declaration(); 
	ROSE_ASSERT(i_name);
	sourcePosOutside= i_name ->get_declaration();
      }
      else {
	SgVariableDeclaration* upIndex = buildVariableDeclaration("_up", buildIntType(), buildAssignInitializer(buildIntVal(0)), upper_scope);
	SgVariableDeclaration* centerIndex = buildVariableDeclaration("_ctr", buildIntType(), buildAssignInitializer(buildIntVal(1)), upper_scope);
	SgVariableDeclaration* downIndex = buildVariableDeclaration("_down", buildIntType(), buildAssignInitializer(buildIntVal(2)), upper_scope);
	ROSE_ASSERT(upIndex);
	
	//declare up, center and down index variables 
	prependStatement(downIndex, upper_scope);    
	prependStatement(centerIndex, upper_scope);
	prependStatement(upIndex, upper_scope);
	
	//int tmp = down ;
	//down = up ;
	//up = ctr ;
	//ctr = tmp ;
	
	SgVarRefExp* upRef = buildVarRefExp(upIndex);
	SgVarRefExp* downRef = buildVarRefExp(downIndex);
	SgVarRefExp* centerRef = buildVarRefExp(centerIndex);
	
	SgVariableDeclaration* tmpIndex = buildVariableDeclaration("_tmpIndex", buildIntType(), buildAssignInitializer(downRef), varScope);
	SgStatement* swap1 = buildAssignStatement(downRef, upRef);
	SgStatement* swap2 = buildAssignStatement(upRef, centerRef);
	SgStatement* swap3 = buildAssignStatement(centerRef, buildVarRefExp(tmpIndex));
      
	appendStatement(tmpIndex, varScope);
	appendStatement(swap1, varScope);
	appendStatement(swap2, varScope);
	appendStatement(swap3, varScope);
	sourcePosOutside = downIndex ; 

      }
    }

  std::set<SgExpression*> ghostCells;
  std::vector<SgExpression*> shareableRefList;

  cout << "  INFO:Mint: Finding all the shareable references " << endl ; 

  findAllShareableReferences(func_def, candidateVar, shareableRefList, ghostCells, corner_yz || unrollWithoutRegisters);

  cout << "  INFO:Mint: Found " << shareableRefList.size() << " many shareable references "<< endl;
  cout << "  INFO:Mint: Found " << ghostCells.size() << " many ghost cells  "<< endl;
  
  int ghostCellsSize = ghostCells.size();
  int shareableRefListSize = shareableRefList.size();

  if(ghostCellsSize > 0 ){

    cout << "  INFO:Mint: Handling ghost cell loads " << endl ; 
    //need to check this block
      
     if(isOptimizableGhostCellsLoads(ghostCells, clauseList, order, dimension))
      {
	//cout << "  INFO:Mint: Optimizing ghost cell loads "<<endl;
	
	GhostCellHandler::loadGhostCellsIntoSharedMemory(func_def, candidateVar, sh_block, 
							 corner_yz, order, unrollWithoutRegisters, sourcePosOutside, varScope);

	GhostCellHandler::defineGhostCellVariables(func_def, candidateVar, order, corner_yz);
      }
     else
	GhostCellHandler::loadGhostCellsIntoSharedMemory(func_def, candidateVar, sh_block, 
							 ghostCells, corner_yz, order, unrollWithoutRegisters, sourcePosOutside, varScope);

     ghostCells.clear();
  }
  
  SgPntrArrRefExp* lhs = NULL;
  if(shareableRefListSize > 0 ){

    cout << "  INFO:Mint: Replacing all the global memory references with shared memory references " << endl; 

    //replace all the global memory references with the shared memory references 
    replaceGlobalMemRefWithSharedMem(func_def, candidateVar, sh_block, 
    				     shareableRefList, corner_yz, unrollWithoutRegisters);
   
    shareableRefList.clear();
    
    if(corner_yz || unrollWithoutRegisters){
      loadAPlaneIntoSharedMemory(func_def, candidateVar, sh_block, varScope,  1, unrollWithoutRegisters); //down
      loadAPlaneIntoSharedMemory(func_def, candidateVar, sh_block, varScope, -1, unrollWithoutRegisters); //up
    }
        
    cout << "  INFO:Mint: Loading center plane into shared memory" << endl; 

    lhs = loadAPlaneIntoSharedMemory(func_def, candidateVar, sh_block, varScope, 0, unrollWithoutRegisters);//center
    
  }
  //May 5, 2011 I removed this function because it takes so much time and introduces 
  //side effect error. Instead, now we insert only few _synch points. Only one variable is responsible 
  //inserting synch points. TODO: check if this is OK with chunksize?
  /*
  if(ghostCellsSize > 0 || shareableRefListSize > 0 ) 
    {
      cout << "  INFO:Mint: Removing unnecessary sync points. " << endl ; 
      removeUnnecessarySynchPoints(kernel, sh_block);
    }
  */
  if(ghostCellsSize == 0 || shareableRefListSize == 0 ) 
    {
      //TODO:Remove the declaration of the shared memory block if there is no sharing 
      SgDeclarationStatement* sh_block_decl = sh_block->get_definition();
      ROSE_ASSERT(sh_block_decl);
      removeStatement(sh_block_decl);
    }

  //We need to adjust the offset of the thread IDs because 
  //order may not be the same as the for loop lower bound
  if(first)
    resetThreadIDs(kernel_body, order);

  //step4:
  //checking if the variable is read-only 
  if(!CudaOptimizerInterface::isReadOnly(readOnlyVars,candidateVar) && lhs!=NULL)
    {
      //write back the value in the shared memory to global memory 
      SgPntrArrRefExp* glmem= CudaOptimizerInterface::createGlobalMemoryReference(varScope, candidateVar, 0);
      SgExprStatement* writeBack = buildAssignStatement( glmem , lhs);

      //Find the source location to insert the write back statement
      SgVariableSymbol* tmp_index = CudaOptimizerInterface::getSymbolFromName(kernel_body, "_tmpIndex");
      
      if(tmp_index != NULL) {// is it second time?
	SgInitializedName* i_name = tmp_index->get_declaration(); 
	ROSE_ASSERT(i_name);
	SgStatement* sourcePos= i_name ->get_declaration();
	insertStatementAfter(sourcePos, writeBack);
      }
      else
	appendStatement(writeBack, varScope);	
    }
  
}



SgPntrArrRefExp* OnChipMemoryOpt::loadAPlaneIntoSharedMemory(SgFunctionDefinition* kernel, 
						 SgInitializedName* candidateVar, 
						 SgInitializedName* sh_block,
						 SgScopeStatement* varScope,
						 int updown, // can be -1 0  1
						 bool unrollWithoutRegisters) 
{
  //sh[idz][idy][idx] = A[z][y][x]
  //sh[up][idy][idx] = A[z-1][y][x] //when chunking
  //sh[down][idy][idx] = A[z+1][y][x] //when chunking
  //sh[idz-1][idy][idx] = A[z-1][y][x]
  //sh[idz+1][idy][idx] = A[z+1][y][x]

  //if regSameShared, then sh[][][] = rA

  //if we are unrolling(chunking) then idz is up, down or ctr
  //otherwise it is idz + 1 or -1

  SgBasicBlock* kernel_body = kernel->get_body();
  SgScopeStatement* scope =kernel_body->get_scope();   
  
  SgDeclarationStatement* upper_decl = NULL;
  SgVariableSymbol* upper_sym = CudaOptimizerInterface::getSymbolFromName(kernel_body, "_upper" + GIDZ);

  if(upper_sym != NULL){
    SgInitializedName* upper_name = upper_sym->get_declaration(); 
    ROSE_ASSERT(upper_name);
    upper_decl = upper_name ->get_declaration();
    ROSE_ASSERT(upper_decl);
  }

  ROSE_ASSERT(varScope);
 
  int dimArray = MintArrayInterface::getDimension(candidateVar);
  int dimSh = MintArrayInterface::getDimension(sh_block);

  SgExpression* idz= buildAddOp(buildVarRefExp(IDZ, varScope), buildIntVal(updown));
  SgVarRefExp* idy = buildVarRefExp(IDY, varScope);
  SgVarRefExp* idx = buildVarRefExp(IDX, varScope);      

  if(unrollWithoutRegisters){
    string indexstr = updown == 0 ? "_ctr" : "_up" ;
    indexstr = updown == 1 ? "_down" : indexstr ; 
    SgScopeStatement* outerScope = upper_sym->get_scope();
    ROSE_ASSERT(outerScope);
    idz = buildVarRefExp(indexstr, outerScope);
  }
  SgVarRefExp* gidz=NULL, *gidy=NULL ;
  //SgVarRefExp* gidx = buildVarRefExp(GIDX, varScope);      

  if(dimArray == 3)
    gidz= buildVarRefExp(GIDZ, varScope);
  if(dimArray >= 2)
    gidy = buildVarRefExp(GIDY, varScope);
 
  //create reference sh[idz][idy][idx]
  SgPntrArrRefExp* lhs = NULL;
  if(dimSh == 3 )  {
      lhs = buildPntrArrRefExp( buildVarRefExp(sh_block, scope), idz);
      lhs  = buildPntrArrRefExp( lhs, idy);
      lhs  = buildPntrArrRefExp( lhs, idx);
    }
  if(dimSh == 2){
    lhs = buildPntrArrRefExp( buildVarRefExp(sh_block, scope), idy);
    lhs  = buildPntrArrRefExp( lhs, idx);
  }
  else if(dimSh == 1){
      lhs = buildPntrArrRefExp( buildVarRefExp(sh_block, scope), idx);
  }

  ROSE_ASSERT(lhs);
  SgExprStatement* block = NULL;

  bool regOpt = MintOptions::GetInstance()->isRegisterOpt() && (updown == 0);
  if(regOpt)
    {
      //sh_block [][] = rH
      string reg_name = candidateVar->get_name().str();
      SgVarRefExp* rhs = buildVarRefExp(REG_SUFFIX + reg_name, varScope);
    
      block= buildAssignStatement(lhs , rhs);  
  
      ROSE_ASSERT(block);

      Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(varScope, V_SgVariableDeclaration);
      Rose_STL_Container<SgNode*>::iterator it = nodeList.begin();
      bool notfound = true; 

      for(it = nodeList.begin(); it != nodeList.end(); it++)
	{	
	  SgVariableDeclaration* decl = isSgVariableDeclaration(*it);	    
	  SgVariableSymbol* reg_sym = getFirstVarSym(decl);
	  SgInitializedName* i_name = reg_sym->get_declaration();
	  
	  if(i_name->get_name().str() == REG_SUFFIX + reg_name)
	    {
	      //this the stmt: float rUold = Uold[_gidz][_gidy][_gidx];
	      //this is block: _sh_block[_idy][_idx] = rUold;
	      SgStatement* stmt = SageInterface::getEnclosingStatement(decl);
	      insertStatementAfter(stmt, block);
	      notfound = false; 
	      break;
	    }
	}
      //in sliding window, above forloop will not find the var decl, 
      //so we should just prepend it. 
      if(notfound)
	prependStatement(block, varScope);
    }
  //if regSameShared == false 
  else //block[idy][idx] = ((U[_idy])[_idx]) 
    {

      SgPntrArrRefExp* rhsExp = CudaOptimizerInterface::createGlobalMemoryReference(varScope, candidateVar, updown);

      ROSE_ASSERT(rhsExp);

      block= buildAssignStatement(lhs , rhsExp);  
      ROSE_ASSERT(block);

      if(unrollWithoutRegisters && updown != 1){ 
	//only up and center should loaded outside of the loop
	insertStatementAfter(upper_decl, block);
      }
      else //these ones should be loaded inside the loop 
	prependStatement(block, varScope);
    }
 
  /*
  if(insertSync)
    {
      if((updown == 0 && !unrollWithoutRegisters) || (updown == 1 && unrollWithoutRegisters))
	{
	  SgStatement *syncthreads= buildFunctionCallStmt("__syncthreads", buildVoidType(), NULL, varScope);
	  
	  ROSE_ASSERT(syncthreads);
	  ROSE_ASSERT(block);
	  
	  if(regOpt){
	    //if(regSameShared){
	    insertStatementAfter(block, syncthreads);
	  }
	  else {
	    insertStatementBefore(block,syncthreads );
	  }
	}
    }
  */
  return lhs;
}

void OnChipMemoryOpt::findAllShareableReferences(const SgFunctionDefinition* kernel, 
						 const SgInitializedName* candidateVar, 
						 std::vector<SgExpression*>& shareableRefList,						 
						 std::set<SgExpression*>& ghostCells, const bool corner_yz)
		       
{
  //TODO:long:if a variable written back to global memory, then I need to write back the value in the
  //shared memory to global memory before the exist from the kernel
  //need a test case

  bool regOpt = MintOptions::GetInstance()->isRegisterOpt();

  SgBasicBlock* kernel_body = kernel->get_body();
  ROSE_ASSERT(kernel_body);
  ROSE_ASSERT(candidateVar);

  size_t dim = MintArrayInterface::getDimension(candidateVar);

  string candidate_name = candidateVar->get_name().str();

  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(kernel_body, V_SgPntrArrRefExp);
  Rose_STL_Container<SgNode*>::iterator arrayNode = nodeList.begin();
  
  std::set<string> ghostCellsStr;

  for(; arrayNode != nodeList.end(); arrayNode++)
    {

      SgExpression* arrRefWithIndices = isSgExpression(*arrayNode);
      ROSE_ASSERT(arrRefWithIndices);
      
      //if the register candidate and shared memory candidates are the same then
      //we shouldn't replace the global mem to shared memory load.
      //e.g. double rh = A[][], do not replace with shared mem reference
      //it shouldn't become rh = _sh_block_A[][]
      if(regOpt)
      {
	SgStatement* stmt = SageInterface::getEnclosingStatement(arrRefWithIndices);
	if(isSgVariableDeclaration(stmt))
	  {
	    SgVariableDeclaration* decl = isSgVariableDeclaration(stmt);	    
	    SgVariableSymbol* reg_sym = getFirstVarSym(decl);
	    ROSE_ASSERT(reg_sym);
	    SgInitializedName* i_name = reg_sym->get_declaration();
	    ROSE_ASSERT(i_name);
	    if(i_name->get_name().str() == REG_SUFFIX + candidate_name)
	       continue; 	    
	  }
      }

      SgExpression* arrayExp = NULL; 
      vector<SgExpression*>  subscripts; //first index is k if E[j][i][k]

      if(MintArrayInterface::isArrayReference(arrRefWithIndices, &arrayExp, &subscripts))
	{
	  //check if array name matches, and the indices are in the shared memory range
	  if(subscripts.size()  == dim) 
	    {
	      SgInitializedName *arrayName = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayExp));
	      
	      string var_name= arrayName->get_name().str();	

	      if(var_name == candidate_name)
		{
		  if (StencilAnalysis::isShareableReference(subscripts, corner_yz))
		    {
		      string arrRefStr = arrRefWithIndices->unparseToString();
		      
		      //same ghost cell may appear more than once, count it only once
		      if(ghostCellsStr.find(arrRefStr) == ghostCellsStr.end())
			{
			  ghostCellsStr.insert(arrRefStr);
			  ghostCells.insert(arrRefWithIndices);
			}
		      shareableRefList.push_back(arrRefWithIndices);
		    }
		}
	      //skip sub array references 
	      arrayNode = arrayNode + dim - 1;
	    }	  
	}
    }
}


void OnChipMemoryOpt::replaceGlobalMemRefWithSharedMem(SgFunctionDefinition* kernel, 
						       const SgInitializedName* candidateVar, 
						       SgInitializedName* sharedRef,
						       const vector<SgExpression*> shareableArrRefList, 
						       const bool corner_yz,
						       const bool unrollWithoutRegisters)

{

  SgBasicBlock* kernel_body = kernel->get_body();

  size_t dim = MintArrayInterface::getDimension(candidateVar);

  ROSE_ASSERT(dim >= 1);	      

  string candidate_name = candidateVar->get_name().str();

  vector<SgExpression*>::const_iterator exp;

  for(exp = shareableArrRefList.begin(); exp != shareableArrRefList.end(); exp++)
    {

      SgExpression* arrayExp=NULL;

      vector<SgExpression*>  subscripts; //first index is k if E[j][i][k]

      SgExpression* arrRefWithIndices = isSgExpression(*exp);

      ROSE_ASSERT(arrRefWithIndices);

      if(MintArrayInterface::isArrayReference(arrRefWithIndices, &arrayExp, &subscripts))
	{

	  //SgInitializedName *arrayName = SageInterface::convertRefToInitializedName(isSgVarRefExp (arrayExp));

	  //Didem: March 02, 2011 
	  //we don't need to check the array name because shareable arr ref list only contains 
	  //references of the candidate array 
	  //string var_name= arrayName->get_name().str();	
	  //ROSE_ASSERT(var_name == candidate_name);
	  //ROSE_ASSERT(subscripts.size()== dim);

	  //check if array name matches, and the indices are in the shared memory range
	  if(subscripts.size()  == dim ) //&& var_name == candidate_name )
	    {
	      //get the index references for:  E[gidy + 1 ][gidx] first: gidx, second: gidy + 1	      
	      vector<SgExpression*>:: iterator it = subscripts.begin();
	      
	      SgExpression* first=NULL, *second=NULL, *third=NULL;
	      
	      first = copyExpression(*it);

	      if(subscripts.size() >= 2){
		second = copyExpression(*(++it));
		second = CudaOptimizerInterface::convertGlobalMemoryIndexIntoLocalIndex(second, IDY, kernel_body);
		ROSE_ASSERT(second);

		if(subscripts.size() >= 3 ) 
                {
		  if(corner_yz && !unrollWithoutRegisters){
		    third = copyExpression(*(++it));
		    third = CudaOptimizerInterface::convertGlobalMemoryIndexIntoLocalIndex(third, IDZ, kernel_body);
		    ROSE_ASSERT(third);
		  }
		  else if(unrollWithoutRegisters){
		    third = copyExpression(*(++it));
		    third = CudaOptimizerInterface::convertGlobalMemoryIndexIntoLocalIndex(third, "_ctr", kernel_body);
		    ROSE_ASSERT(third);
		  }
                }
	      }

	      first  = CudaOptimizerInterface::convertGlobalMemoryIndexIntoLocalIndex(first, IDX, kernel_body);
	    	      
	      ROSE_ASSERT(first);

	      SgExpression* sharedPntrRef = 
		CudaOptimizerInterface::createSharedMemoryReference(kernel, sharedRef, first, second, third); 

	      ROSE_ASSERT(sharedPntrRef);
	      
	      replaceExpression(arrRefWithIndices, sharedPntrRef);
	    }
	}
    }
}
