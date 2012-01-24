/*
  On Chip Memory Optimizer

  reduces global memory accesses with 

  the utilization of registers and shared memory


*/

#include "CudaOptimizerInterface.h"
#include "ASTtools.hh"
#include "../../midend/mintTools/MintOptions.h"
#include "../../midend/arrayProcessing/MintArrayInterface.h"

#include <string>
#include "OmpAttribute.h"

using namespace std;

CudaOptimizerInterface::CudaOptimizerInterface()
{


}

CudaOptimizerInterface::~CudaOptimizerInterface()
{


}


//! Creates an SgInitializedName.                                                                                              
static
SgInitializedName *
myCreateInitName (const string& name, SgType* type,
                SgDeclarationStatement* decl,
                SgScopeStatement* scope,
                SgInitializer* init = 0)
{
  SgName sg_name (name.c_str ());

  // DQ (2/24/2009): Added assertion.                                                                                            
  ROSE_ASSERT(name.empty() == false);
  SgInitializedName* new_name = new SgInitializedName (ASTtools::newFileInfo (), sg_name, type, init,decl, scope, 0);
  ROSE_ASSERT (new_name);
  // Insert symbol                                                                                                             
  if (scope)
    {
      SgVariableSymbol* new_sym = new SgVariableSymbol (new_name);
      scope->insert_symbol (sg_name, new_sym);
    }

  return new_name;
}

bool CudaOptimizerInterface::isReadOnly(const std::set<SgInitializedName*>& readOnlyVars, 
			       SgInitializedName* i_name)
{

  bool readOnly = false;
  if (readOnlyVars.find(const_cast<SgInitializedName*> (i_name)) != readOnlyVars.end())
    readOnly = true;

  return readOnly;
}



SgExpression* CudaOptimizerInterface::convertGlobalMemoryIndexIntoLocalIndex(SgExpression* exp, 
									     string index, 
									     SgBasicBlock* kernel)
{
  //Modified in March 02, 2011
  //convert gidx + c into idx + c 
  //convert gidz + 1 into down 
  //convert gidz - 1 into up 
  //convert gidz into ctr 

  SgVariableSymbol* sym = CudaOptimizerInterface::getSymbolFromName(kernel,index);
  ROSE_ASSERT(sym);

  SgScopeStatement* scope = sym->get_scope();

  SgExpression* indexExp = buildVarRefExp(index, scope);
 
  //we want either one add or subtract operation in the index expression
  //no complex indexing is supported for now. 
  //A[i+2][i-4] is valid but A[i*2] is not valid

  Rose_STL_Container<SgNode*> constList = NodeQuery::querySubTree(exp, V_SgIntVal);
  
  if( constList.size()==1)
    {
      Rose_STL_Container<SgNode*> opList1 = NodeQuery::querySubTree(exp, V_SgAddOp);
      Rose_STL_Container<SgNode*> opList2 = NodeQuery::querySubTree(exp, V_SgSubtractOp);

      SgIntVal* constantVal = isSgIntVal(*(constList.begin()));

      int val = constantVal->get_value();

      ROSE_ASSERT(opList1.size()== 1 || opList2.size() == 1);

      if(opList1.size() == 1)//add
	{
	  if(val == 1 && index == "_ctr")
	    indexExp = buildVarRefExp("_down", scope);
	  else 
	    indexExp = buildAddOp(indexExp, buildIntVal(val) ); 
	}
      else if (opList2.size() == 1)//subtract
	{
	  if(val == 1 && index == "_ctr")
	    indexExp = buildVarRefExp("_up", scope);
	  else
	    indexExp = buildSubtractOp(indexExp , buildIntVal(val)); 
	}
    }
  else{

    ROSE_ASSERT(constList.size()==0);

  }
  return indexExp;

}

SgPntrArrRefExp* CudaOptimizerInterface::createGlobalMemoryRefWithOffset(SgScopeStatement* varScope, 
									 SgInitializedName* candidateVar, 
									 SgExpression* offset)
{

  int dim = MintArrayInterface::getDimension(candidateVar);

  SgPntrArrRefExp* glmem=NULL;

  if(dim == 3){
    glmem  = buildPntrArrRefExp( buildVarRefExp(candidateVar, varScope), buildVarRefExp(GIDZ, varScope));
    glmem  = buildPntrArrRefExp( glmem, buildVarRefExp(GIDY, varScope));
    if(offset == NULL)
      glmem  = buildPntrArrRefExp( glmem, buildVarRefExp(GIDX, varScope));
    else 
      glmem  = buildPntrArrRefExp( glmem, buildAddOp(buildVarRefExp(GIDX, varScope), offset));
  }
  else if (dim == 2){
    glmem  = buildPntrArrRefExp( buildVarRefExp(candidateVar, varScope), buildVarRefExp(GIDY, varScope));
    if(offset == NULL)
      glmem  = buildPntrArrRefExp( glmem, buildVarRefExp(GIDX, varScope));
    else 
      glmem  = buildPntrArrRefExp( glmem, buildAddOp(buildVarRefExp(GIDX, varScope), offset));
  }
  else if (dim == 1){
    if(offset == NULL)
      glmem  = buildPntrArrRefExp( buildVarRefExp(candidateVar, varScope), buildVarRefExp(GIDX, varScope));
    else 
      glmem  = buildPntrArrRefExp( buildVarRefExp(candidateVar, varScope), buildAddOp(offset, buildVarRefExp(GIDX, varScope)));
  }
  ROSE_ASSERT(glmem);

  return glmem;
}


SgPntrArrRefExp* CudaOptimizerInterface::createGlobalMemoryReference(SgScopeStatement* varScope, 
								     SgInitializedName* candidateVar, 
								     int updown)
{

  int dim = MintArrayInterface::getDimension(candidateVar);

  SgPntrArrRefExp* glmem=NULL;

  if(dim == 3){
    if(updown == 1)
      glmem  = buildPntrArrRefExp( buildVarRefExp(candidateVar, varScope), buildAddOp(buildVarRefExp(GIDZ, varScope), buildIntVal(updown)));
    else if(updown == -1)
      glmem  = buildPntrArrRefExp( buildVarRefExp(candidateVar, varScope), buildSubtractOp(buildVarRefExp(GIDZ, varScope), buildIntVal(-updown)));
    else 
      glmem  = buildPntrArrRefExp( buildVarRefExp(candidateVar, varScope), buildVarRefExp(GIDZ, varScope));

    glmem  = buildPntrArrRefExp( glmem, buildVarRefExp(GIDY, varScope));
    glmem  = buildPntrArrRefExp( glmem, buildVarRefExp(GIDX, varScope));
  }
  else if (dim == 2){
    glmem  = buildPntrArrRefExp( buildVarRefExp(candidateVar, varScope), buildVarRefExp(GIDY, varScope));
    glmem  = buildPntrArrRefExp( glmem, buildVarRefExp(GIDX, varScope));
  }
  else if (dim == 1)
    glmem  = buildPntrArrRefExp( buildVarRefExp(candidateVar, varScope), buildVarRefExp(GIDX, varScope));

  ROSE_ASSERT(glmem);

  return glmem;
}


SgPntrArrRefExp* CudaOptimizerInterface::createSharedMemoryReference(SgScopeStatement* varScope, 
								     SgInitializedName* sh_block, 
								     SgExpression* first, 
								     SgExpression* second, 
								     SgExpression* third)
{

  SgPntrArrRefExp* shmem=NULL;

  if(third != NULL){
    shmem  = buildPntrArrRefExp( buildVarRefExp(sh_block, varScope), third);
    shmem  = buildPntrArrRefExp( shmem, second);
    shmem  = buildPntrArrRefExp( shmem, first);
  }
  else if (second != NULL){
    shmem  = buildPntrArrRefExp( buildVarRefExp(sh_block, varScope), second);
    shmem  = buildPntrArrRefExp( shmem, first);
  }
  else if (first != NULL)
    shmem  = buildPntrArrRefExp( buildVarRefExp(sh_block, varScope), first);

  ROSE_ASSERT(shmem);

  return shmem;
}


bool CudaOptimizerInterface::isWrittenBeforeRead(SgFunctionDeclaration *func, SgInitializedName* iname)
{
  
  SgBasicBlock* func_body = func->get_definition()->get_body();

  Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(func_body, V_SgVarRefExp);
    
  Rose_STL_Container<SgNode*>::iterator v;
  
  for(v = varList.begin(); v != varList.end(); v++)
    {
      ROSE_ASSERT(*v);
      
      SgVarRefExp* vExp = isSgVarRefExp(*v);
      
      SgInitializedName *varName = SageInterface::convertRefToInitializedName(vExp);
  
      string varNameStr= varName->get_name().str();	

      
      if(varNameStr == iname->get_name().str()){
	
	SgStatement* stmt = SageInterface::getEnclosingStatement(*v);
	
	//cout << stmt->unparseToString() << endl ;

	set<SgInitializedName*> readVars;
	set<SgInitializedName*> writeVars;
	
	SageInterface::collectReadWriteVariables(stmt, readVars, writeVars);
	
	if(readVars.find(iname) != readVars.end()) //variable is read
	  {
	    //cout << varNameStr << " is first read " << endl;
	    return false; 
	  }
	if (writeVars.find(iname) != writeVars.end()) 
	  {
	    //cout << varNameStr << " is first written " << endl;
	    return true;
	  }
      }
    }
  return false;
}



void CudaOptimizerInterface::removeOtherSharedBlocks(SgFunctionDeclaration* kernel)
{
    SgBasicBlock* kernel_body = kernel->get_definition()->get_body();

    Rose_STL_Container<SgNode*> list = NodeQuery::querySubTree(kernel_body, V_SgVariableDeclaration);
    Rose_STL_Container<SgNode*>::iterator it;

    for(it= list.begin(); it != list.end(); it++)
      {
	SgVariableDeclaration* decl = isSgVariableDeclaration(*it);
	
	ROSE_ASSERT(decl);

	SgInitializedName* i_name = getFirstInitializedName(decl);

	ROSE_ASSERT(i_name);

	if(i_name ->get_name().str() == SHARED_BLOCK)
	  {
	    SgStatement* stmt = getEnclosingStatement(isSgNode(decl));

	    ROSE_ASSERT(stmt);
	    removeStatement(stmt);
	  }
      }
    
}

SgInitializedName* CudaOptimizerInterface::createSharedMemoryBlock(SgFunctionDeclaration* kernel, 
								   const SgInitializedName* candidateVar,
								   const int dimension, const int order)
						
{
  //creates a shared memory block for a given candidate variable 
  //e.g. __shared__ float _sh_block_data[][]
  //the size of the shared memory block depends on the thread block size and order 

  SgBasicBlock* kernel_body = kernel->get_definition()->get_body();
  ROSE_ASSERT(kernel_body);

  SgScopeStatement* func_scope =kernel->get_scope();  
  ROSE_ASSERT(func_scope);

  SgStatement* first_stmt = SageInterface::getFirstStatement(kernel_body);
  ROSE_ASSERT(first_stmt);

  ROSE_ASSERT(candidateVar);

  SgType* varType = candidateVar->get_type();

  ROSE_ASSERT(varType);

  SgType* arrayType = varType->findBaseType();

  //float, double, int?
  ROSE_ASSERT(arrayType);

  if(dimension <= 3){
  
    SgVariableDeclaration* blockDim =  buildVariableDeclaration(TILE_X,  buildIntType(), NULL , kernel_body);

    SgExpression* blockdim_x = buildAddOp(buildVarRefExp(blockDim), buildIntVal(order*2));

    arrayType = buildArrayType(arrayType, blockdim_x);

    ROSE_ASSERT(arrayType);

    if(dimension >= 2)
      {

	SgVariableDeclaration* blockDimY =  buildVariableDeclaration(TILE_Y,  buildIntType(), NULL , kernel_body);
	
	SgExpression* blockdim_y = buildAddOp(buildVarRefExp(blockDimY), buildIntVal(order*2));
	
	arrayType = buildArrayType(arrayType, blockdim_y);
      }
    if(dimension == 3)
      {
	//we allow up to 3 planes in shared memory
	arrayType = buildArrayType(arrayType, buildIntVal(3));
      }
  }

  string arr_name = candidateVar->get_name().str();

  //we only create static shared memory blocks
  //TODO: dynamic allocation support for shared memory?

  SgVariableDeclaration* shared_block =  buildVariableDeclaration(SHARED_BLOCK + arr_name, arrayType, NULL, kernel_body);
  ROSE_ASSERT(shared_block);

  //setting __shared_ as its storage modifier _ 
  shared_block->get_declarationModifier().get_storageModifier().setCudaShared(); 

  //insert this variable into the symbol table of the kernel
  SgInitializedName* shared_block_name = myCreateInitName(SHARED_BLOCK + arr_name , arrayType, shared_block, kernel_body);
  ROSE_ASSERT(shared_block_name);

  SageInterface::insertStatementBefore(first_stmt, shared_block);

  return shared_block_name;
}






bool CudaOptimizerInterface::isSubScope(SgScopeStatement* parent, SgScopeStatement* child)
{//checks if the child scope is indeed the child scope of the parent scope

    NodeQuerySynthesizedAttributeType scopes = NodeQuery::querySubTree(parent, V_SgScopeStatement);

    NodeQuerySynthesizedAttributeType::const_iterator ii = scopes.begin();
      

    for (; ii!=scopes.end();++ii) {

      if(isSgNode(child) == isSgNode(*ii))
	{
	  return true;
	  //cout << (*p)->unparseToString() << endl << " is child of " << endl ; 
	  //cout << (*ii)->unparseToString() << endl << endl ; 
	}
    }

    return false;
}

SgScopeStatement* CudaOptimizerInterface::findCommonParentScope(SgScopeStatement* topScope, 
								SgScopeStatement* s1,
								SgScopeStatement* s2)
{//takes two scopes and find the smallest common scope until topScope

  SgScopeStatement* commonScope= NULL;

  if(isSubScope(s1,s2)) //s2 is a child scope of s1
    commonScope = s1;

  else if (isSubScope(s2,s1))//s1 is a child scope of s2
    commonScope = s2;

  else {//find the first commong parent scope 

    commonScope = topScope ; 

    SgScopeStatement* parent= isSgScopeStatement(isSgNode(s1)->get_parent());

    ROSE_ASSERT(parent);

    while(parent!= NULL && parent != topScope)
      {
	if(isSubScope(parent, s2))
	  {
	    commonScope = parent;
	    break;
	  }
       
	parent = isSgScopeStatement(isSgNode(parent)->get_parent());
      }
  }

  return commonScope;
}


vector<SgScopeStatement*>  CudaOptimizerInterface::getAllScopes(SgBasicBlock* func_body, SgInitializedName*& candidateVar)
{//in a function body finds all the scopes that candidatevar appears in

  vector<SgScopeStatement*> scopeList;

  NodeQuerySynthesizedAttributeType vars = NodeQuery::querySubTree(func_body, V_SgVarRefExp);
  
  NodeQuerySynthesizedAttributeType::const_iterator i = vars.begin();
  
  for (; i!=vars.end();++i) {
    
    SgVarRefExp* varRef = isSgVarRefExp(*i);
    
    SgInitializedName* initName = isSgInitializedName(varRef->get_symbol()->get_declaration());
    ROSE_ASSERT(initName);

    std::string var_name = initName -> get_name().str();

    if(candidateVar->get_name().str()== var_name )
      {
	SgStatement* stmt = getEnclosingStatement(varRef);
	SgScopeStatement* var_scope = stmt->get_scope();
	ROSE_ASSERT(var_scope);

	candidateVar = initName;

	vector<SgScopeStatement*>::const_iterator it = std::find(scopeList.begin(), scopeList.end(), var_scope);
	
	if(it == scopeList.end())//scope not found
	  {
	    scopeList.push_back(var_scope);
	  }
      }
  }
  return scopeList;
}

SgScopeStatement*  CudaOptimizerInterface::findScopeForVarDeclaration(SgFunctionDefinition* func, 
								      SgInitializedName*& candidateVar)
{

  SgBasicBlock* func_body = func->get_body();
  
  ROSE_ASSERT(func_body);
  ROSE_ASSERT(candidateVar);

  vector<SgScopeStatement*> scopeList = getAllScopes(func_body, candidateVar);
  
  ROSE_ASSERT(scopeList.size()!=0);
  
  vector<SgScopeStatement*>::iterator it = scopeList.begin(); 
  SgScopeStatement* parentScope = *it;	
  
  ROSE_ASSERT(parentScope);

  for(it++; it != scopeList.end(); it++)
    {
      SgScopeStatement* s2 = *it;
      ROSE_ASSERT(s2);
      parentScope = findCommonParentScope(func_body, parentScope, s2);
    }
  ROSE_ASSERT(parentScope);
  return parentScope;
}

SgVariableSymbol* CudaOptimizerInterface::getSymbolFromName(SgBasicBlock* block, string varStr)
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
