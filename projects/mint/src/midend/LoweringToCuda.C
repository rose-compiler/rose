
/* 
   Author: Didem Unat
   Date  : Nov 2, 2009 
   This is a transformation class from openmp AST to cuda AST
 */

#include "rose.h"
#include "Outliner.hh"
#include "LoweringToCuda.h"

#include "rewrite.h"            //ast rewrite mechanism: insert, remove, replace                                   
//#include "OmpAttribute.h"       //openmp pragmas are defined                                                       
#include "unparser.h"  

#include "mintPragmas/MintPragmas.h"
//#include "arrayProcessing/MintArrayInterface.h"
#include <algorithm>

#include "mintTools/MintOptions.h"
#include "mintTools/MintTools.h"//

#include <sstream> //string streams
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace OmpSupport;

#if 0
  //! Create a stride expression from an existing stride 
  //expression based on the loop iteration's order (incremental or decremental)
  // The assumption is orig_stride is just the raw operand of the condition expression of a loop
  // so it has to be adjusted to reflect the real stride: *(-1) if decremental
  static SgExpression* createAdjustedStride(SgExpression* orig_stride, bool isIncremental)
  {
    ROSE_ASSERT(orig_stride);
    if (isIncremental)
      return copyExpression(orig_stride); // never share expressions
    else
    {
//      cout<<"\t"<<orig_stride->unparseToString()<<endl;
      return copyExpression(orig_stride);
      //return buildMultiplyOp(buildIntVal(-1),copyExpression(orig_stride));
    }
  }
#endif

static void convertAndFilter (const SgInitializedNamePtrList input, ASTtools::VarSymSet_t& output)
  {
    for (SgInitializedNamePtrList::const_iterator iter =  input.begin(); iter != input.end(); iter++)
    {
      const SgInitializedName * iname = *iter;
      SgVariableSymbol* symbol = isSgVariableSymbol(iname->get_symbol_from_symbol_table ()); 
      ROSE_ASSERT (symbol != NULL);
      if (! isSgClassType(symbol->get_type()))
        output.insert(symbol);
    }
  }


//! A helper function to generate implicit or explicit task for either omp parallel or omp task
// It calls the ROSE AST outliner internally. 
SgFunctionDeclaration* 
LoweringToCuda::generateOutlinedTask(SgNode* node, ASTtools::VarSymSet_t& devSyms, 
				     std::set<SgInitializedName*>& readOnlyVars,
				     MintHostSymToDevInitMap_t hostToDevVars, 
				     const SgVariableSymbol* dev_struct_sym,
				     ASTtools::VarSymSet_t& devSyms_withStruct)
{
  ROSE_ASSERT(node != NULL);
  SgOmpClauseBodyStatement* target = isSgOmpClauseBodyStatement(node);  
  ROSE_ASSERT (target != NULL);

  // must be either omp task or omp parallel
  //SgOmpTaskStatement* target1 = isSgOmpTaskStatement(node);
  //SgOmpParallelStatement* target2 = isSgOmpForStatement(node);
  //ROSE_ASSERT (target1 != NULL || target2 != NULL);

  SgStatement * body =  target->get_body();
  ROSE_ASSERT(body != NULL);
  SgFunctionDeclaration* result= NULL;
  //Initialize outliner 
  //always wrap parameters for outlining used during OpenMP translation
  Outliner::useParameterWrapper = false; 
  Outliner::useStructureWrapper = false;

  //TODO there should be some semantics check for the regions to be outlined
  //for example, multiple entries or exists are not allowed for OpenMP
  //This is however of low priority since most vendor compilers have this already
  SgBasicBlock* body_block = Outliner::preprocess(body);

  // Variable handling is done after Outliner::preprocess() to ensure a basic block for the body,
  // but before calling the actual outlining 
  // This simplifies the outlining since firstprivate, private variables are replaced 
  //with their local copies before outliner is used 
  transOmpVariables (target, body_block, NULL);

  ASTtools::VarSymSet_t pSyms, fpSyms,reductionSyms, pdSyms;

  string func_name = MintTools::generateFuncName(target);
  SgGlobal* g_scope = SageInterface::getGlobalScope(body_block);
  ROSE_ASSERT(g_scope != NULL);

  cout << "  INFO:Mint: Generating a kernel with name -- " << func_name << " --"<< endl;
  // This step is less useful for private, firstprivate, and reduction variables
  // since they are already handled by transOmpVariables(). 
  //Outliner::collectVars(body_block, syms, pSyms);
  CudaOutliner::collectVars(body_block, devSyms);

  //TODO: we should do something about the readOnlyVars, why not send them as const
  //SageInterface::collectReadOnlyVariables(body_block,readOnlyVars);
  // We choose to be conservative about the variables needing pointer dereferencing first
  // AllParameters - readOnlyVars  - private -firstprivate 
  // Union ASTtools::collectPointerDereferencingVarSyms(body_block, pdSyms) 

  // Assume all parameters need to be passed by pointers first
  //std::copy(syms.begin(), syms.end(), std::inserter(pdSyms,pdSyms.begin()));

  //exclude firstprivate variables: they are read only in fact
  //TODO keep class typed variables!!!  even if they are firstprivate or private!! 
  SgInitializedNamePtrList fp_vars = collectClauseVariables (target, V_SgOmpFirstprivateClause);
  ASTtools::VarSymSet_t fp_syms, pdSyms2;
  convertAndFilter (fp_vars, fp_syms);
  set_difference (pdSyms.begin(), pdSyms.end(),
      fp_syms.begin(), fp_syms.end(),
      std::inserter(pdSyms2, pdSyms2.begin()));

  // Similarly , exclude private variable, also read only
 SgInitializedNamePtrList p_vars = collectClauseVariables (target, V_SgOmpPrivateClause);
 ASTtools::VarSymSet_t p_syms, pdSyms3;
 convertAndFilter (p_vars, p_syms);
  //TODO keep class typed variables!!!  even if they are firstprivate or private!! 
  set_difference (pdSyms2.begin(), pdSyms2.end(),
      p_syms.begin(), p_syms.end(),
      std::inserter(pdSyms3, pdSyms3.begin()));
 
  // lastprivate and reduction variables cannot be excluded  since write access to their shared copies

  // a data structure used to wrap parameters
  //SgClassDeclaration* struct_decl = Outliner::generateParameterStructureDeclaration (body_block, func_name, syms, pdSyms3, g_scope);
  //ROSE_ASSERT (struct_decl != NULL); // can be NULL if no parameters to be passed

  //Generate the outlined function
  cout << "  INFO:Mint: Generating the outlined function " << endl;

  if(dev_struct_sym != NULL)
    {
      //We will send the dev_struct and scalar variables, 
      //so we need to remove the vector variables from devSyms
      
      //creates a new devSyms list by removing the ones that will be passed with struct
      CudaOutliner::getDevSymsNotInStruct(devSyms_withStruct, devSyms, hostToDevVars);

      //add the struct to the parameters to be passed to the kernel
      devSyms_withStruct.insert(dev_struct_sym);

      result = CudaOutliner::generateFunction (body_block, func_name, 
					       devSyms_withStruct, hostToDevVars, pdSyms3, pdSyms3, g_scope);         
    }
  else{
      result = CudaOutliner::generateFunction (body_block, func_name, 
					       devSyms, hostToDevVars, pdSyms3, pdSyms3, g_scope);   
    }

  cout << "  INFO:Mint: Inserting function into the scope " << endl;

  ROSE_ASSERT(result);
  ROSE_ASSERT(g_scope);
  ROSE_ASSERT(body_block);

  Outliner::insert(result, g_scope, body_block);

  cout << "  INFO:Mint: Created function prototype" << endl;

 isSgFunctionDeclaration(result->get_firstNondefiningDeclaration())->get_functionModifier().setCudaKernel(); 
  // A fix from Tristan Ravitch travitch@cs.wisc.edu to make outlined functions static to avoid name conflicts
  //SageInterface::setStatic(result->get_definingDeclaration());
  //SageInterface::setStatic(result->get_firstNondefiningDeclaration());

  // Generate a call to the outlined function
  // Generate packing statements
  // must pass target , not body_block to get the right scope in which the declarations are inserted
  //string wrapper_name= Outliner::generatePackingStatements(target,syms,pdSyms3, struct_decl);
  ROSE_ASSERT (result != NULL);

  return result;
}

/*
Todo with MintForClauses_t

Nested: parallelize based on how many rows 
allowed to be parallelized
Thread sizes: we need to use these sizes in the nested,
shared memory sizes or other things, effects should propogate
to the optimizer
*/

void  configureDim3KernelParams(SgNode* node, const MintForClauses_t clauseList, 
				SgVariableDeclaration* &thread_decl, SgVariableDeclaration* & grid_decl, 
				const string dimGrid, const string dimBlock, ASTtools::VarSymSet_t& deviceSyms)
{
  
  ROSE_ASSERT(node != NULL);
  SgOmpForStatement* target = isSgOmpForStatement(node);
  ROSE_ASSERT (target != NULL);
  SgScopeStatement* p_scope = target->get_scope();
  ROSE_ASSERT (p_scope != NULL);
  SgScopeStatement* g_scope = getGlobalScope(target);
  ROSE_ASSERT (g_scope != NULL);
  SgStatement * body =  target->get_body();
  ROSE_ASSERT(body != NULL);

  // get index ranges upper - lower + 1 before outline 
  // we need this info to compute number of thread blocks 
  MintInitNameExpMap_t index_ranges; 
  MintTools::forLoopIndexRanges(node, index_ranges);

  //Number of blocks: loop range % tileDim.x == 0 ? range/ tileDim.x : range/tileDim.x + 1
  //dim3 grid(numXblocks, numYblocks * numZblocks );

  std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(body,V_SgForStatement);
  //outmost loop first 

  int nesting = clauseList.nested ; 

  std::vector<string> numblockList; 

  for( std::vector<SgForStatement* > ::iterator i = loopNest.begin(); i != loopNest.end(); i++)
    {

      SgForStatement* for_loop = isSgForStatement(*i);
      SgInitializedName* index_var = getLoopIndexVariable(for_loop);

      MintInitNameExpMap_t :: iterator loop_ind = index_ranges.find(index_var);
      
      ROSE_ASSERT(loop_ind != index_ranges.end());
      
      SgExpression* range = loop_ind -> second;      
      
      //create a variable called numXblocks = range exp / tiledim.x with fringe 
      //SgVarRefExp* blockdim_xyz = nesting == 1 ? buildVarRefExp (TILE_X, g_scope) : buildVarRefExp(TILE_Y , g_scope);
      //blockdim_xyz = nesting == 3 ? buildVarRefExp (TILE_Z, g_scope) : blockdim_xyz ;

      SgIntVal* intVal = buildIntVal((nesting == 1) ? clauseList.tileDim.x: clauseList.tileDim.y); 
      intVal = (nesting == 3) ? buildIntVal(clauseList.tileDim.z) : intVal; 

      SgExpression* numblocks = buildDivideOp(range, intVal);
      SgExpression* uneven_block_cond = buildEqualityOp(buildModOp(range, intVal),buildIntVal(0)); //range % blocksize ==0                                                    
      //range/blocksize + 1
      SgExpression* numblocks_exp = buildConditionalExp(uneven_block_cond, numblocks, buildAddOp(numblocks,buildIntVal(1))); 

      std::stringstream tmp; tmp <<  nesting ; 
      string var_name = "num" + tmp.str() + dimBlock; 

      SgVariableDeclaration* numblocks_decl = buildVariableDeclaration(var_name, buildIntType(), 
								       buildAssignInitializer(numblocks_exp), p_scope);

      numblockList.push_back(var_name);

      ROSE_ASSERT(numblocks_decl);
      insertStatementBefore(target, numblocks_decl);
      nesting--;

      if(nesting== 0)
	break;

    } //end of for 

  if(clauseList.nested == 3)
    {
      SgVariableDeclaration* invYnumBlocks= buildVariableDeclaration("invYnum" + dimBlock, buildFloatType(), 
								     buildAssignInitializer(buildDivideOp(buildFloatVal(1.0), 
								     buildVarRefExp("num2" + dimBlock, p_scope))), p_scope) ;
      ROSE_ASSERT(invYnumBlocks);
      //float invYnumBlocks = 1.0f/ (float)numYblocks;
      insertStatementBefore(target, invYnumBlocks);
      
      SgVariableSymbol* invYnumBlocks_sym = getFirstVarSym(invYnumBlocks);

      deviceSyms.insert(invYnumBlocks_sym);

      SgVariableSymbol* numBlocksInY = p_scope->lookup_var_symbol("num2" + dimBlock);
      ROSE_ASSERT(numBlocksInY);
      
      deviceSyms.insert(numBlocksInY);
    }

  cout << "  WARNING:MINT: opaque type dim3" << endl ;                    
  SgType* dim3_type = buildOpaqueType("dim3", p_scope);

  std::stringstream tmp_x; tmp_x  <<  clauseList.tileDim.x/clauseList.chunksize.x ; 
  std::stringstream tmp_y; tmp_y  <<  clauseList.tileDim.y/clauseList.chunksize.y ; 
  std::stringstream tmp_z; tmp_z  <<  clauseList.tileDim.z/clauseList.chunksize.z  ; 

  string blocksizes = dimBlock + "(" + tmp_x.str() + "," + tmp_y.str() +  "," + tmp_z.str() +  ")";
  //threads();

//  SgVarRefExp* thread_exp = buildVarRefExp(blocksizes, p_scope);

  thread_decl = buildVariableDeclaration(blocksizes, dim3_type); 
  
  ROSE_ASSERT(thread_decl);

  string gridsizes; 
  //dim3 grid(numXblocks, numYblocks * numZblocks );
  if(numblockList.size() == 1)
    {
      std::stringstream dim_x; dim_x << numblockList[0] ;       
      gridsizes = dimGrid + "(" + dim_x.str() + ")"; 
    }
  if(numblockList.size() == 2)
    {
      std::stringstream dim_y; dim_y << numblockList[0] ;       
      std::stringstream dim_x; dim_x << numblockList[1] ;       
      gridsizes = dimGrid + "(" + dim_x.str() + "," + dim_y.str() +  ")";
    }
  if(numblockList.size() == 3)
    {
      std::stringstream dim_z; dim_z << numblockList[0] ;       
      std::stringstream dim_y; dim_y << numblockList[1] ;       
      std::stringstream dim_x; dim_x << numblockList[2] ;       
      gridsizes = dimGrid + "(" + dim_x.str() + "," + dim_y.str() +  "*" + dim_z.str() +  ")";
    }

//  SgVarRefExp* grid_exp = buildVarRefExp(gridsizes, p_scope);

  grid_decl = buildVariableDeclaration(gridsizes, dim3_type);  
  
  ROSE_ASSERT(grid_decl);

}

  /*
  SgClassDeclaration* dim3_class = buildClassDeclaration("dim3", p_scope);
  ROSE_ASSERT(dim3_class);

  SgClassDefinition* dim3_def = buildClassDefinition(dim3_class);
  ROSE_ASSERT(dim3_def);

  SgDeclarationStatement* dim3_stmt = dim3_class->get_firstNondefiningDeclaration(); 
  dim3_stmt ->setForward();

  SgFunctionParameterList *block_params = buildFunctionParameterList();

  SgInitializedName* param1 = buildInitializedName("dimx", buildIntType());
  SgInitializedName* param2 = buildInitializedName("dimy", buildIntType());
  SgInitializedName* param3 = buildInitializedName("dimz", buildIntType());

  prependArg(block_params, param1);
  prependArg(block_params, param2);
  prependArg(block_params, param3);

  ROSE_ASSERT(block_params);

  SgMemberFunctionDeclaration* dim3_const = buildDefiningMemberFunctionDeclaration ( "dim3", buildVoidType(),										  
										     block_params, dim3_def);

  ROSE_ASSERT(dim3_const);

  SgExprListExp* const_params = buildExprListExp(buildIntVal(clauseList.tileDim.x), 
						 buildIntVal(clauseList.tileDim.y), 
						 buildIntVal(clauseList.tileDim.z));

  //SgType* dim3_type = dim3_class->get_type();
  SgClassType* dim3_type = new SgClassType(dim3_stmt);
 
  ROSE_ASSERT(dim3_type);
  SgConstructorInitializer* const_init = buildConstructorInitializer ( dim3_const, const_params, 
								       dim3_type, false, false, false, false);

  
  thread_decl =  buildVariableDeclaration(dimBlock ,  dim3_type , const_init,  p_scope );

  cout << thread_decl-> unparseToString() << endl ;
  */

void LoweringToCuda::addInvYBlockNumDecl(SgFunctionDeclaration* func)
{

  SgBasicBlock* func_body = func->get_definition()->get_body();
  ROSE_ASSERT(func_body);

  SgFunctionParameterList* params = func->get_parameterList ();
  SgInitializedNamePtrList paramList = params->get_args();

  SgScopeStatement* scope = func_body->get_scope();
  ROSE_ASSERT (scope != NULL);

  SgStatement* firstStmt = getFirstStatement(scope);

  if(firstStmt == NULL){

    SgStatementPtrList stmt_list = func_body->get_statements();
    ROSE_ASSERT(stmt_list.size()!=0);
    firstStmt = stmt_list.at(0);
  }

  ROSE_ASSERT(firstStmt);

  //add invYnum and numBlocksInY definitions 

  SgInitializedNamePtrList::const_iterator i;
  
  for(i=paramList.begin(); i!=paramList.end(); i++)
    {

      SgInitializedName* arg = (*i);
      string arg_name = arg->get_name(); 

      if(arg_name.find("invYnum") != string::npos )
	{
	  SgVariableDeclaration* invBlocksInY = buildVariableDeclaration("invBlocksInY", buildFloatType(), 
									 buildAssignInitializer(buildVarRefExp(arg, scope)), scope);
	  
	  insertStatementBefore(firstStmt, invBlocksInY);
	}
      else if (arg_name.find("num2blockDim") != string::npos )
      {
	SgVariableDeclaration* blocksInY = buildVariableDeclaration("blocksInY", buildFloatType(), 
								    buildAssignInitializer(buildVarRefExp(arg, scope)), scope);
	
	insertStatementBefore(firstStmt, blocksInY);

      }
    }
}

/*
  This function converts given omp for loop into a cuda kernel
  Inputs:
  -node points to the omp-for pragma
  -hostToDevVars contains the host-device variable mapping that we drive 
  from the copy pragmas
  -all_deviceSyms contains all the variable that we needed to send over
  the device, it contains both vector and scalar variables, we update the list
  with the variables that are going to be passed to the device with this kernel launch
  -dev_struct is the struct for kernel parameters. If there are too many parameters
  then we transfer the vector parameters with a struct. We create only one struct
  per parallel region but only unpack the ones needed for the kernel inside the kernel.
  
  Step 1. get the parameters for the for-directive e.g nest, tile ...
  Step 2. generate the names for cuda grid and block
  Step 3. generate kernel configurations e.g # of blocks, threads
  Step 4. generate the outlined function 
  Step 5. Unpack the variable inside the outlined function 
  Step 6. In the outlined-func, covert for-loops into if-stmt and replace indices with the thread IDs
  Step 7. Generate the kernel launch and replace it with the omp-for directive
  Step 8. Add a sync point after the kernel launch.

*/

SgFunctionDeclaration* LoweringToCuda::transOmpFor(SgNode* node, 
						   MintHostSymToDevInitMap_t hostToDevVars,
						   ASTtools::VarSymSet_t& all_deviceSyms, 
						   MintForClauses_t& clauseList, 
						   const SgVariableSymbol* dev_struct)
{
  ROSE_ASSERT(node != NULL);
  SgOmpForStatement* target = isSgOmpForStatement(node);
  ROSE_ASSERT (target != NULL);
  SgScopeStatement* p_scope = target->get_scope();
  ROSE_ASSERT (p_scope != NULL);
  
  SgStatement * body =  target->get_body();
  ROSE_ASSERT(body != NULL);

  SgStatement* prevNode = getPreviousStatement(target);

  //parse mint pragmas for omp for pragma
  clauseList = MintPragmas::getForParameters(isSgNode(prevNode));

  cout << "  INFO:Mint: Using following configurations for omp for @ Line "<<  body->get_file_info()->get_line() << endl ;
  cout << "  Nesting Level : " << clauseList.nested ;
  cout << ", Tile Sizes : "  << "(" << clauseList.tileDim.x << ","
       << clauseList.tileDim.y <<","<< clauseList.tileDim.z << ")"; 
  cout << ", Thread Geometries : "  << "(" << clauseList.tileDim.x/clauseList.chunksize.x << ","
       << clauseList.tileDim.y/clauseList.chunksize.y <<","<< clauseList.tileDim.z/clauseList.chunksize.z << ")"; 
  cout << ", Chunk Sizes : "  << "(" << clauseList.chunksize.x << ","
       << clauseList.chunksize.y <<","<< clauseList.chunksize.z << ")"<< endl << endl; 

  //Kernel configurations dim3 grids() and dim3 threads()
  string dimGrid = MintTools::generateGridDimName(target);
  string dimBlock = MintTools::generateBlockDimName(target);

  SgVariableDeclaration* thread_decl= NULL; 
  SgVariableDeclaration* grid_decl= NULL; 

  ASTtools::VarSymSet_t deviceSyms;
  ASTtools::VarSymSet_t deviceSyms_withStruct;

  std::set<SgInitializedName*> readOnlyVars;

  // Save preprocessing info as early as possible, avoiding mess up from the outliner
  AttachedPreprocessingInfoType save_buf1, save_buf2, save_buf_inside;
  cutPreprocessingInfo(target, PreprocessingInfo::before, save_buf1) ;
  cutPreprocessingInfo(target, PreprocessingInfo::after, save_buf2) ;
  cutPreprocessingInfo(target, PreprocessingInfo::inside, save_buf_inside) ;

  configureDim3KernelParams(node, clauseList, thread_decl, grid_decl, dimGrid, dimBlock, deviceSyms);

  Outliner::enable_debug = false; //set it to true for debugging

  // generated an outlined function as the task
  SgFunctionDeclaration* outlined_func = generateOutlinedTask (node, deviceSyms, readOnlyVars, 
							       hostToDevVars, dev_struct, deviceSyms_withStruct);

  // add this kernel params to all kernel parameters for parallel region
  CudaOutliner::addNewVars(all_deviceSyms, deviceSyms);

  if(clauseList.nested > 2)
    addInvYBlockNumDecl(outlined_func);

  cout << "  INFO:Mint: Unpacking cuda pitched pointers " << endl;
  
  //unpacks cudaPitchedPtr variables inside the kernel
  unpackCudaPitchedPtrs(outlined_func, hostToDevVars, dev_struct, deviceSyms);

  cout << "  INFO:Mint: Handling loop nest " << endl;

  LoweringToCuda::nestedForLoops(outlined_func, clauseList);

  cout << "  INFO:Mint: Generating function call and replacing it with the parallel for region " << endl;

  SgStatement* func_call = NULL;
  
  if(dev_struct == NULL)
    func_call= CudaOutliner::generateCall (outlined_func, hostToDevVars, 
					   deviceSyms, p_scope,dimBlock, dimGrid);
  else 
    func_call= CudaOutliner::generateCall (outlined_func, hostToDevVars, 
					   deviceSyms_withStruct, p_scope,dimBlock, dimGrid);

  ROSE_ASSERT(func_call != NULL);  
  
  // Replace the parallel region with the function call statement
  // TODO should we introduce another level of scope here?
  SageInterface::replaceStatement(target,func_call, true);

  pastePreprocessingInfo(func_call, PreprocessingInfo::before, save_buf1); 
  pastePreprocessingInfo(outlined_func->get_definition()->get_body(), PreprocessingInfo::inside, save_buf_inside); 

  // some #endif may be attached to the body, we should not move it with the body into
  // the outlined funcion!!
  // move dangling #endif etc from the body to the end of s2
  movePreprocessingInfo(body,func_call,PreprocessingInfo::before, PreprocessingInfo::after); 

  //synchronization and error checking code   
  SgStatement* sync_call = buildFunctionCallStmt("cudaThreadSynchronize", buildVoidType(), NULL, p_scope);

  insertStatementAfter(func_call, sync_call);  

  SgVariableDeclaration* errVar = generateCudaGetLastErrorStatement(outlined_func->get_name(), p_scope);
  ROSE_ASSERT(errVar);
  insertStatementAfter(sync_call, errVar);  

  SgStatement* printErr = getCudaErrorStringStatement(buildVarRefExp(errVar) , p_scope, outlined_func->get_name());
  ROSE_ASSERT(printErr);

  insertStatementAfter(errVar, printErr); 

  insertStatementBefore(func_call, thread_decl);

  insertStatementBefore(func_call, grid_decl);

  bool preferL1 = MintOptions::GetInstance()-> isL1Preferred();

  if(preferL1){
    //Insert the function to prefer L1 cache over shared memory if the compiler option is set
    //cudaFuncSetCacheConfig(mint_6_1527, cudaFuncCachePreferL1);
    
    SgFunctionRefExp* func_ref = buildFunctionRefExp(outlined_func);
    ROSE_ASSERT(func_ref);

    SgVarRefExp* cachePreferL1 = buildVarRefExp("cudaFuncCachePreferL1", p_scope);
    ROSE_ASSERT(cachePreferL1);

    SgExprListExp* parameters = buildExprListExp( func_ref, cachePreferL1);

    SgStatement* config_call = buildFunctionCallStmt("cudaFuncSetCacheConfig", buildVoidType(), parameters, p_scope);

    ROSE_ASSERT(config_call);

    insertStatementBefore(thread_decl, config_call);
  }

  return outlined_func;

}

void LoweringToCuda::unpackCudaPitchedPtrs(SgFunctionDeclaration* func, 
					   const MintHostSymToDevInitMap_t hostToDevVars, 
					   const SgVariableSymbol* dev_struct,
					   const ASTtools::VarSymSet_t devSyms)
{
  //For each function parameter
  // check if it is with type cudaPitchedPtr 
  //       Yes: then find its key in the hostToDevVars
  //       Create a variable with the same name as the host sym 
  //       Variable should have the same base type 
  //       Ex: REAL* Uold = (REAL*) dUoldPtr.ptr;
  //       Then set the slice and width 
  //       int widthUold = pitchUold/sizeof(REAL);
  //       int slice = dUoldPtr.ysize * widthUold ;

  SgBasicBlock* func_body = func->get_definition()->get_body();
  ROSE_ASSERT(func_body);
 
  SgScopeStatement* scope = func_body->get_scope();
  ROSE_ASSERT (scope != NULL);

  SgStatement* firstStmt = getFirstStatement(scope);

  if(firstStmt == NULL){

    SgStatementPtrList stmt_list = func_body->get_statements();
    ROSE_ASSERT(stmt_list.size()!=0);
    firstStmt = stmt_list.at(0);
  }

  ROSE_ASSERT(firstStmt);

  SgFunctionParameterList* params = func->get_parameterList ();
  SgInitializedNamePtrList paramList = params->get_args();

  if(dev_struct != NULL)
    {
      //We need to read the fields of the struct into local variables and then unpack them
      //cudaPitchedPtr dev_2_Unew = dev__out_argv1__1527__ -> dev_2_Unew;
      //cudaPitchedPtr dev_1_Uold = dev__out_argv1__1527__ -> dev_1_Uold;

      SgScopeStatement* struct_scope = dev_struct->get_scope();
      SgInitializedName* struct_name = (dev_struct)->get_declaration ();
      //string struct_name_str = struct_name->get_name().str();

      //This is the list of actual device symbol list 
      for (ASTtools::VarSymSet_t::const_iterator i = devSyms.begin (); i!= devSyms.end (); ++i)
	{     
	  SgVariableSymbol* sym= const_cast<SgVariableSymbol*> (*i);
	  
	  //If it is in the hostToDevVars list then, the variable should be of the fields in the struct
	  if(hostToDevVars.find(sym) != hostToDevVars.end())
	    {
	      MintHostSymToDevInitMap_t::const_iterator it =hostToDevVars.find(sym); 
	      SgVariableSymbol* host_sym = (*it).first;
	      SgInitializedName* dev_name = (*it).second; //this is its field name
	      ROSE_ASSERT(host_sym);
	      ROSE_ASSERT(dev_name);

	      string dev_name_str = dev_name ->get_name().str();

	      SgAssignInitializer* struct_field = buildAssignInitializer(buildArrowExp(buildVarRefExp(struct_name, struct_scope), 
										       buildVarRefExp(dev_name_str, struct_scope)));
	      ROSE_ASSERT(struct_field);

	      SgVariableDeclaration* pitched_ptr = buildVariableDeclaration(dev_name_str, dev_name->get_type(), struct_field, scope);

	      ROSE_ASSERT(pitched_ptr);

	      insertStatementBefore(firstStmt, pitched_ptr);

	      paramList.push_back(dev_name);
	    }
	}      
    }

  //if the useSameIndex flag is set, then we only declare width and slice variable once
  //use the common width and slice variables for all the arrays

  bool useSameIndex = MintOptions::GetInstance()-> useSameIndex();
  bool once = false; 

  SgInitializedNamePtrList::const_iterator i;
  
  for(i=paramList.begin(); i!=paramList.end(); i++){

    SgInitializedName* arg = (*i);
    string arg_name = arg->get_name(); 
    
    for(MintHostSymToDevInitMap_t::const_iterator it = hostToDevVars.begin(); it != hostToDevVars.end(); it++)
      {
	SgVariableSymbol* host_sym = (*it).first ; 
	SgInitializedName* dev_name = (*it).second ;
	ROSE_ASSERT(dev_name);
	string dev_name_str = dev_name->get_name();

	if(dev_name_str == arg_name )
	  { //symbol is found 	    
	    //REAL* Uold = (REAL*) dUoldPtr.ptr;	    
	    string host_str = host_sym->get_name().str();
	    
	    SgType* baseType = host_sym->get_type()->findBaseType();
	    SgType* type = buildPointerType(baseType);

	    SgVarRefExp* dev_exp = buildVarRefExp(dev_name);
	    SgExpression* pitchedPtr = buildDotExp(dev_exp, buildVarRefExp("ptr", scope));
	    ROSE_ASSERT(pitchedPtr);

	    SgInitializer* initVal = buildAssignInitializer(buildCastExp(pitchedPtr, type, SgCastExp::e_C_style_cast));
	    SgVariableDeclaration *dev_decl = buildVariableDeclaration(host_str, type, initVal, scope); 
	    ROSE_ASSERT(dev_decl);

	    insertStatementBefore(firstStmt, dev_decl);

	    //we declare slice and width variables only once
	    if(once)
	      break;

	    if(useSameIndex)
	      {
		once = true;
		//int widthUold = dUoldPtr.pitch/sizeof(REAL);
		//int heightUold = dUoldPtr.ysize; // do we need height?
		//int sliceUold = dUoldPtr.ysize * widthUold ;
		string width_str = "_width" ;
		//string height_str = "height" + host_str ;
		string slice_str = "_slice" ;
		
		SgInitializer* initW = buildAssignInitializer(buildDivideOp(buildDotExp(dev_exp, buildVarRefExp("pitch", scope)), 
									    buildSizeOfOp(baseType)));
//		SgInitializer* initH = buildAssignInitializer(buildDotExp(dev_exp, buildVarRefExp("ysize", scope) )) ;
		
		SgVariableDeclaration *width_decl = buildVariableDeclaration(width_str, buildIntType(), initW, scope);
		//SgVariableDeclaration *height_decl = buildVariableDeclaration(height_str, buildIntType(), initH, scope);
		
		SgInitializer* initS = buildAssignInitializer(buildMultiplyOp(buildDotExp(dev_exp, buildVarRefExp("ysize", scope)), 
									      buildVarRefExp(width_decl)));
		
		SgVariableDeclaration *slice_decl = buildVariableDeclaration(slice_str, buildIntType(), initS, scope); 
		
		ROSE_ASSERT(width_decl);
		//ROSE_ASSERT(height_decl);
		ROSE_ASSERT(slice_decl);
		
		insertStatementBefore(firstStmt, width_decl);
		//insertStatementBefore(firstStmt, height_decl);
		insertStatementBefore(firstStmt, slice_decl);
		
	      }
	    else
	      {
		//int widthUold = dUoldPtr.pitch/sizeof(REAL);
		//int heightUold = dUoldPtr.ysize; // do we need height?
		//int sliceUold = dUoldPtr.ysize * widthUold ;
		string width_str = "width" + host_str ;
		//string height_str = "height" + host_str ;
		string slice_str = "slice" + host_str ;    
		
		SgInitializer* initW = buildAssignInitializer(buildDivideOp(buildDotExp(dev_exp, buildVarRefExp("pitch", scope)), 
									    buildSizeOfOp(baseType)));
//		SgInitializer* initH = buildAssignInitializer(buildDotExp(dev_exp, buildVarRefExp("ysize", scope) )) ;
		
		SgVariableDeclaration *width_decl = buildVariableDeclaration(width_str, buildIntType(), initW, scope);
		//SgVariableDeclaration *height_decl = buildVariableDeclaration(height_str, buildIntType(), initH, scope);
		
		SgInitializer* initS = buildAssignInitializer(buildMultiplyOp(buildDotExp(dev_exp, buildVarRefExp("ysize", scope)), 
									      buildVarRefExp(width_decl)));
		
		SgVariableDeclaration *slice_decl = buildVariableDeclaration(slice_str, buildIntType(), initS, scope); 
		
		ROSE_ASSERT(width_decl);
		//ROSE_ASSERT(height_decl);
		ROSE_ASSERT(slice_decl);
		
		insertStatementBefore(firstStmt, width_decl);
		//insertStatementBefore(firstStmt, height_decl);
		insertStatementBefore(firstStmt, slice_decl);
	      }
	
	    break;
	  }
      }
  }
}

SgVariableDeclaration* LoweringToCuda::generateCudaGetLastErrorStatement(string suffix, 
									 SgScopeStatement* scope)
{

  //insert error check codes
  //Ex: cudaError_t err = cudaGetLastError();
 
  SgType* err_type = buildOpaqueType("cudaError_t", scope);
  ROSE_ASSERT(err_type);
 
  SgInitializer* initVal = buildAssignInitializer(buildFunctionCallExp("cudaGetLastError", buildVoidType(),
								       NULL, scope));

  SgVariableDeclaration *status_decl =  buildVariableDeclaration("err_"+ suffix, err_type, initVal, scope);
  ROSE_ASSERT(status_decl);
  
  return status_decl;
}

SgStatement* LoweringToCuda::getCudaErrorStringStatement(SgVarRefExp* errVar,
							 SgScopeStatement* scope, 
							 string func_name)

{

  ROSE_ASSERT(errVar);

  //if(err)  {fprintf(stderr, "%s \n", cudaGetErrorString(err) ) ;}
  SgFunctionCallExp* cudaGetErr = buildFunctionCallExp("cudaGetErrorString", 
						       buildVoidType(), buildExprListExp(errVar), scope);
  //"%s\n"                                                                                                             
  SgExpression* output =buildStringVal("In %s, %s\\n");
  ROSE_ASSERT(output);

  SgVarRefExp* stderrRef = buildVarRefExp("stderr", scope);

  SgExpression* funcRef = buildStringVal(func_name);
  ROSE_ASSERT(funcRef);

  SgExprListExp* paramList = buildExprListExp(stderrRef, output, funcRef, cudaGetErr);

  SgBasicBlock* block = buildBasicBlock();
  ROSE_ASSERT(block);

  SgStatement* body = buildFunctionCallStmt("fprintf", buildVoidType() , paramList, scope);
  ROSE_ASSERT(body);

  appendStatement(body, block);
  
  SgIfStmt* if_stmt = buildIfStmt(errVar, block, NULL);
  ROSE_ASSERT(if_stmt);

  return isSgStatement(if_stmt);
}

//What is a boundary condition loop
//if an array is n-dim and the loop is n-1 dim
//
bool LoweringToCuda::isBoundaryConditionLoop(SgNode* node)
{

  //Didem: April 20, 2011, This is way to complicated. I just checked if the depth of the 
  //loop is less than the array subcript size. Not entirely reliable but more reliable 
  //than the previous test. 

  //1.the loop might be a nested loop, query other for loops in this loop
  //2.find the index variable of each for loop
  //3.In the loop body find the pointer array references 
  //4.For each array reference, find the dimension of the array and find the 
  //indices used in the subscription
  //5.If at least one of  index in the array reference doesnot depend on the loop index, this might be a boundary 
  //condition. This should hold for all the array references in the loop.
  //6.Check if index modifies the other most region (read might be from the interior regions)

  bool yes = false;

  //1.the loop might be a nested loop, query other for loops in this loop
  //2.find the index variable of each for loop
  std::vector<SgInitializedName*> indexList;
  std::vector<SgForStatement* > loopNest= SageInterface::querySubTree<SgForStatement>(node,V_SgForStatement);
  int loopDeep = loopNest.size();

  for (std::vector<SgForStatement* > ::iterator i = loopNest.begin(); i!= loopNest.end(); i++)
    {
      SgInitializedName* loop_ind = getLoopIndexVariable(*i); 
      indexList.push_back(loop_ind);
    }

  Rose_STL_Container<SgNode*> arrRefList= NodeQuery::querySubTree(node, V_SgPntrArrRefExp);

  //We just check if the dimension of the array, if the loop depth is less than that, 
  //we know that we are only touching the surface of the array not the entire grid. 
  //In that case, return true (yes, it is a boundary condition loop)
  for ( Rose_STL_Container<SgNode*>::iterator arr = arrRefList.begin(); arr != arrRefList.end(); arr++)
    {
      SgExpression* arrayName; 
      vector<SgExpression*>  subscripts; //first index is i if E[j][i]
      
      //index list are from right to left 
      bool yesArrayRef = MintArrayInterface::isArrayReference(isSgExpression(*arr), &arrayName, &subscripts);
      assert(yesArrayRef == true);

      int arrDim = subscripts.size();
      
      arr = arr + arrDim - 1 ;

      if(loopDeep < arrDim)
	{
	  yes = true;
	  break;
	}
    }  
  /*
  for ( Rose_STL_Container<SgNode*>::iterator arr = arrRefList.begin(); arr != arrRefList.end(); arr++)
    {

      SgExpression* arrayName; 
      vector<SgExpression*>  subscripts; //first index is i if E[j][i]
      
      //index list are from right to left 
      bool yesArrayRef = MintArrayInterface::isArrayReference(isSgExpression(*arr), &arrayName, &subscripts);
      assert(yesArrayRef == true);
 
      int arrDim = subscripts.size();

      arr = arr + arrDim - 1 ;

      vector<SgExpression*>::iterator index;
            
      int found =0;

      for(index = subscripts.begin(); index != subscripts.end(); index++)
	{
	  SgExpression* indexExp = *index;

	  string indexExpStr = indexExp->unparseToString();
	  	  
	  //check if this index expression depends on loop index
	  //For example: [n-1] doesnot depend on i or j but [i+1] does
	  std::vector<SgInitializedName*>::iterator it;

	  found = 0;

	  for(it = indexList.begin(); it!= indexList.end(); it++)
	    {
	      string loopIndexStr = (*it)->get_name().str() ;
	
	      if(indexExpStr.find(loopIndexStr) != -1) //found
		found++;
	    }

	  if(found == 0)
	    yes= true; //yes it is boundary condition.

	}

      if(found == subscripts.size())
	return false; // at least one of the array reference is not boundary
	
    }
  */
  if (yes)
    cout << "  INFO:Mint: this is a  boundary condition loop " << endl ;
      //cout <<"Mint:isBoundaryConditionLoop: expressions :" <<  (*arr)->unparseToString() << endl;
      //cout <<"Mint:isBoundaryConditionLoop: subexp      :" <<  arrDim << endl;
      
  return yes;
}

bool LoweringToCuda::isStencilLoop(SgNode* node)
{
//need more sophisticated way to test this, 
//it is not reliable 

  MintInitNameMapExpList_t arrRefList;
  MintArrayInterface::getArrayReferenceList(node, arrRefList);

  MintInitNameMapExpList_t::iterator it;
  for(it = arrRefList.begin(); it != arrRefList.end(); it++)
    {
//      SgInitializedName* array = it->first;
      std::vector<SgExpression*> expList = it->second;

      if(MintArrayInterface::isStencilArray(expList))
	{
	  return true;
	}
    }
  return false;
}



SgVariableDeclaration*  LoweringToCuda::threadIndexCalculations(SgBasicBlock* outerBlock, SgForStatement* for_loop , 
								int loopNo, bool isStencil, MintForClauses_t clauseList)
{

  SgExpression* orig_lower = NULL;

  bool is_canonical = isCanonicalForLoop (for_loop, NULL, & orig_lower, NULL, NULL, NULL, NULL);

  ROSE_ASSERT(is_canonical);
  ROSE_ASSERT(orig_lower);

  if(loopNo == 1 )
    {
      
      //int idx= threadIdx.x ; 
      SgAssignInitializer* init_idx =  buildAssignInitializer(buildVarRefExp(THREADIDXX, outerBlock)); 
      
      //if(isStencil)      //int idx= threadIdx.x + 1; 
      if(clauseList.chunksize.x == 1)
	init_idx = buildAssignInitializer(buildAddOp(buildVarRefExp(THREADIDXX, outerBlock), orig_lower)); 
      else
	init_idx = buildAssignInitializer(buildAddOp(buildMultiplyOp(buildVarRefExp(THREADIDXX, outerBlock),
								     buildIntVal(clauseList.chunksize.x)), orig_lower)); 
      // Declare thread id to control the loop indices 
      SgVariableDeclaration* local_index =  buildVariableDeclaration(IDX,  buildIntType(), init_idx ,outerBlock);

      SgInitializedName* local_index_name = new SgInitializedName(ASTtools::newFileInfo(), IDX, 
								 buildIntType(), init_idx, local_index,outerBlock, 0);
      ROSE_ASSERT(local_index_name);
      SgVariableSymbol* local_index_sym = new SgVariableSymbol (local_index_name);

      ROSE_ASSERT(local_index_sym);
      outerBlock->insert_symbol (IDX, local_index_sym);


      SgVariableDeclaration* blockDim_x =  buildVariableDeclaration("blockDim.x",  buildIntType(), NULL,outerBlock);
      SgVariableDeclaration* blockIdx_x =  buildVariableDeclaration("blockIdx.x",  buildIntType(), NULL,outerBlock);
           
      SgAssignInitializer* init_gidx ;
      if(clauseList.chunksize.x == 1)
	init_gidx =  buildAssignInitializer(buildAddOp(buildVarRefExp(local_index), 
						       buildMultiplyOp(buildVarRefExp(blockDim_x), 
								       buildVarRefExp(blockIdx_x))));
      else 
	init_gidx =  buildAssignInitializer(buildAddOp(buildVarRefExp(local_index), 
						       buildMultiplyOp(buildMultiplyOp(buildVarRefExp(blockDim_x), buildIntVal(clauseList.chunksize.x)), 
								       buildVarRefExp(blockIdx_x))));


      SgVariableDeclaration* index_decl =  buildVariableDeclaration("_gidx",  buildIntType(), init_gidx,outerBlock);

      appendStatement(local_index, outerBlock);    
      appendStatement(index_decl, outerBlock);
      
      return index_decl;
    }

  if(loopNo == 2)
    {
      //int idx= threadIdx.x ; 
      SgAssignInitializer* init_idy =  buildAssignInitializer(buildVarRefExp(THREADIDXY, outerBlock)); 
      
      //if(isStencil)      //int idy= threadIdx.y + 1; 
      if(clauseList.chunksize.y == 1)
	init_idy = buildAssignInitializer(buildAddOp(buildVarRefExp(THREADIDXY, outerBlock), orig_lower)); 
      else  //int _idy = threadIdx.y * UNROLL_Y + (1);
	init_idy = buildAssignInitializer(buildAddOp(buildMultiplyOp(buildVarRefExp(THREADIDXY, outerBlock),
								     buildIntVal(clauseList.chunksize.y)), 
								     orig_lower)); 

      // Declare thread id to control the loop indices 
      SgVariableDeclaration* local_index =  buildVariableDeclaration(IDY,  buildIntType(), init_idy ,outerBlock);

      SgVariableDeclaration* blockDim_y =  buildVariableDeclaration("blockDim.y",  buildIntType(), NULL,outerBlock);
      SgVariableDeclaration* blockIdx_y =  buildVariableDeclaration("blockIdx.y",  buildIntType(), NULL,outerBlock);
            
      SgAssignInitializer* init_gidy ;
      //unsigned int y = __umul24(blockIdxy , blockDim.y * UNROLL_Y) + _idy ;
      init_gidy = buildAssignInitializer(buildAddOp(buildVarRefExp(local_index), 
						    buildMultiplyOp(buildMultiplyOp(buildVarRefExp(blockDim_y),
										    buildIntVal(clauseList.chunksize.y)),
								    buildVarRefExp(blockIdx_y))));

      
      SgType* typeY =  SgTypeInt::createType();

      SgVariableDeclaration* index_decl =  buildVariableDeclaration("_gidy",  typeY, init_gidy,outerBlock);

      SgInitializedName* index_name = new SgInitializedName(ASTtools::newFileInfo(), GIDY, 
							   typeY, init_gidy, index_decl, outerBlock, 0);
      ROSE_ASSERT(index_name);
      SgVariableSymbol* index_sym = new SgVariableSymbol (index_name);


      ROSE_ASSERT(index_sym);
      outerBlock->insert_symbol (GIDY, index_sym);

      appendStatement(local_index, outerBlock);    
      appendStatement(index_decl, outerBlock);

      return index_decl; 
    }

  if(loopNo == 3)
    {
      //int idz= 0 ; 
      SgAssignInitializer* init_idz =  buildAssignInitializer(buildAddOp(buildVarRefExp(THREADIDXZ, outerBlock), orig_lower));
      
      //if(isStencil)      //int idz=  1; 
      //init_idz = buildAssignInitializer( buildIntVal(1)); 
      //global indices                                                                                                                                                                                       
      /*unsigned int blockIdxz = __float2uint_rd(blockIdx.y * invBlocksInY) ;
      unsigned int blockIdxy = blockIdx.y - blockIdxz * blocksInY ;  
      unsigned int y = blockIdxy * blockDim.y  + idy0 ;
      unsigned int z = blockIdxz  + idz;
      */

      SgVariableDeclaration* blockxy =  buildVariableDeclaration("blockIdx.y",  buildIntType(), NULL,outerBlock);

      //unsigned int blockIdxz = __float2uint_rd(blockIdx.y * invBlocksInY) ;
      SgAssignInitializer* init_blockIdxz =  buildAssignInitializer(buildMultiplyOp(buildVarRefExp(blockxy), 
										    buildVarRefExp("invBlocksInY", outerBlock)));
      SgVariableDeclaration* blockIdxz  =  buildVariableDeclaration("blockIdxz",  buildIntType(), init_blockIdxz , outerBlock);


      SgAssignInitializer* init_blockIdxy =  buildAssignInitializer(buildSubtractOp(buildVarRefExp(blockxy), 
										    buildMultiplyOp(buildVarRefExp(blockIdxz), buildVarRefExp("blocksInY", outerBlock))));

      SgVariableDeclaration* blockIdxy =  buildVariableDeclaration("blockIdxy",  buildIntType(), init_blockIdxy ,outerBlock);

      // Declare thread id to control the loop indices 
      SgVariableDeclaration* local_index =  buildVariableDeclaration(IDZ,  buildIntType(), init_idz ,outerBlock);
      

      SgVariableDeclaration* blockDim_y =  buildVariableDeclaration("blockDim.y",  buildIntType(), NULL, outerBlock);

      //y = blockIdxy * blockDim.y  + idy0 ;
      SgStatement* stmt_gidy;

      if(clauseList.chunksize.y != 1)
	stmt_gidy = buildAssignStatement(buildVarRefExp(GIDY, outerBlock), 
					 buildAddOp(buildVarRefExp(IDY,outerBlock), 
						    buildMultiplyOp(buildVarRefExp(blockIdxy), 
								    buildMultiplyOp(buildVarRefExp(blockDim_y),
										    buildIntVal(clauseList.chunksize.y))))); 
      else
	stmt_gidy = buildAssignStatement(buildVarRefExp(GIDY, outerBlock), 
					 buildAddOp(buildVarRefExp(IDY,outerBlock), 
						    buildMultiplyOp(buildVarRefExp(blockIdxy), 
								    buildVarRefExp(blockDim_y))));
								
      //unsigned int z = blockIdxz  + idz;
      SgAssignInitializer* init_gidz = NULL;

      if(clauseList.chunksize.z == 1)
	init_gidz =  buildAssignInitializer(buildAddOp(buildVarRefExp(local_index), buildMultiplyOp(buildVarRefExp(blockIdxz),
												    buildVarRefExp(BLOCKDIM_Z, outerBlock))));
      else 								 
	init_gidz =  buildAssignInitializer(buildAddOp(buildVarRefExp(local_index), buildMultiplyOp(buildVarRefExp(blockIdxz),
												    buildIntVal(clauseList.chunksize.z))));

      SgType* typeZ =  SgTypeInt::createType();

      SgVariableDeclaration* index_decl =  buildVariableDeclaration("_gidz",  typeZ, init_gidz,outerBlock);

      SgInitializedName* index_name = new SgInitializedName(ASTtools::newFileInfo(), GIDZ, 
							   typeZ, init_gidz, index_decl, outerBlock, 0);
      ROSE_ASSERT(index_name);
      SgVariableSymbol* index_sym = new SgVariableSymbol (index_name);


      ROSE_ASSERT(index_sym);
      outerBlock->insert_symbol (GIDZ, index_sym);

      appendStatement(local_index, outerBlock);    
      appendStatement(blockIdxz, outerBlock);    

      appendStatement(blockIdxy, outerBlock);    
      appendStatement(stmt_gidy, outerBlock);    
      appendStatement(index_decl, outerBlock);

      return index_decl; 
    }

  cout << "  ERROR:Mint: Nested Loop with depth "<<loopNo << " is NOT SUPPORTED " <<endl ; 
  cout << "  Please parallelize up 3 outmost loops in the nest! (Hint nest(3))" <<endl; 
  ROSE_ABORT();

  return NULL;
}



//SgBasicBlock*  LoweringToCuda::nestedForLoops(SgNode* node)
void  LoweringToCuda::nestedForLoops(SgFunctionDeclaration* func, MintForClauses_t clauseList)
{
  //1.step : get the body of the omp for loop    
  //2.step : replace the omp pragma with empty basic block 
  //3.step : query all the for loops (nested loops)
  //         there may be a nested loop, we are interested in the param.nested many of the loops 
  //         because only those are parallelizable
  //4.step : append the transformed loop into empty basic block and return the basic block
  
  SgBasicBlock* func_body = func->get_definition()->get_body();
  ROSE_ASSERT(func_body);
  SgScopeStatement* p_scope = func_body->get_scope();
  ROSE_ASSERT (p_scope != NULL);
    
  std::vector<SgForStatement* > loopNest2; 
  loopNest2= SageInterface::querySubTree<SgForStatement>(func_body,V_SgForStatement);

  SgForStatement* outMost = *(loopNest2.begin());

  bool isStencil=false;
 
  if(isStencilLoop(isSgNode(outMost)) || isBoundaryConditionLoop(outMost))
    {
      isStencil = true;
    }
  
  //there should be at least 1 loop to convert
  ROSE_ASSERT(loopNest2.size()>=1);

  SgBasicBlock * bb1 = SageBuilder::buildBasicBlock(); 
  insertStatementBefore(outMost, bb1);

  std::vector<SgForStatement* > loopNest; 
  loopNest= SageInterface::querySubTree<SgForStatement>(func_body,V_SgForStatement);

  /* add index calculations
  int _idx = threadIdx.x;
  int _gidx = _idx + blockDim.x * blockIdx.x;
  int _idx = threadIdx.x;
  int _gidx = _idx + blockDim.x * blockIdx.x;
  */

  //*for example loopNest has 4 loops but 2 of them are parallelizable*/
  int parallelizableLoopNo = 1 ;
  int unParallelLoops  = loopNest.size()- clauseList.nested; //4-2 = 2

  for (std::vector<SgForStatement* > ::reverse_iterator i = loopNest.rbegin(); i!= loopNest.rend(); i++)
    {
      //we need to skip the inner loops that are not parallelizable
      if( unParallelLoops >  0  )
	unParallelLoops--;
      else 
	{
	  SgForStatement* cur_loop = *i;
	  
	  //normalize the loop, moves index declaration outside of the loop
	  forLoopNormalization(cur_loop);

	  SgVariableDeclaration* index_decl = threadIndexCalculations(bb1,cur_loop, parallelizableLoopNo, isStencil, clauseList);	  
	  ROSE_ASSERT(index_decl);
	  SgInitializedName* index_var = getLoopIndexVariable(cur_loop);
	  
	  ROSE_ASSERT(index_var);
	  SgSymbol* sym_tmp = index_var->get_symbol_from_symbol_table();
	  ROSE_ASSERT(sym_tmp);
	  
	  replaceVariableReferences(cur_loop,
				    isSgVariableSymbol(sym_tmp),
				    getFirstVarSym(index_decl));
	  
	  parallelizableLoopNo++;
	}
    }
  
  SgForStatement* for_stmt = deepCopy(outMost);
  ROSE_ASSERT(for_stmt);

  //add the loop into the new basic block 
  appendStatement(for_stmt, bb1);
  
  std::vector<SgForStatement* > loopNest3; 
  loopNest3= SageInterface::querySubTree<SgForStatement>(bb1,V_SgForStatement);

  int loopNo3 =1 ;

  unParallelLoops  = loopNest.size()- clauseList.nested; //4-2 = 2

  //now replace for loops with if statements 
  for (std::vector<SgForStatement* > ::reverse_iterator i = loopNest3.rbegin(); i!= loopNest3.rend(); i++)
    {
      if(unParallelLoops > 0 )
	unParallelLoops--;
      else {
	SgForStatement* cur_loop = *i;

	SgBasicBlock* inner = processLoops(cur_loop, bb1, loopNo3, isStencil, clauseList);

	loopNo3++;

	ROSE_ASSERT(inner);
      }
    }

  removeStatement(outMost);     
}

SgForStatement* UnrollForMultipleElements(SgBasicBlock* bbloop1, SgForStatement* cur_loop, 
					  int unroll_factor )
{
  //a thread may compute more than 1 data element, then we should 
  //not convert all the for loops into if statements rather we should 
  //use chunksize provided by the user

  //for (_gidz = c; _gidz <= N; _gidz += 1) ) becomes

  //int upper = gidz + unroll_factor < orig_upper ? gidz + unroll_factor - 1 : orig_upper  
  //for (_gidz = _gidz; _gidz <= upper; _gidz += 1) )

  SgScopeStatement* outer_scope = cur_loop->get_scope();
  SgStatement* loop_body = cur_loop->get_loop_body();

  SgInitializedName* index_var = getLoopIndexVariable(cur_loop);
  //need to create variable for upper value
  string index_str = index_var->get_name().str();

  SgVarRefExp* var_i = buildVarRefExp(index_var, outer_scope);
  ROSE_ASSERT(var_i);

  SgExpression* loop_test = cur_loop->get_test_expr() ;
  SgExpression * orig_upper = isSgBinaryOp(loop_test)->get_rhs_operand();
  ROSE_ASSERT(orig_upper);

  //int upper = gidz + unroll_factor < orig_upper ? gidz + unroll_factor : orig_upper 
  SgExpression* condition_upper = buildLessThanOp(buildAddOp(var_i, buildIntVal(unroll_factor)),
						  copyExpression(orig_upper));

  SgExpression* upper_initVal = buildConditionalExp(condition_upper, buildAddOp(var_i, buildIntVal(unroll_factor-1)),
						    copyExpression(orig_upper));
						     
  SgInitializer* upperExp = buildAssignInitializer(upper_initVal);//buildAddOp(var_i, buildIntVal(unroll_factor)));
  ROSE_ASSERT(upperExp);

  SgVariableDeclaration* decl_upper = buildVariableDeclaration("_upper" + index_str, buildIntType(), upperExp, outer_scope);
  ROSE_ASSERT(decl_upper);  

  //add the declaration into the block surrounding loop
  appendStatement(decl_upper, bbloop1);

  SgStatement* newInitStmt = buildAssignStatement(var_i, var_i);

  ROSE_ASSERT(newInitStmt != NULL);

  SgExpression* condition = buildLessOrEqualOp(var_i, buildVarRefExp(decl_upper));

  SgStatement* newTest = buildExprStatement(condition);
							    
  ROSE_ASSERT(newTest != NULL);

  SgExpression* newIncrement = copyExpression(isSgExpression(cur_loop->get_increment()));
  SgForStatement* newForStmt = buildForStatement(newInitStmt, newTest, newIncrement, loop_body);
  ROSE_ASSERT(newForStmt);
 
  return newForStmt; 

}

SgBasicBlock* LoweringToCuda::processLoops(SgForStatement* cur_loop, SgBasicBlock* outerBlock, 
					   int loopNo, bool isStencil, MintForClauses_t clauseList)
{
     
  // Step 1. Check if the loop is canonical
  // step 2. Declare thread id to control the loop indices 
  // Step 3. Add thread id calculation 
  // Step 4. Replace loop index with the new ones 
  // Step 5. Replace for loop with if statement to check the boundaries 

    SgBasicBlock * bbloop1 = SageBuilder::buildBasicBlock();
    
    SgInitializedName * orig_index = NULL;
    SgExpression* orig_lower = NULL;
    SgExpression* orig_upper= NULL;
    SgExpression* orig_stride= NULL;
    

    // if the loop iteration space is incremental      
    bool isIncremental = true; 
    // grab the original loop 's controlling information                                                                                    
    // checks if loop index is not being written in the loop body
    bool is_canonical = isCanonicalForLoop (cur_loop, &orig_index, & orig_lower, 
					    &orig_upper, &orig_stride, NULL, &isIncremental);
    ROSE_ASSERT(is_canonical == true);
    ROSE_ASSERT(isIncremental);
    ROSE_ASSERT(orig_index);    

    if(loopNo <= 2)
      {
	//Added 14 March 2011 
	//We need to check the thread's global id < input range when we load 
	//the ghost cells. Since for becomes if, we lose the range expression for the upper bound
	//We create an variable for each upper bound and reference to that variable instead.

	string range_name = loopNo == 1 ? UPPERBOUND_X : UPPERBOUND_Y;
	ROSE_ASSERT(orig_upper);
	SgVariableDeclaration* range = buildVariableDeclaration(range_name, buildIntType(), buildAssignInitializer(copyExpression(orig_upper)), outerBlock);
	ROSE_ASSERT(range);
	prependStatement(range, outerBlock);
      }

    //cout <<"INFO-mint: Processing Loop " <<loopNo << " with index " << orig_index->get_name().str()  << endl;

    if(loopNo <= 3 ) //currently support up to 3dim 
    {
      int unroll_factor = loopNo == 3 ? clauseList.chunksize.z : clauseList.chunksize.y ;
      unroll_factor = loopNo == 1 ? clauseList.chunksize.x : unroll_factor ;

      if(unroll_factor != 1)
	{
	  //a thread may compute more than 1 data point, then we need to divide the loop 
	  //into chunk size and assign a chunk size of comptuation to a thread. 
	  SgForStatement* for_statement = UnrollForMultipleElements(bbloop1, cur_loop, unroll_factor);
	  ROSE_ASSERT(for_statement);
	  appendStatement(for_statement, bbloop1);
	}	

      else {
	//replace for loop with if statement and use loop_body as the if statement body
	SgBasicBlock* temp_bb = buildBasicBlock();
	
	SgStatement* loop_body = cur_loop->get_loop_body();
      	
	appendStatement(loop_body, temp_bb);

	ROSE_ASSERT(temp_bb);

	SgIfStmt* if_statement = buildIfStmt( buildAndOp(buildGreaterOrEqualOp(buildVarRefExp(orig_index, outerBlock),
									       copyExpression(orig_lower)), 
							 buildLessOrEqualOp(buildVarRefExp(orig_index, outerBlock),
									    copyExpression(orig_upper))) , temp_bb, NULL);
	ROSE_ASSERT(if_statement);
	appendStatement(if_statement, bbloop1);
      }
      //do not comment out this. Otherwise it will keep old loop body. 
      isSgStatement(cur_loop->get_parent())->replace_statement(cur_loop, bbloop1);
     
    }
   else 
    {
      
      SgStatement* loop_body = cur_loop->get_loop_body();
      
      //ROSE_ASSERT(newTree != NULL);
      SgStatementPtrList &init = cur_loop ->get_init_stmt();
     
      SgStatement* newInitStmt = init.front();
      //SgStatement* newInitStmt = (isSgStatement(cur_loop->get_for_init_stmt()))->front();

      ROSE_ASSERT(newInitStmt != NULL);

      SgStatement* newTest = isSgStatement(cur_loop->get_test());
      ROSE_ASSERT(newTest != NULL);

      SgExpression* newIncrement = isSgExpression(cur_loop->get_increment());

      SgForStatement* newForStmt = buildForStatement(newInitStmt, newTest, newIncrement, loop_body);

      appendStatement(newForStmt, bbloop1);

      ROSE_ASSERT(cur_loop->get_parent());
      ROSE_ASSERT(isSgStatement(cur_loop->get_parent()));

      isSgStatement(cur_loop->get_parent())->replace_statement(cur_loop, bbloop1);

    }

    return bbloop1;
  
}

