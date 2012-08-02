
#include "CudaMemoryManagement.h"
#include "../mintTools/MintTools.h"
#include <algorithm>
#include "../arrayProcessing/MintArrayInterface.h"
#include "Outliner.hh"
#include "rose.h" //needed for ROSE_ASSERT

using namespace std;


//! Generate a symbol set from an initialized name list                                                                                                                                 
static void convert (const SgInitializedNamePtrList input, ASTtools::VarSymSet_t& output)
{
  for (SgInitializedNamePtrList::const_iterator iter =  input.begin(); iter != input.end(); iter++)
    {
      const SgInitializedName * iname = *iter;
      SgSymbol* tmp_sym =  iname->get_symbol_from_symbol_table ();
      ROSE_ASSERT(tmp_sym);
      SgVariableSymbol* symbol = isSgVariableSymbol(tmp_sym);
      ROSE_ASSERT (symbol != NULL);
      output.insert(symbol);
    }
}


SgExpression* 
CudaMemoryManagement::getSizeExpressionOfDynamicArray(SgStatement* target, 
						      SgInitializedName* var_name,
						      MintSymSizesMap_t& trfSizes)
{
  SgGlobal* g_scope = SageInterface::getGlobalScope(target);
  SgExpression* sizeExp ;
  SgVariableSymbol* sym = isSgVariableSymbol(var_name->get_symbol_from_symbol_table ());	
  ROSE_ASSERT(sym);

  MintSymSizesMap_t::iterator it_sym;

  it_sym=  trfSizes.find(sym);

  if(it_sym == trfSizes.end())
    {
      cerr << "  ERR:Mint: getSizeExpressionOfDynamicArray: cannot find the symbol !!!" <<  endl;
      cerr << "  Did you forget to insert copy pragma? " <<  endl;
      ROSE_ABORT();
    }
  else //found the symbol in the transfer size list 
    {
      //step 1: check the size of the string vector
      //if size == 0
      // something is wrong
      //if size == 1 
      // create an expression for the size 
      // and return it
      //if size > 1 
      // create a function that uses predicates to determine the maximum size 
      // function should return an expression which is the maximum of the sizes

      std::vector < string>& sizeStrings = it_sym->second ;
      
      if(sizeStrings.size() == 0 )
	{
	  cerr << "getSizeExpressionOfDynamicArray: no size is defined !!!" <<  endl;
	  ROSE_ABORT();	  
	}
      else if (sizeStrings.size() == 1)
	{
	  string sizeStr = *sizeStrings.begin() ;

	  SgExpression* var_exp = buildVarRefExp(sym);

	  SgExpression* ref1 = buildPntrArrRefExp(var_exp, buildIntVal(1)); //E[1]
	  SgExpression* ref2 = buildPntrArrRefExp(var_exp, buildIntVal(0)); //E[0]

	  //second dimension size: &E[1] - &E[0] gives the number of columns
	  SgExpression* secondDimSize = buildSubtractOp(buildAddressOfOp(ref1), buildAddressOfOp(ref2)); 

	  sizeExp = buildMultiplyOp(buildVarRefExp (sizeStr , g_scope), secondDimSize);

	}
       else 
	{
	  //1.create a size variable to store the size of the array
	  //2. intialize it to the first size value in the list 
	  //3. add if statements to find the max value of the size 
	  //4. return the max as the sizeExp

	  SgBasicBlock * bb1 = SageBuilder::buildBasicBlock();
	 
	  SgExpression* var_exp = buildVarRefExp(sym);
	  string var_name_str= var_name->get_name().str();
	  var_name_str = "size_" + var_name_str;

	  std::vector<string>::iterator sstr= sizeStrings.begin(); 


	  SgVariableDeclaration* size_var_decl = buildVariableDeclaration(var_name_str ,buildIntType(), 
									  buildAssignInitializer(buildVarRefExp(*sstr,bb1)), g_scope);

	  SageInterface::insertStatementBefore(target, size_var_decl);
	  	  
	  sizeExp = buildVarRefExp(size_var_decl);
	  
	  sstr++;
	  for(; sstr != sizeStrings.end(); sstr++){
	    
	    SgExpression* curSize = buildVarRefExp(*sstr, bb1);

	    SgStatement* if_cond_stmt = buildExprStatement(buildGreaterThanOp(curSize, sizeExp));
	    
	    SgStatement* true_body = buildAssignStatement(sizeExp, curSize);
							  
	    SgIfStmt* if_stmt= buildIfStmt(if_cond_stmt, true_body, NULL);
	    
	    appendStatement( if_stmt, bb1);
	    }
		   
	  SageInterface::insertStatementBefore(target, bb1);

	  SgExpression* ref1 = buildPntrArrRefExp(var_exp, buildIntVal(1)); //E[1]
	  SgExpression* ref2 = buildPntrArrRefExp(var_exp, buildIntVal(0)); //E[0]

	  //second dimension size: &E[1] - &E[0] gives the number of columns
	  SgExpression* secondDimSize = buildSubtractOp(buildAddressOfOp(ref1), buildAddressOfOp(ref2)); 

	  sizeExp = buildMultiplyOp(sizeExp, secondDimSize);
	  
	  }
    }
  return sizeExp; 
}

SgVariableDeclaration* CudaMemoryManagement::issueTransferForArrayStruct(SgStatement* target, string wrapper_name)
{
  //1.step create a dev pointer with the struct type
  //2.insert that pointer into the code
  //3.Malloc space for the struct on the device 
  //4.Memcpy from host to device 
  //5.Free the struct at the end of the parallel region

  //step 1.

  SgScopeStatement* scope = target->get_scope();
  ROSE_ASSERT(scope);

  SgVariableSymbol *host_sym = scope->lookup_var_symbol(wrapper_name);
  ROSE_ASSERT(host_sym);

  string dev_struct_name = "dev" + wrapper_name ;

  SgType* struct_type = host_sym -> get_type();
  ROSE_ASSERT(struct_type);
  
  SgVariableDeclaration* dev_struct = buildVariableDeclaration(dev_struct_name, buildPointerType(struct_type), NULL, scope);

  //step 2.
  SageInterface::insertStatementBefore(target, dev_struct);

  //step 3.
  SgVariableSymbol *dev_sym = getFirstVarSym(dev_struct);
  ROSE_ASSERT(host_sym);

  SgStatement* cuda_malloc_func = generateCudaMallocCall(dev_sym, buildIntVal(1));
  SageInterface::insertStatementBefore(target, cuda_malloc_func);

  //step 4.
  SgExpression* trfKind = buildVarRefExp("cudaMemcpyHostToDevice", scope);				  
  SgStatement *memcpy_func= generateCudaMemCpyCallToDevice(dev_sym, host_sym, buildIntVal(1), trfKind );
  
  SageInterface::insertStatementBefore(target, memcpy_func);

  //step 5.

  SgStatement *cudaFree_stmt = generateCudaFreeCall(dev_sym);
  SageInterface::insertStatementAfter(target, cudaFree_stmt);

  return dev_struct;
}

void CudaMemoryManagement::issueDataTransferToDevice(SgNode* node, 
						     SgVariableSymbol* src_sym, 
						     SgInitializedName* dest, 
						     SgExpression* numElements)
				
{
  SgStatement* target = isSgStatement(node);

  SgScopeStatement* g_scope = src_sym->get_scope();
  
  string var_name = dest->get_name(); 
  SgType* new_type = dest->get_type();

  SgVariableDeclaration *var_decl =  buildVariableDeclaration(var_name, new_type, NULL, g_scope);
 
  SgVariableSymbol* var_sym = getFirstVarSym(var_decl);
  
  SageInterface::insertStatementBefore(target, var_decl);

//2.step: allocate memory on the gpu for each device pointer created
//cudaError_t cudaMalloc(void** devPtr, size_t size);
//cudaMalloc((void**) &d_a, sizeof(element type ) * N);

  SgStatement* cudamalloc_func = generateCudaMallocCall(var_sym, numElements);

  SageInterface::insertStatementBefore(target, cudamalloc_func);

//3.step: perform memory copy from host to device like this one:
//cudaError_t cudaMemcpy(void* dst, const void* src, size_t, enum cudaMemcpyKind kind);
							    	
  SgExpression* trfKind = buildVarRefExp("cudaMemcpyHostToDevice", g_scope);						   
  
  SgStatement *memcpy_func= generateCudaMemCpyCallToDevice(var_sym, src_sym, numElements, trfKind );
  
  SageInterface::insertStatementBefore(target, memcpy_func);
  
  removeStatement(target);

}

SgStatement* CudaMemoryManagement::generateCudaMallocCall(SgVariableSymbol* sym, SgExpression* numElements)
{
  //cudaMalloc((void**) &d_a, sizeof(element type ) * N);

  SgType* base_type = sym->get_type()->findBaseType();

  SgScopeStatement* g_scope = sym->get_scope();

  SgExpression* var_exp = buildVarRefExp(sym);
  
  SgExpression* dev_ptr = buildCastExp( buildAddressOfOp(var_exp), 
					buildPointerType(buildPointerType(buildVoidType())), SgCastExp::e_C_style_cast);
  
  
  //sizeof(type) * N
  SgExpression* size = buildMultiplyOp(numElements ,buildSizeOfOp(base_type));
  
  SgExprListExp* parameters1 = buildExprListExp(dev_ptr, size);
  
  SgStatement * cudamalloc_func = buildFunctionCallStmt("cudaMalloc", buildVoidType(), parameters1, g_scope);
 
  ROSE_ASSERT(cudamalloc_func);

  return cudamalloc_func;
}

SgStatement* CudaMemoryManagement::generateCudaMemCpyCallToDevice(SgVariableSymbol* dest_sym, 
								  SgVariableSymbol* src_sym, 
								  SgExpression* numElements,
								  SgExpression* trfKind)
{
//cudaError_t cudaMemcpy(void* dst, const void* src, size_t, enum cudaMemcpyKind kind);
  
  SgExpression* dest = buildVarRefExp(dest_sym);
  SgExpression* src  = buildVarRefExp(src_sym); 
  
  SgType* base_type = dest_sym->get_type()->findBaseType();
  SgExpression* size = buildMultiplyOp(numElements ,buildSizeOfOp(base_type));
  SgExprListExp* parameters2 = buildExprListExp(dest, buildAddressOfOp(src), size , trfKind);
  
  SgScopeStatement* scope = dest_sym ->get_scope();

  SgStatement *memcpy_func= buildFunctionCallStmt("cudaMemcpy", buildVoidType(), parameters2, scope);
  
  ROSE_ASSERT(memcpy_func != NULL);

  return memcpy_func;

}


SgStatement* CudaMemoryManagement::generateCudaMemCpyCallFromDevice(SgVariableSymbol* dest_sym, 
								  SgVariableSymbol* src_sym, 
								  SgExpression* numElements,
								  SgExpression* trfKind)
{
//cudaError_t cudaMemcpy(void* dst, const void* src, size_t, enum cudaMemcpyKind kind);
  
  SgExpression* dest = buildVarRefExp(dest_sym);
  SgExpression* src  = buildVarRefExp(src_sym); 
  
  SgType* base_type = dest_sym->get_type()->findBaseType();
  SgExpression* size = buildMultiplyOp(numElements ,buildSizeOfOp(base_type));
  SgExprListExp* parameters2 = buildExprListExp(buildAddressOfOp(dest), src, size , trfKind);
  
  SgScopeStatement* scope = dest_sym ->get_scope();

  SgStatement *memcpy_func= buildFunctionCallStmt("cudaMemcpy", buildVoidType(), parameters2, scope);
  
  ROSE_ASSERT(memcpy_func != NULL);

  return memcpy_func;

}


SgStatement* CudaMemoryManagement::generateCudaFreeCall(SgVariableSymbol* sym)
{
  SgScopeStatement* scope = sym ->get_scope();

  SgExprListExp* parameters4 = buildExprListExp(buildVarRefExp(sym));
  
  SgStatement *memfree_func= buildFunctionCallStmt("cudaFree", buildVoidType(), parameters4, scope);
  
  ROSE_ASSERT(memfree_func != NULL);

  return memfree_func;
}

//Cuda memory management class
//transfers, allocates and declares variables that are shared with the host and gpu
void CudaMemoryManagement::generateCudaHostVar(ASTtools::VarSymSet_t& deviceSyms, 
					       MintSymSizesMap_t& trfSizes,
					       SgNode* node, 
					       MintHostSymToDevInitMap_t& hostToDevVars)
{
//1. need to create new gpu device pointers for host arrays 
//2. then malloc them on the gpu
//3. perform memcopy from host to device
//4. find which ones are modified, copy them back to the host 
//5. free all the pointers with cudafree

//TODO
//1. Need to handle pointers. If the shared array is a pointer and dynamically allocated
//arraysize fails. 
//2. Copy back the modified variables

  ROSE_ASSERT(node != NULL);
  SgOmpClauseBodyStatement* target = isSgOmpClauseBodyStatement(node);
  ROSE_ASSERT (target != NULL);
  
  //SgGlobal* g_scope = SageInterface::getGlobalScope(target);
  SgScopeStatement* g_scope = target->get_scope();

  for (ASTtools::VarSymSet_t::iterator orig_sym = deviceSyms.begin (); orig_sym!= deviceSyms.end (); ++orig_sym)
    {

//1.step: create a device pointer for every host array that is shared 
      SgVariableSymbol* sym = const_cast<SgVariableSymbol*> (*orig_sym);

      SgInitializedName* i_name = (*orig_sym)->get_declaration ();
      
      ROSE_ASSERT (i_name);
      SgType* var_type = i_name->get_type() ;
      string var_name= i_name->get_name().str();
      
      if(hostToDevVars.find(sym) != hostToDevVars.end())
	{//this variables was part of pragma copy
	  //no need to transfer it
	  
	  MintHostSymToDevInitMap_t::iterator it = hostToDevVars.find(sym);
	 
	  SgInitializedName* iname = it -> second;
	  ROSE_ASSERT(iname);

	  //cudaFree(dUold.ptr);
	  //cudaFree(dUnew.ptr);

	  //SgVariableSymbol* var_sym =g_scope -> lookup_var_symbol(i_name->get_name()); 
	  //ROSE_ASSERT(var_sym);

	  SgScopeStatement* i_scope = iname->get_scope();
	  ROSE_ASSERT(i_scope);

	  SgExpression* var_ptr = buildDotExp(buildVarRefExp(iname), buildVarRefExp("ptr", i_scope));

	  SgExprListExp* parameters4 = buildExprListExp(var_ptr);
					      
	  SgStatement *memfree_func= buildFunctionCallStmt("cudaFree", buildVoidType(), parameters4, i_scope);
  
	  ROSE_ASSERT(memfree_func != NULL);

	  SgStatement* lastStmt = getLastStatement(i_scope);

	  if(!isSgReturnStmt(lastStmt)){
	    //SageInterface::insertStateEmentAfter(lastStmt, memfree_func);
	    appendStatement(memfree_func, i_scope);
	  }
	  else
	    SageInterface::insertStatementBefore(lastStmt, memfree_func);
	  
	}
      else
	{
	  //has to be not scalar
	  if(isSgArrayType (var_type) || isSgPointerType(var_type)){
	    
	    var_name ="d_" + var_name;	
	    SgType* base_type = var_type->findBaseType();
	    SgType* new_type = buildPointerType(base_type);
	    
	    SgVariableDeclaration *var_decl = buildVariableDeclaration(var_name, new_type, NULL, g_scope);
	    SgVariableSymbol* var_sym = getFirstVarSym(var_decl);
	    
	    SageInterface::insertStatementBefore(target, var_decl);
	    
	    //add this symbol to the device symbol list
	    //deviceSyms.insert(g_scope -> lookup_var_symbol(var_name)); 
	    
	    //2.step: allocate memory on the gpu for each device pointer created
	    //cudaError_t cudaMalloc(void** devPtr, size_t size);
	    //cudaMalloc((void**) &d_a, sizeof(element type ) * N);
	    
	    SgExpression* numElements;
	    
	    if(isSgArrayType(var_type)){
	      
	      size_t arraySize= getArrayElementCount(isSgArrayType(var_type)); 
	      std::ostringstream stm;
	      stm << arraySize ;
	      
	      numElements = buildVarRefExp (stm.str());
	    }
	    else{ 
	      numElements = getSizeExpressionOfDynamicArray(target, i_name, trfSizes) ;
	    }
	    
	    //cudaError_t cudaMalloc(void** devPtr, size_t size);
	    SgStatement* cudamalloc_func = generateCudaMallocCall(var_sym, numElements);
	    
	    SageInterface::insertStatementBefore(target, cudamalloc_func);
	    
	    //3.step: perform memory copy from host to device like this one:
	    //cudaError_t cudaMemcpy(void* dst, const void* src, size_t, enum cudaMemcpyKind kind);
	    
	    SgExpression* trfKind = buildVarRefExp("cudaMemcpyHostToDevice", g_scope);						   
	    
	    SgStatement *memcpy_func= generateCudaMemCpyCallToDevice(var_sym, sym, numElements, trfKind );
	    
	    SageInterface::insertStatementBefore(target, memcpy_func);
	    
	    //5.step: free the memory on the device
	    //cudaError_t cudaFree(void** dev_ptr)
	    
	    SgStatement* memfree_func = generateCudaFreeCall(var_sym);
	    
	    SageInterface::insertStatementAfter(target, memfree_func);
	    
	    //4.step: perform memory copy from device to host, copy back only modified data
	    //cudaError_t cudaMemcpy(void* dst, const void* src, size_t, enum cudaMemcpyKind kind);
	    
	    //TODO: copy back only modified data
	    
	    trfKind = buildVarRefExp("cudaMemcpyDeviceToHost", g_scope);						    	
	    SgStatement *memcpy_func2= generateCudaMemCpyCallFromDevice(sym, var_sym, numElements, trfKind);
	    
	    SageInterface::insertStatementAfter(target, memcpy_func2);
	  }
	  else 
	    {//shared scalar arguments 
	      //Do nothing
	    }
	}
   } 
}

void CudaMemoryManagement::freeParallelRegionVars(SgStatement* last_stmt, 
					     MintHostSymToDevInitMap_t& hostToDevVars)
{
  if(last_stmt == NULL)
    return;

  for(MintHostSymToDevInitMap_t::iterator it = hostToDevVars.begin(); it!= hostToDevVars.end(); it++)
  {
//    SgVariableSymbol* host_sym = it->first; 
    SgInitializedName* dev_name = it->second; 
    ROSE_ASSERT(dev_name);

    SgScopeStatement* i_scope = dev_name->get_scope();
    ROSE_ASSERT(i_scope);
    
    SgExpression* var_ptr = buildDotExp(buildVarRefExp(dev_name), buildVarRefExp("ptr", i_scope));
    ROSE_ASSERT(var_ptr);

    SgExprListExp* parameters4 = buildExprListExp(var_ptr);
    
    SgStatement *memfree_func= buildFunctionCallStmt("cudaFree", buildVoidType(), parameters4, i_scope);
    
    ROSE_ASSERT(memfree_func != NULL);
    
    SageInterface::insertStatementBefore(last_stmt, memfree_func);
  }
}


/*************************************************************************************************/
// Generate variables on the host that are needed on the gpu
// Determine which variables are device variables, which ones are shared, which ones are private ...
/*************************************************************************************************/
void CudaMemoryManagement::handleCudaHostVars(SgNode* node, //std::string& wrapper_name,
					      ASTtools::VarSymSet_t& deviceSyms, 
					      std::set<SgInitializedName*>& writeVars,
					      MintSymSizesMap_t& trfSizes,
					      MintHostSymToDevInitMap_t& hostToDevVars)

{
  ROSE_ASSERT(node != NULL);
  SgOmpClauseBodyStatement* target = isSgOmpClauseBodyStatement(node);
  ROSE_ASSERT (target != NULL);

  SgStatement * body =  target->get_body();
  ROSE_ASSERT(body != NULL);
  //SgFunctionDeclaration* result= NULL;

  SgBasicBlock* body_block = Outliner::preprocess(body);
 
  // Variable handling is done after Outliner::preprocess() to ensure a basic block for the body, 
  // but before calling the actual outlining                                                                     
  // This simplifies the outlining since firstprivate, private variables are replaced                     
  // with their local copies before outliner is used
  // Didem: do I need this?
  transOmpVariables (target, body_block);

  ASTtools::VarSymSet_t fp_Syms, p_Syms;

  // This step is less useful for private, firstprivate, and reduction variables   
  // since they are already handled by transOmpVariables().                        
  // Computes the set of variables in 'body_block' that need to be                                                                          
  // passed to the outlined routine == shared variables in OpenMP: sharedSyms                       
  // and private variables (in OpenMP): privateSyms                         

  //ASTtools::VarSymSet_t sharedSyms;  
  //Outliner::collectVars(body_block, sharedSyms, privateSyms);

  CudaMemoryManagement::generateCudaHostVar(deviceSyms, trfSizes, node, hostToDevVars);

  //std::set<SgInitializedName*> readOnlyVars2;

  //SageInterface::collectReadOnlyVariables(body_block,readOnlyVars2);                                     
      
  // We choose to be conservative about the variables needing pointer dereferencing first   
  // AllParameters - readOnlyVars  - private -firstprivate                                          
  // Union ASTtools::collectPointerDereferencingVarSyms(body_block, pdSyms)                          
  // Assume all parameters need to be passed by pointers first                                                                   

  //exclude firstprivate variables: they are read only in fact                                                                   


  SgInitializedNamePtrList fp_vars = collectClauseVariables (target, V_SgOmpFirstprivateClause);
  ASTtools::VarSymSet_t fp_syms, pdSyms2;
  convert (fp_vars, fp_syms);

  /*
  set_difference (pdSyms.begin(), pdSyms.end(),
		  fp_syms.begin(), fp_syms.end(),
		  std::inserter(pdSyms2, pdSyms2.begin()));
  */
  //pdSyms : variables that use pointer types
  // Similarly , exclude private variable, also read only                                                                                
  SgInitializedNamePtrList p_vars = collectClauseVariables (target, V_SgOmpPrivateClause);
  ASTtools::VarSymSet_t p_syms; //, pdSyms3;
  convert (p_vars, p_Syms);

  /*set_difference (pdSyms2.begin(), pdSyms2.end(),
		  p_syms.begin(), p_syms.end(),
		  std::inserter(pdSyms3, pdSyms3.begin()));
  */
  // lastprivate and reduction variables cannot be excluded  since write access to their shared copies                                


  //cout << "Shared Var     :"<< ASTtools::toString(sharedSyms) << endl;
  //cout << "Device Var     :"<< ASTtools::toString(deviceSyms) << endl;
  //cout << "pdSyms3 Var    :"<< ASTtools::toString(pdSyms3) << endl;
  //cout << "privateSyms Var:"<< ASTtools::toString(privateSyms) << endl;


}




/*************************************************************************************************/
// replaces host variables references with device variable references in the parallel region 
// if the variable is referenced through pointer swapping than it is ok to replace 
// In other cases, we may need to copy back the data to the host 
/*************************************************************************************************/
void CudaMemoryManagement::postTransformationVariableHandling(SgNode* node,
							      ASTtools::VarSymSet_t& deviceSyms, 
							      MintHostSymToDevInitMap_t& hostToDevVars)
  
{
  //node is the parallel region 
  //replace all the host variable references with their device variable references 
  ROSE_ASSERT(node != NULL);
  
  SgOmpParallelStatement* targetStatement = isSgOmpParallelStatement(node);
  ROSE_ASSERT (targetStatement != NULL);

  SgBasicBlock * target_body =  isSgBasicBlock(targetStatement->get_body());

  Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(target_body, V_SgAssignOp);

  Rose_STL_Container<SgNode*>::iterator it;

  for(it = varList.begin(); it != varList.end(); it++) 
    {
      //convert the following statement into the second statement
      //Uold = Unew 
      //dev_1_Uold.ptr = ((double *)(dev_2_Unew.ptr));

      SgAssignOp* as_op = isSgAssignOp(*it);

      ROSE_ASSERT(as_op);

      SgExpression* lhs = as_op -> get_lhs_operand();
      ROSE_ASSERT(lhs);
      
      SgExpression* rhs = as_op -> get_rhs_operand();
      ROSE_ASSERT(rhs);

      if(isSgFunctionCallExp(rhs))
	{
	  SgFunctionCallExp* func_exp = isSgFunctionCallExp(rhs);
	  string name = func_exp->unparseToString();

	  if (name.find( "make_cudaPitchedPtr" ) != string::npos )
	    continue;
	}

      replaceHostRefsWithDeviceRefs(isSgNode(rhs), deviceSyms, hostToDevVars, false); //perform type casting (don't, changed my mind)
      replaceHostRefsWithDeviceRefs(isSgNode(lhs), deviceSyms, hostToDevVars, false); //don't perform type casting
      
    }      

  //make a final check
  //replaceHostRefsWithDeviceRefs(isSgNode(target_body), deviceSyms, hostToDevVars, false);

}

void CudaMemoryManagement::replaceHostRefsWithDeviceRefs(SgNode* node,
							 ASTtools::VarSymSet_t& deviceSyms, 
							 MintHostSymToDevInitMap_t& hostToDevVars,
							 bool typeCast)
  
{

  ROSE_ASSERT(node);

  SgStatement* stmt = getEnclosingStatement(node);
  ROSE_ASSERT(stmt);

  SgScopeStatement* g_scope = stmt->get_scope();
  ROSE_ASSERT(g_scope);

  Rose_STL_Container<SgNode*> varList = NodeQuery::querySubTree(node, V_SgVarRefExp);

  Rose_STL_Container<SgNode*>::iterator it;

  for(it = varList.begin(); it != varList.end(); it++) 
    {
      SgVarRefExp* var = isSgVarRefExp(*it);
      
      SgVariableSymbol* host_sym = var->get_symbol();
      ROSE_ASSERT(host_sym);

      SgType* type = var->get_type();

      string var_name = host_sym->get_name().str();

      //We need to replace the array reference with its device pointer
      //if(isSgArrayType(type) || 
      if( isSgPointerType(type))
	{	
	  //if this variable is in the deviceSyms list, then it has
	  //the original host name because deviceSyms list contains
	  //the original host names. 
	  
	  ASTtools::VarSymSet_t::iterator found = find(deviceSyms.begin(), deviceSyms.end(), host_sym );

	  if(found != deviceSyms.end())
	    {
	      if(hostToDevVars.find(host_sym) != hostToDevVars.end())
		{
		  //this variable is created thru copy pragma
		  MintHostSymToDevInitMap_t::iterator it = hostToDevVars.find(host_sym);
		  
		  SgInitializedName* dev_name = it -> second;
		  ROSE_ASSERT(dev_name);
		  
		  //get variable symbol from the init name
		  SgVariableSymbol* dev_sym = isSgVariableSymbol(dev_name->get_symbol_from_symbol_table ());
		  
		  ROSE_ASSERT(dev_sym);
		  
		  SgExpression* dev_ptr = buildDotExp(buildVarRefExp(dev_sym), buildVarRefExp("ptr", g_scope));
		  ROSE_ASSERT(dev_ptr);
		  
		  if(typeCast){
		    SgType* type = host_sym->get_type()->findBaseType();
		    dev_ptr = buildCastExp( dev_ptr, buildPointerType(type), SgCastExp::e_C_style_cast);
		  }
		  
		  replaceExpression(isSgExpression(var), dev_ptr);
		}
	      else{
		cout << "  ERROR:Mint: @ Line " << isSgNode(var)->get_file_info()->get_line()  << endl;
		cout << "  The variable "<< var_name << " is not transfered to the device memory but used on the device!" << endl; 
		cout << "  Mint doesn't know what to do with it. Please double-check your data transfers. Terminating... " << endl; 
		ROSE_ABORT();		    
	      }
	    }//end of found symbol
	  else 
	    {
	      //this varibale is not used on the device but referenced in the parallel region
	      string dev_ptr = "dev_" + var_name; 
	      SgVariableSymbol* dev_sym = g_scope -> lookup_var_symbol(dev_ptr);
	      //we haven't declared a variable for this host variable
	      if(dev_sym == NULL)
		{
		  SgVariableDeclaration* tmp = buildVariableDeclaration("dev_" + var_name, buildPointerType(buildVoidType()), NULL, g_scope);
		  ROSE_ASSERT(tmp);
		  insertStatementBefore(stmt, tmp);
		  replaceExpression(isSgExpression(var), buildVarRefExp(tmp));		  
		}
	      else 
		{
		  replaceExpression(isSgExpression(var), buildVarRefExp(dev_sym));		  
		}
	    }
	}//end of array or pointer ref
    }//end of varlist 

}//end of function
	    


/*************************************************************************************************/
// replaces host variables references with device variable references in the parallel region 
// if the variable is referenced through pointer swapping than it is ok to replace 
// In other cases, we may need to copy back the data to the host 
/*************************************************************************************************/

SgFunctionCallExp* CudaMemoryManagement::generateCudaMallocCall(SgVariableDeclaration* dev_decl,
								SgVariableDeclaration* extent_decl,			
								SgScopeStatement* scope)
{
  //status = cudaMalloc3D (&dUold, dimArr);

  SgExpression* var_exp = buildAddressOfOp( buildVarRefExp(dev_decl));
  SgExpression* extent_exp =  buildVarRefExp(extent_decl);

  ROSE_ASSERT(var_exp);
  ROSE_ASSERT(extent_exp);
  
  SgExprListExp* parameters1 = buildExprListExp(var_exp, extent_exp);
  
  SgFunctionCallExp * cudamalloc_func = buildFunctionCallExp("cudaMalloc3D", 
							     buildVoidType(), parameters1, scope);
 
  ROSE_ASSERT(cudamalloc_func);

  return cudamalloc_func;
}


SgFunctionCallExp* CudaMemoryManagement::generateCudaMemCpyCall(SgVariableDeclaration* param_decl,
								SgScopeStatement* scope)
{
  //status=cudaMemcpy3D(&pOld);
  SgExpression* var_exp = buildAddressOfOp( buildVarRefExp(param_decl));

  ROSE_ASSERT(var_exp);
  
  SgExprListExp* parameters1 = buildExprListExp(var_exp);
  
  SgFunctionCallExp * cudaMemCpy_func = buildFunctionCallExp("cudaMemcpy3D", buildVoidType(), parameters1, scope);
 
  ROSE_ASSERT(cudaMemCpy_func);

  return cudaMemCpy_func;
}

SgStatement* CudaMemoryManagement::generateCheckCudaSuccessStatement(SgVarRefExp* stat,
								     SgScopeStatement* scope)
{
  //if(status != cudaSuccess){fprintf(stderr, "%s\n", cudaGetErrorString(status));}
  SgVarRefExp* cudaSuccess = buildVarRefExp("cudaSuccess", scope); 
  ROSE_ASSERT(cudaSuccess);

  SgExpression* condition = buildNotEqualOp(stat, cudaSuccess);
  ROSE_ASSERT(condition);  

  SgVarRefExp* stderrRef = buildVarRefExp("stderr", scope);

  //"%s\n"
  SgExpression* output =buildStringVal("%s\\n");
   
  ROSE_ASSERT(output);

  SgFunctionCallExp* cudaGetErr = buildFunctionCallExp("cudaGetErrorString", buildVoidType(), 
						       buildExprListExp(stat), scope);

  SgExprListExp* paramList = buildExprListExp(stderrRef, output, cudaGetErr);
  SgStatement* body = buildFunctionCallStmt("fprintf", buildVoidType() , paramList, scope);
  ROSE_ASSERT(body);  

  SgIfStmt* if_stmt = buildIfStmt( condition, body, NULL);   
  ROSE_ASSERT(if_stmt);

  return if_stmt;
}


void CudaMemoryManagement::generateCudaMemCpyToDeviceParams(SgStatement* target, 
							    SgVariableDeclaration* param_decl,
							    SgVarRefExp* dest_var,
							    SgVarRefExp* extent_var,			
							    SgInitializedName* src_name,
							    MintTrfParams_t params,
							    SgScopeStatement* scope)
{

  //pOld.srcPtr = make_cudaPitchedPtr((void*)(Uold[0]), (n+2)* sizeof(REAL), (n+2), (n+2));
  //pOld.dstPtr = dUold;
  //pOld.extent = dimArr;
  //pOld.kind   = cudaMemcpyHostToDevice;

  SgExpression* cpyParam = buildVarRefExp(param_decl);

  //pOld.extent = dimArr;
  SgStatement* extent = buildAssignStatement(buildDotExp(cpyParam, buildVarRefExp("extent", scope)),
					     extent_var);

  //pOld.kind   = cudaMemcpyHostToDevice;
  SgStatement* kind =  buildAssignStatement(buildDotExp(cpyParam, buildVarRefExp("kind", scope)), 
						     buildVarRefExp("cudaMemcpyHostToDevice", scope));
 
  //pOld.dstPtr = dUold;
  SgStatement* dstPtr = buildAssignStatement(buildDotExp(cpyParam, buildVarRefExp("dstPtr", scope)), 
					     dest_var);
  
  //pOld.srcPtr = make_cudaPitchedPtr((void*)(Uold[0]), (n+2)* sizeof(REAL), (n+2), (n+2));
  SgExprListExp* paramList=NULL;

  int dimArray = MintArrayInterface::getDimension(src_name);

  SgVarRefExp* srcExp = buildVarRefExp(src_name, scope);
  
  if(dimArray == 3)
    {
      SgPntrArrRefExp* srcPtrExp = buildPntrArrRefExp(srcExp, buildIntVal(0));
      srcPtrExp = buildPntrArrRefExp(srcPtrExp, buildIntVal(0));

      SgExpression* srcArr = buildCastExp( srcPtrExp, buildPointerType(buildVoidType()), SgCastExp::e_C_style_cast);

      ROSE_ASSERT(srcArr);

      SgExpression* width = buildVarRefExp(params.sizes[0], scope);
      SgExpression* pitch = buildMultiplyOp(width ,buildSizeOfOp(src_name->get_type()->findBaseType()));
      SgExpression* height = buildVarRefExp(params.sizes[1], scope);

      ROSE_ASSERT(width); ROSE_ASSERT(pitch); ROSE_ASSERT(height);
      paramList = buildExprListExp(srcArr, pitch, width , height);

    }
  else if (dimArray == 2)
    {
      SgPntrArrRefExp* srcPtrExp = buildPntrArrRefExp(srcExp, buildIntVal(0));
      SgExpression* srcArr = buildCastExp( srcPtrExp, buildPointerType(buildVoidType()), SgCastExp::e_C_style_cast);

      ROSE_ASSERT(srcArr);

      SgExpression* width = buildVarRefExp(params.sizes[0], scope);
      SgExpression* pitch = buildMultiplyOp(width ,buildSizeOfOp(src_name->get_type()->findBaseType()));
      SgExpression* height = buildVarRefExp(params.sizes[1], scope);

      ROSE_ASSERT(width); ROSE_ASSERT(pitch); ROSE_ASSERT(height);
      paramList = buildExprListExp(srcArr, pitch, width , height);

    }
  else if (dimArray == 1 ){
      SgExpression* srcArr = buildCastExp( srcExp, buildPointerType(buildVoidType()), SgCastExp::e_C_style_cast);
      ROSE_ASSERT(srcArr);

      SgExpression* width = buildVarRefExp(params.sizes[0], scope);
      SgExpression* pitch = buildMultiplyOp(width ,buildSizeOfOp(src_name->get_type()->findBaseType()));
      SgExpression* height = buildVarRefExp(params.sizes[1], scope);
      paramList = buildExprListExp(srcArr, pitch, width , height);
  }
 else
   {
     cout << "  ERROR:Mint: cannot find the dimension of the array. Type might be hidden. Exiting ..." << endl;
     ROSE_ABORT();
   }					     

  SgExpression *make_cudaPtr= buildFunctionCallExp("make_cudaPitchedPtr", buildVoidType(), paramList, scope);
  SgStatement* srcPtr =  buildAssignStatement(buildDotExp(cpyParam, buildVarRefExp("srcPtr", scope)),
					      make_cudaPtr);

  ROSE_ASSERT(srcPtr);

  insertStatementAfter(target, kind );
  insertStatementAfter(target, extent );
  insertStatementAfter(target, dstPtr );
  insertStatementAfter(target, srcPtr );

}


void CudaMemoryManagement::issueDataTransferToDevice(SgNode* node, 
						     const MintTrfParams_t params, 
						     MintHostSymToDevInitMap_t& hostToDevVars, 
						     bool inside)
{
  //issues data transfers based on the user pragmas 
  //user provides the necessary information about the variable mapping, their sizes and direction 
  //of the transfer

  SgStatement* target = isSgStatement(node);
  ROSE_ASSERT(target);

  SgScopeStatement* scope = target->get_scope();
  ROSE_ASSERT(scope);

  SgFunctionDeclaration* func= SageInterface::getEnclosingFunctionDeclaration(scope);
  ROSE_ASSERT(func);

  string dst_name = params.dst ; 
  string srcStr = params.src ;

  string targetStr = target->unparseToString();
  
  SageInterface::attachComment(target, "Mint: Replaced Pragma: " + targetStr);

  SgInitializedName* src = MintTools::findInitName(isSgNode(func), srcStr );

  if(src == NULL){
    //might be a global variable
    SgGlobal* g_scope = SageInterface::getGlobalScope(target);
    src = MintTools::findInitName(isSgNode(g_scope), srcStr );
    if(src == NULL){
      cout << "  ERROR:Mint: @ Line " << node->get_file_info()->get_line()  << endl;
      cout << "  The source variable "<<srcStr << " is undefined"<< endl;
      ROSE_ABORT();
    } 
  }

  SgVariableSymbol* src_sym = isSgVariableSymbol(src->get_symbol_from_symbol_table ());
  ROSE_ASSERT(src_sym);

  SgType* arrType = src_sym->get_type();
  ROSE_ASSERT(arrType); 

  SgType* baseType = arrType->findBaseType();
  ROSE_ASSERT(baseType);

  SgVariableDeclaration* extent_decl = NULL;
  SgVariableDeclaration* status_decl = NULL;
  SgVariableDeclaration* dev_decl  = NULL;

  SgStatement* last_stmt = isSgStatement(target);
  ROSE_ASSERT(last_stmt);

  if(!inside && hostToDevVars.find(src_sym) == hostToDevVars.end() ){
    last_stmt = declareAndAllocateDevicePtr(isSgStatement(target), dev_decl,
					    extent_decl, status_decl,
					    baseType, dst_name, params);
    ROSE_ASSERT(dev_decl);
    ROSE_ASSERT(extent_decl);
    ROSE_ASSERT(status_decl);
  }
  else 
    {
      //inside of a parallel region
      //we need get the values for dev_decl, extent_decl and status_decl
      //we didn't transfer this variable (write-only) but we still need to allocate space
      if(hostToDevVars.find(src_sym) != hostToDevVars.end())
	{
	  //this variables is copied outside of the parallel region 
	  //let's find its declaration
	  MintHostSymToDevInitMap_t::const_iterator it =hostToDevVars.find(src_sym);
	  SgInitializedName* dev_name = (*it).second; //this is its field name                                 
	  ROSE_ASSERT(dev_name);
	  dev_decl = isSgVariableDeclaration(dev_name-> get_declaration());
	  ROSE_ASSERT(dev_decl);

	  //because dst_name is different than the one original name (generated by MintPragma)
	  dst_name = dev_name ->get_name().str();

	  //get the extent declaration :ext_dst_name
	  SgInitializedName* ext_name = MintTools::findInitName(isSgNode(func), "ext_"+ dst_name );
	  ROSE_ASSERT(ext_name);
	  extent_decl = isSgVariableDeclaration(ext_name-> get_declaration());
	  ROSE_ASSERT(extent_decl);

	  //get the status declaration
	  SgInitializedName* stat_name = MintTools::findInitName(isSgNode(func), "stat_"+ dst_name );
	  ROSE_ASSERT(stat_name);
	  status_decl = isSgVariableDeclaration(stat_name-> get_declaration());
	  ROSE_ASSERT(status_decl);
	  
	}
      else
	{
	  //this variable is not copied outside of the parallel region
	  //should we allow this? 
	  
	  //We need to find where to insert the variable declaration in the kernel
	  //When there is no data transfer to the device, then we need to 
	  //find a location to declare and allocate the variable
	  //that location is right before the parallel region in which the copy pragma is
	  SgOmpParallelStatement* parallel_reg= MintTools::getEnclosingParallelRegion(scope);
	  ROSE_ASSERT(parallel_reg);
	  
	  SgStatement* stmt = isSgStatement(parallel_reg);
	  ROSE_ASSERT(stmt);
	  //we don't use this last_stmt because we do the copy inside the parallel region
	  //but declare and allocate it outside the parallel region
//	  SgStatement* last_stmt = declareAndAllocateDevicePtr(stmt, dev_decl,
//						  extent_decl, status_decl,
//						  baseType, dst_name, params);
	  
	  ROSE_ASSERT(dev_decl);      
	  ROSE_ASSERT(status_decl);      
	  ROSE_ASSERT(extent_decl);
	  
	  hostToDevVars[src_sym] = dev_decl->get_decl_item(dst_name);
	}
    }// end of inside 

  //copy host to device                                                                                          
  //cudaMemcpy3DParms pOld                 //pitch (fake pitch), w, h                                                      
  //pOld.srcPtr = make_cudaPitchedPtr((void*)(Uold[0]), (n+2)* sizeof(REAL), (n+2), (n+2));
  //pOld.dstPtr = dUold;
  //pOld.extent = dimArr;
  //pOld.kind   = cudaMemcpyHostToDevice;
  //status=cudaMemcpy3D(&pOld);
  //if(status != cudaSuccess){fprintf(stderr, "MemcpyHtD: %s\n", cudaGetErrorString(status));}

  SgType* param3D_type = buildOpaqueType("cudaMemcpy3DParms", scope); 
  ROSE_ASSERT(param3D_type);
  SgAssignInitializer* initToEmpty = buildAssignInitializer(buildVarRefExp("{0}", scope));

  string param_name = MintTools::generateCopyParamName(target);
  SgVariableDeclaration *param_decl = buildVariableDeclaration(param_name + dst_name, param3D_type, initToEmpty, scope) ;

  SageInterface::insertStatementAfter(last_stmt, param_decl);
  SageInterface::attachComment(param_decl, "Mint: Copy host to device");

  SgFunctionCallExp* cudaMemCpy3D = generateCudaMemCpyCall(param_decl, scope);
  SgStatement* memcpy = buildAssignStatement(buildVarRefExp(status_decl), cudaMemCpy3D);

  insertStatementAfter(param_decl, memcpy );
 
  SgStatement* status = generateCheckCudaSuccessStatement(buildVarRefExp(status_decl), scope);
  insertStatementAfter(memcpy, status );

  generateCudaMemCpyToDeviceParams(isSgStatement(param_decl), param_decl, 
				   buildVarRefExp(dev_decl),  buildVarRefExp(extent_decl), src, params, scope);

  //add this host-dev variable pair into the map 
  SgInitializedName* devVar_name = dev_decl->get_decl_item(dst_name);
  ROSE_ASSERT(devVar_name);
  SgVariableSymbol* new_sym = new SgVariableSymbol (devVar_name);                                         
  ROSE_ASSERT(new_sym);                                                                               
                                       
  scope->insert_symbol (devVar_name->get_name(), new_sym);                                                                                               
  hostToDevVars[src_sym] = devVar_name;      

  //remove the pragma statement
  removeStatement(target);

}

void CudaMemoryManagement::generateCudaMemCpyFromDeviceParams(SgStatement* target, 
							      SgVariableDeclaration* param_decl,
							      SgVarRefExp* src_var,SgVarRefExp* extent_var,SgInitializedName* dest,
							      MintTrfParams_t params,
							      SgScopeStatement* scope)
{

  //qUold.srcPtr = dUold;
  //qUold.dstPtr = make_cudaPitchedPtr((void*)Uold[0], (n+2)*sizeof(REAL), (n+2), (n+2));
  //qUold.extent = dimArr;
  //qUold.kind = cudaMemcpyDeviceToHost;

  SgExpression* cpyParam = buildVarRefExp(param_decl);

  //pOld.extent = dimArr;
  SgStatement* extent = buildAssignStatement(buildDotExp(cpyParam, buildVarRefExp("extent", scope)),
					     extent_var);

  //pOld.kind   = cudaMemcpyDeviceToHost;
  SgStatement* kind =  buildAssignStatement(buildDotExp(cpyParam, buildVarRefExp("kind", scope)), 
						     buildVarRefExp("cudaMemcpyDeviceToHost", scope));
 
  //pOld.srcPtr = dUold;
  SgStatement* srcPtr = buildAssignStatement(buildDotExp(cpyParam, buildVarRefExp("srcPtr", scope)), 
					     src_var);
  
  //pOld.dstPtr = make_cudaPitchedPtr((void*)(Uold[0]), (n+2)* sizeof(REAL), (n+2), (n+2));
  SgExprListExp* paramList=NULL;

  int dimArray = MintArrayInterface::getDimension(dest);

  SgVarRefExp* destExp = buildVarRefExp(dest, scope);
  
  if(dimArray == 3)
    {
      SgPntrArrRefExp* destPtrExp = buildPntrArrRefExp(destExp, buildIntVal(0));
      destPtrExp = buildPntrArrRefExp(destPtrExp, buildIntVal(0));

      SgExpression* destArr = buildCastExp( destPtrExp, buildPointerType(buildVoidType()), SgCastExp::e_C_style_cast);

      ROSE_ASSERT(destArr);

      SgExpression* width = buildVarRefExp(params.sizes[0], scope);
      SgExpression* pitch = buildMultiplyOp(width ,buildSizeOfOp(dest->get_type()->findBaseType()));
      SgExpression* height = buildVarRefExp(params.sizes[1], scope);

      paramList = buildExprListExp(destArr, pitch, width , height);

    }
  else if (dimArray == 2)
    {
      SgPntrArrRefExp* destPtrExp = buildPntrArrRefExp(destExp, buildIntVal(0));
      SgExpression* destArr = buildCastExp( destPtrExp, buildPointerType(buildVoidType()), SgCastExp::e_C_style_cast);

      ROSE_ASSERT(destArr);

      SgExpression* width = buildVarRefExp(params.sizes[0], scope);
      SgExpression* pitch = buildMultiplyOp(width ,buildSizeOfOp(dest->get_type()->findBaseType()));
      SgExpression* height = buildVarRefExp(params.sizes[1], scope);

      paramList = buildExprListExp(destArr, pitch, width , height);

    }
  else {
      SgExpression* destArr = buildCastExp( destExp, buildPointerType(buildVoidType()), SgCastExp::e_C_style_cast);
      ROSE_ASSERT(destArr);

      SgExpression* width = buildVarRefExp(params.sizes[0], scope);
      SgExpression* pitch = buildMultiplyOp(width ,buildSizeOfOp(dest->get_type()->findBaseType()));
      SgExpression* height = buildVarRefExp(params.sizes[1], scope);
      paramList = buildExprListExp(destArr, pitch, width , height);
  }

					     
  SgExpression *make_cudaPtr= buildFunctionCallExp("make_cudaPitchedPtr", buildVoidType(), paramList, scope);
  SgStatement* dstPtr =  buildAssignStatement(buildDotExp(cpyParam, buildVarRefExp("dstPtr", scope)),
					       make_cudaPtr);

  ROSE_ASSERT(dstPtr);

  insertStatementBefore(target, srcPtr );
  insertStatementBefore(target, dstPtr );
  insertStatementBefore(target, extent );
  insertStatementBefore(target, kind );
}

SgStatement* CudaMemoryManagement::declareAndAllocateDevicePtr(SgStatement* target,
							       SgVariableDeclaration*& dev_decl,
							       SgVariableDeclaration*& extent_decl,
							       SgVariableDeclaration*& status_decl,									 
							       SgType* baseType,
							       string ptr_name,
							       const MintTrfParams_t params)
{
  /*
    cudaError_t stat_dev_1_Uold;
    cudaExtent ext_dev_1_Uold = make_cudaExtent((n+2) * sizeof(double ),(n+2),1);    
    cudaPitchedPtr dev_1_Uold;
    stat_dev_1_Uold = cudaMalloc3D(&dev_1_Uold,ext_dev_1_Uold);
    if (stat_dev_1_Uold != cudaSuccess) 
    fprintf(stderr,"%s\n",cudaGetErrorString(stat_dev_1_Uold));
  */
  
  SgScopeStatement* scope = target->get_scope();

  //  cudaError_t status ;
  SgType* err_type = buildOpaqueType("cudaError_t", scope);
  ROSE_ASSERT(err_type);
  status_decl =  buildVariableDeclaration("stat_" + ptr_name, err_type, NULL, scope);
  ROSE_ASSERT(status_decl);
  
  SageInterface::insertStatementBefore(target, status_decl);
  
  //  cudaExtent extent = make_cudaExtent(width * sizeof(float), height, depth);                       
  extent_decl =  generateCudaExtentVar(params, scope, baseType);

  SageInterface::insertStatementAfter( status_decl, extent_decl);
  
  SgType* pitchPtr_type = buildOpaqueType("cudaPitchedPtr", scope); 
  ROSE_ASSERT(pitchPtr_type);

  dev_decl = buildVariableDeclaration(ptr_name, pitchPtr_type, NULL, scope); ;
  SageInterface::insertStatementAfter(extent_decl, dev_decl);
  SageInterface::attachComment(dev_decl, "Mint: Malloc on the device");
  
  
  SgFunctionCallExp* cudaMalloc = generateCudaMallocCall(dev_decl, extent_decl,  scope);
  SgStatement* malloc3D = buildAssignStatement(buildVarRefExp(status_decl), cudaMalloc);
  insertStatementAfter(dev_decl, malloc3D );
  
  SgStatement* status = generateCheckCudaSuccessStatement(buildVarRefExp(status_decl), scope);
  
  insertStatementAfter(malloc3D, status );
  
  return status; //last statement
}

void CudaMemoryManagement::issueDataTransferFromDevice(SgNode* node, 
						       const MintTrfParams_t params, 
						       MintHostSymToDevInitMap_t& hostToDevVars, 
						       bool inside)
{

  //Copy back to host                                                                             
  //cudaMemcpy3DParms qUold = { 0 };
  //qUold.srcPtr = dUold;
  //qUold.dstPtr = make_cudaPitchedPtr((void*)Uold[0], (n+2)*sizeof(REAL), (n+2), (n+2));
  //qUold.extent = dimArr;
  //qUold.kind = cudaMemcpyDeviceToHost;
  //status=cudaMemcpy3D(&qUold);
  //if(status != cudaSuccess){fprintf(stderr, "MemcpyDtoH: %s\n", cudaGetErrorString(status));}
  //cudaFree(dUold.ptr);
  //cudaFree(dUnew.ptr);

  SgStatement* target = isSgStatement(node);
  SgScopeStatement* g_scope = target->get_scope();
  
  SgFunctionDeclaration* func= SageInterface::getEnclosingFunctionDeclaration(g_scope);

  string dst_name = params.dst ; 
  string src_name = params.src ;

  SgInitializedName* dest = MintTools::findInitName(isSgNode(func), dst_name );

  if(dest == NULL){
    //might be a global variable
    SgGlobal* g_scope = SageInterface::getGlobalScope(target);
    dest = MintTools::findInitName(isSgNode(g_scope), dst_name );
    if(dest == NULL){
      cout << "  ERROR:Mint: @ Line " << node->get_file_info()->get_line()  << endl;
      cout << "  The destination variable "<< dst_name << " is undefined"<< endl; 
      ROSE_ABORT();
    } 
  }

  ROSE_ASSERT(dest);
  SgVariableSymbol* dest_sym = isSgVariableSymbol(dest->get_symbol_from_symbol_table ());
  ROSE_ASSERT(dest_sym);

  string targetStr = target->unparseToString();
  SageInterface::attachComment(target, "Mint: Replaced Pragma: " + targetStr);

  //SgType* arrType = dest_sym->get_type();
  //SgType* baseType = arrType->findBaseType();

  SgVarRefExp* status_var = buildVarRefExp("stat_" + src_name, g_scope);
  SgVarRefExp* dev_var = buildVarRefExp(src_name, g_scope);
  SgVarRefExp* extent_var = buildVarRefExp("ext_" + src_name, g_scope);

  //we didn't transfer this variable (write-only) but we still need to allocate space
  if(hostToDevVars.find(dest_sym) == hostToDevVars.end())
    {

      SgInitializedName* host_name = dest_sym -> get_declaration();
      ROSE_ASSERT(host_name);
      SgDeclarationStatement* host_decl = host_name -> get_declaration(); 
      ROSE_ASSERT(host_decl);
      SgScopeStatement* scope = host_decl -> get_scope();

      SgVariableDeclaration* status_decl = NULL;
      SgVariableDeclaration* extent_decl = NULL;
      SgVariableDeclaration* dev_decl = NULL;

      //We need to find where to insert the variable declaration in the kernel
      //When there is no data transfer to the device, then we need to 
      //find a location to declare and allocate the variable

      Rose_STL_Container<SgNode*> stmtList = NodeQuery::querySubTree(scope, V_SgOmpParallelStatement);

      ROSE_ASSERT(stmtList.size() != 0);
            
      //SgStatement* stmt = isSgStatement(*(stmtList.begin()));

 //     SgStatement* last_stmt = declareAndAllocateDevicePtr(stmt, dev_decl, 
//							   extent_decl, status_decl,
//							   baseType, src_name, params );      
      ROSE_ASSERT(dev_decl);      
      ROSE_ASSERT(status_decl);      
      ROSE_ASSERT(extent_decl);

      status_var = buildVarRefExp(status_decl);
      dev_var = buildVarRefExp(dev_decl);
      extent_var = buildVarRefExp(extent_decl);

      hostToDevVars[dest_sym] = dev_decl->get_decl_item(src_name);
    }

  SgType* param3D_type = buildOpaqueType("cudaMemcpy3DParms", g_scope);
  SgAssignInitializer* initToEmpty = buildAssignInitializer(buildVarRefExp("{0}", g_scope));

  string param_name = MintTools::generateCopyParamName(target);

  SgVariableDeclaration *param_decl = buildVariableDeclaration(param_name + src_name, 
							       param3D_type, initToEmpty, g_scope) ;
  SageInterface::insertStatementBefore(target, param_decl);
  SageInterface::attachComment(param_decl, "Mint: Copy device to host");

  //qUold.srcPtr = dUold;
  //qUold.dstPtr = make_cudaPitchedPtr((void*)Uold[0], (n+2)*sizeof(REAL), (n+2), (n+2));
  //qUold.extent = dimArr;
  //qUold.kind = cudaMemcpyDeviceToHost;

  generateCudaMemCpyFromDeviceParams(target, param_decl, dev_var ,  extent_var, dest,  params, g_scope);
  
  SgFunctionCallExp* cudaMemCpy3D = generateCudaMemCpyCall(param_decl, g_scope);
  SgStatement* memcpy = buildAssignStatement(status_var, cudaMemCpy3D);
  insertStatementBefore(target, memcpy );
 
  SgStatement* status = generateCheckCudaSuccessStatement(status_var, g_scope);
  insertStatementBefore(target, status );

  //cudaFree(dUold.ptr);
  //cudaFree(dUnew.ptr);

  removeStatement(target);

}

SgVariableDeclaration* CudaMemoryManagement::generateCudaExtentVar(MintTrfParams_t params, 
								   SgScopeStatement* g_scope,
								   SgType* baseType)
{
  //  cudaExtent extent = make_cudaExtent(width * sizeof(float), height, depth);                       
  SgExprListExp* paramList=NULL;

  string dst_name = params.dst ; 

  ROSE_ASSERT((params.sizes).size() <= 3);
  
  SgExpression* width = buildVarRefExp(params.sizes[0], g_scope);
  width = buildMultiplyOp(width ,buildSizeOfOp(baseType));
  
  if(params.sizes.size() == 3)
    {
      SgExpression* height = buildVarRefExp(params.sizes[1], g_scope);
      SgExpression* depth = buildVarRefExp(params.sizes[2], g_scope);
      paramList = buildExprListExp(width , height, depth);
    }
  else if(params.sizes.size()== 2) 
    {
      SgExpression* height= buildVarRefExp(params.sizes[1], g_scope);
      paramList = buildExprListExp(width, height, buildIntVal(1));
    }
  else 
    {
      paramList = buildExprListExp(width, buildIntVal(1), buildIntVal(1));
    }

  SgExpression *make_cudaExtent= buildFunctionCallExp("make_cudaExtent", buildVoidType(), paramList, g_scope);

  SgInitializer* initVal = buildAssignInitializer(make_cudaExtent);

  SgType* ext_type = buildOpaqueType("cudaExtent", g_scope);

  SgVariableDeclaration *extent_decl =  buildVariableDeclaration("ext_" + dst_name, ext_type, initVal, g_scope);
  
  ROSE_ASSERT(extent_decl);
  return extent_decl;
}
