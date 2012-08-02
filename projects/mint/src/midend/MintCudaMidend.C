

#include "MintCudaMidend.h"


#include "mintTools/MintTools.h"
#include "mintPragmas/MintPragmas.h"

#include "../optimizer/CudaOptimizer.h"
#include "./mintTools/MintOptions.h"

#define DEBUG 1 

using namespace std;
using namespace OmpSupport;
using namespace SageBuilder;
using namespace SageInterface;



class visitorTraversal : public AstSimpleProcessing
{
protected:
  virtual void visit(SgNode* n);
};



void visitorTraversal::visit(SgNode* node)
{
  OmpAttributeList* attributelist = getOmpAttributeList(node);

  if (attributelist)
    attributelist->print();//debug only for now                                                                                                                         
}

//! Create a stride expression from an existing stride expression based on the loop iteration's order (incremental or decremental) 
// The assumption is orig_stride is just the raw operand of the condition expression of a loop                      
// so it has to be adjusted to reflect the real stride: *(-1) if decremental                         
#if 0                                                                   
static SgExpression* createAdjustedStride(SgExpression* orig_stride, bool isIncremental)
{
  ROSE_ASSERT(orig_stride);
  if (isIncremental)
    return copyExpression(orig_stride); // never share expressions                                                    
  else
    {
      /*  I changed the normalization phase to generate consistent incremental expressions           
       *  it should be i+= -1  for decremental loops                                                                                   
       *   no need to adjust it anymore.                                                                             
       *  */
      //      printf("Found a decremental case: orig_stride is\n");                                               
      //      cout<<"\t"<<orig_stride->unparseToString()<<endl;                                                   
      return copyExpression(orig_stride);
      //return buildMultiplyOp(buildIntVal(-1),copyExpression(orig_stride));                                         
                                                     
    }
}

#endif

void MintCudaMidend::processLoopsInParallelRegion(SgNode* parallelRegionNode, 
						 MintHostSymToDevInitMap_t hostToDevVars,
						 ASTtools::VarSymSet_t& deviceSyms, 
						 MintSymSizesMap_t& trfSizes,
						 std::set<SgInitializedName*>& readOnlyVars,
						 const SgVariableSymbol* dev_struct)
{
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(parallelRegionNode, V_SgStatement);
  Rose_STL_Container<SgNode*>::reverse_iterator nodeListIterator = nodeList.rbegin();

  for ( ;nodeListIterator !=nodeList.rend();  ++nodeListIterator)
    {
      SgStatement* node = isSgStatement(*nodeListIterator);
   
      ROSE_ASSERT(node != NULL);
      switch (node->variantT())
        {
        case V_SgOmpForStatement:
          {
#ifdef VERBOSE_2
	    cout << "  INFO:Mint: @ Line " << node->get_file_info()->get_line()  << endl;
            cout << "  Processing Omp For Statement" << endl << endl;
#endif
	    //DataTransferSizes::findTransferSizes(node, trfSizes);
	
	    bool isBoundaryCond = LoweringToCuda::isBoundaryConditionLoop(node);
	    
	    SgFunctionDeclaration* kernel;
	
	    MintForClauses_t clauseList; 

	    //kernel= LoweringToCuda::transOmpFor(node, hostToDevVars, deviceSyms, readOnlyVars,clauseList, dev_struct) ;  	    
	    kernel= LoweringToCuda::transOmpFor(node, hostToDevVars, deviceSyms, clauseList, dev_struct) ;  	    

	    //swap anyways // x swapping is buggy, need to fix that before allowing x as well
	    if(clauseList.chunksize.x == 1 && ( clauseList.chunksize.z != 1 || clauseList.chunksize.y != 1 ))
	      {
		//if(MintOptions::GetInstance()->isSwapOpt())
		CudaOptimizer::swapLoopAndIf(kernel, clauseList);
	      }
	    if (!isBoundaryCond && MintOptions::GetInstance()->optimize())
	      //if (MintOptions::GetInstance()->optimize())
	      {
		cout << "\n\n  INFO:Mint: Optimization is ON. Optimizing ...\n\n" ;

		CudaOptimizer::optimize(kernel, clauseList);
	      } 
	    //  MintTools::printAllStatements(isSgNode(kernel));
	    //MintArrayInterface::linearizeArrays(kernel);
	    break;
          }
	default:
	  {
	    //cout << "  INFO:Mint: @ Line " << node->get_file_info()->get_line()  << endl;
	    //cout << "  Currently we only handle for loops" << endl << endl;
	    
	    //do nothing
	    //currently we only handle for loops
	    break;
	  }
	}
    }
  
  
  for (ASTtools::VarSymSet_t::const_iterator i = deviceSyms.begin (); i!= deviceSyms.end (); ++i)
    {     
      SgVariableSymbol* sym= const_cast<SgVariableSymbol*> (*i);

      SgInitializedName* name = sym->get_declaration();
      SgType* type = name->get_type();
     
      if(isSgArrayType(type) || isSgPointerType(type)){
	
	//Check if is of the fields of the struct
	if(hostToDevVars.find(sym) == hostToDevVars.end())
	  {
	    string name_str = name->get_name().str();
	    
	    cerr << "  ERR:Mint: Ooops! Did you forget to insert a copy pragma for the variable ("<< name_str << ") ?"<< endl;
	    cerr << "  ERR:Mint: Please insert the copy pragma and compile again "<< endl;
	    cerr << "  INFO:Mint: Note that copy pragmas should appear right before and after a parallel region" << endl;
	    ROSE_ABORT();
	  }
      }
      }
}

SgVariableSymbol* MintCudaMidend::packArgsInStruct(SgNode* node, const MintHostSymToDevInitMap_t hostToDevVars)
{
  //Added 14 March 2011 
  //(e.g. earthquake simulation has so many parameters to pass to a function)
  //Each cudaPitchedPtr occupies 32 btyes and the limit of kernel params is 256 bytes. 
  //we allow up to 5 cudaPitchedPtr without structure declaration, (the rest is reserved for scalar val)
  //if the vector variables are more than 5 than we need to declare a structure and send the structure pointer to the kernel

  if(hostToDevVars.size() <= 6 )
    return NULL;
  
  //we are using a struct to handle the vectors
  Outliner::useStructureWrapper = true; 

  ROSE_ASSERT(node != NULL);  
  SgOmpClauseBodyStatement* target = isSgOmpClauseBodyStatement(node);
  ROSE_ASSERT (target != NULL);

  SgScopeStatement* scope = target->get_scope();
  ROSE_ASSERT (scope != NULL);

  SgOmpParallelStatement* targetStatement = isSgOmpParallelStatement(node);
  ROSE_ASSERT (targetStatement != NULL);

  SgBasicBlock * body =  isSgBasicBlock(targetStatement->get_body());
  ROSE_ASSERT(body != NULL);

  ASTtools::VarSymSet_t syms;
  ASTtools::VarSymSet_t psyms;

  string func_name =  MintTools::generateArgName(target);
  SgGlobal* g_scope = getGlobalScope(target);

  MintHostSymToDevInitMap_t::const_iterator it;

  for(it= hostToDevVars.begin(); it != hostToDevVars.end(); it++)
    {
      //get the device name we used for the array 
      SgInitializedName* i_name = it -> second;
      ROSE_ASSERT(i_name);

      SgScopeStatement* i_scope = i_name -> get_scope();
      ROSE_ASSERT(i_scope);

      SgVariableSymbol *dev_sym = i_scope -> lookup_var_symbol(i_name->get_name());
      ROSE_ASSERT(dev_sym);

      SgType* type = dev_sym->get_type();
      ROSE_ASSERT(type);
          
      syms.insert(dev_sym);
    }

  SgClassDeclaration* struct_decl = Outliner::generateParameterStructureDeclaration(body, func_name, syms, psyms, g_scope); 
  ROSE_ASSERT(struct_decl);

  string wrapper_name= Outliner::generatePackingStatements(target, syms, psyms, struct_decl);

  Outliner::useStructureWrapper = false; 

  SgVariableDeclaration* dev_decl = CudaMemoryManagement::issueTransferForArrayStruct(target, wrapper_name);

  SgVariableSymbol* dev_struct_sym = getFirstVarSym(dev_decl);
  ROSE_ASSERT(dev_struct_sym);

  return dev_struct_sym;
}

/*************************************************************************************************/
/*                   replace the parallel region with cuda kernel functions                      */
/*************************************************************************************************/
void MintCudaMidend::transOmpParallel(SgNode* node,
				     MintHostSymToDevInitMap_t& hostToDevVars)
{
  ROSE_ASSERT(node != NULL);
  SgOmpParallelStatement* targetStatement = isSgOmpParallelStatement(node);
  ROSE_ASSERT (targetStatement != NULL);

  SgStatement * body =  targetStatement->get_body();
  ROSE_ASSERT(body != NULL);

  //Save the preprocessing info before and after the target statement 
  AttachedPreprocessingInfoType save_buf_before, save_buf_after;
  cutPreprocessingInfo(targetStatement, PreprocessingInfo::before, save_buf_before) ;
  cutPreprocessingInfo(targetStatement, PreprocessingInfo::after , save_buf_after ) ;

  ASTtools::VarSymSet_t deviceSyms;  //scalar and vector variables (need to transfered to the gpu)
  ASTtools::VarSymSet_t privateSyms; //local variables, no need to transfer 
  //ASTtools::VarSymSet_t sharedSyms;  //shared variables: same as device but they have the original host name 

  //Initialize outliner 
  //We do not wrap parameters with struct for outlining
  //we use a struct for the vector variables excluding scalar vars. 
  Outliner::useParameterWrapper = false;
  Outliner::useStructureWrapper = false; //use classic
  Outliner::enable_debug = true; 

  //if there are several arrays to pass then we pack them into a struct 
  SgVariableSymbol* dev_struct_sym = packArgsInStruct(node, hostToDevVars);

  MintSymSizesMap_t trfSizes;
  std::set<SgInitializedName*> readOnlyVars;

  //Search all the pragma omp for loops in the parallel region 
  //Convert them into a kernel and replace with a kernel launch
  MintCudaMidend::processLoopsInParallelRegion(node, hostToDevVars, deviceSyms, trfSizes, readOnlyVars, dev_struct_sym); 

  //this function creates device pointers on the host, allocate memory on the dev and issues memory transfer 
  //outside of the parallel region
  //if the data transferred is asked, then this function only frees them at the end of the parallel region
  //CudaMemoryManagement::handleCudaHostVars(node, deviceSyms, readOnlyVars,  trfSizes, hostToDevVars);

  //In body of the parallel region, we need to replace the host variables
  //with device variables if they are pointer swapping. 
  //TODO: In other cases such IO or etc. we need to copy the data back the updated
  //value from the device

  CudaMemoryManagement::postTransformationVariableHandling(node, deviceSyms, hostToDevVars);

  //remove the parallel omp statement and replace it with the modified body
  SageInterface::replaceStatement(targetStatement, body, true);

  // have to use cut-paste instead of direct move since                                                    
  // the preprocessing information may be moved to a wrong place during outlining                          
  // and the destination node is unknown until the outlining is done.                                             
  pastePreprocessingInfo(body, PreprocessingInfo::before, save_buf_before);                                        
  pastePreprocessingInfo(body, PreprocessingInfo::after, save_buf_after);

}

int MintCudaMidend::insertHeaders(SgProject* project)
{

  //cout << "  INFO:Mint: Inserting runtime header cutil.h" << endl << endl ;

  Rose_STL_Container<SgNode*> globalScopeList = NodeQuery::querySubTree (project,V_SgGlobal);

  for (Rose_STL_Container<SgNode*>::iterator i = globalScopeList.begin(); i != globalScopeList.end(); i++)
    {
      SgGlobal* globalscope = isSgGlobal(*i);
      ROSE_ASSERT (globalscope != NULL);
      //SageInterface::insertHeader("cutil.h",PreprocessingInfo::after,false,globalscope);
    }
  return 0; //assume always successful currently                                                                                             
}


void MintCudaMidend::mintPragmasFrontendProcessing(SgSourceFile* file)
{
  
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(file, V_SgPragmaDeclaration);
  Rose_STL_Container<SgNode*>::iterator nodeListIterator = nodeList.begin();

  for ( ;nodeListIterator !=nodeList.end();  ++nodeListIterator)
    {
      SgPragmaDeclaration* node = isSgPragmaDeclaration(*nodeListIterator);
      ROSE_ASSERT(node != NULL);
      switch (node->variantT())
	{
	case V_SgPragmaDeclaration:
	  {
	    //soley checks syntax of mint pragmas 
	    MintPragmas::isMintPragma(node);
	    break;
	  }
        default:
          break;
	}
    }
}

/*
  We already check if the pragmas are inserted correctly. 
  e.g. all the copy pragmas are either right after/before 
  a prallel region or they are inside of a parallel region. 

  This function just separates copies inside of a parallel 
  and outsides of a parallel region. 

*/

void MintCudaMidend::processDataTransferPragmas(SgNode* parallel_reg,
					       MintHostSymToDevInitMap_t& hostToDevVars)
{
  //1. first handle the copy pragmas preceeding the parallel region  
  //2. then handle the pragmas inside of the parallel region 
  //3. second handle the copy pragmas proceeding the parallel region

  //1. first handle the copy pragmas preceeding the parallel region  
  SgStatement* prev = getPreviousStatement(isSgStatement(parallel_reg));
  vector <SgStatement*> copyList;

  while(prev != NULL)
    {
      SgPragmaDeclaration* pragma = isSgPragmaDeclaration(prev);

      if(pragma != NULL && MintPragmas::isTransferToFromDevicePragma(pragma)){
	  copyList.push_back(prev);
	}
	else 
	  break;
      prev = getPreviousStatement(prev); //continue processing copy pragmas
    }//end of while 

  for(vector<SgStatement*>::reverse_iterator it=copyList.rbegin() ; it!= copyList.rend(); it++ )
    {
      SgPragmaDeclaration* pragma = isSgPragmaDeclaration(*it);
      MintTrfParams_t params = MintPragmas::getTransferParameters(pragma, hostToDevVars);
	
      if(params.trfType == MINT_DATA_TRF_TO_DEV_PRAGMA){
	CudaMemoryManagement::issueDataTransferToDevice(pragma, params, hostToDevVars);
#ifdef VERBOSE_2
	  cout << "  INFO:Mint: @ Line "  << pragma->get_file_info()->get_line() << endl
	       << "  Issue Data Transfers to Device " << "(dest: "<< params.dst << " ,src: "<< params.src << ")"<< endl << endl;
#endif
      }
      /*
      else 
	{
	  cout << "  WARNING:Mint: @ Line "  << pragma->get_file_info()->get_line() << endl
	       << "  Expecting a copy to device..." << endl 
	       << "  Want to merge two parallel regions? or change the transfer type?" << endl << endl;

	}
      */
    }

  //this is the INSIDE list 
  //2.step then handle the pragmas inside of the parallel region 
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(parallel_reg, V_SgPragmaDeclaration);
  Rose_STL_Container<SgNode*>::iterator nodeListIterator = nodeList.begin();

  for ( ;nodeListIterator !=nodeList.end();  ++nodeListIterator)
    {
      SgPragmaDeclaration* node = isSgPragmaDeclaration(*nodeListIterator);
      ROSE_ASSERT(node != NULL);
    
      if(MintPragmas::isTransferToFromDevicePragma(node)){	     

	bool inside = true; 

	MintTrfParams_t params = MintPragmas::getTransferParameters(node, hostToDevVars);
	
	if(params.trfType == MINT_DATA_TRF_TO_DEV_PRAGMA){

	  CudaMemoryManagement::issueDataTransferToDevice(node, params, hostToDevVars, inside);
#ifdef VERBOSE_2
	  cout << "  INFO:Mint: @ Line "  << node->get_file_info()->get_line() << endl
	       << "  Issue Data Transfers to Device " << "(dest: "<< params.dst << " ,src: "<< params.src << ")"<< endl << endl;
#endif
	}
	else if (params.trfType == MINT_DATA_TRF_FROM_DEV_PRAGMA){
	  CudaMemoryManagement::issueDataTransferFromDevice(node, params, hostToDevVars, inside);
#ifdef VERBOSE_2
	  cout << "  INFO:Mint: @ Line "  << node->get_file_info()->get_line() << endl
	       << "  Issue Data Transfers to Host " << "(dest: "<< params.dst << " ,src: "<< params.src << ")"<< endl << endl;
#endif
	}
	else 
	  {
	    
	    cout << "  ERROR:Mint: @ Line "  << node->get_file_info()->get_line() << endl
		 << "  Unrecognized transfer type " << endl << endl;
	    ROSE_ABORT();
	  }
      }
    }

  //3. step  handle the copy pragmas proceeding the parallel region
  SgStatement* post = getNextStatement(isSgStatement(parallel_reg));
  copyList.clear();

  while(post != NULL)
    {
      SgPragmaDeclaration* pragma = isSgPragmaDeclaration(post);

      if(pragma != NULL && MintPragmas::isTransferToFromDevicePragma(pragma)){
	  copyList.push_back(post);
	}
	else 
	  break;
      post = getNextStatement(post); //continue processing copy pragmas

    }//end of while 


  //here we want to free the allocated memory for all the hostToDevVars vars
  //post statement is where we are going to free them.
  CudaMemoryManagement::freeParallelRegionVars(post,  hostToDevVars);

  //We want to keep a list of variables that we freed 
  //because next step (step 3) may add new variables to free
  //We will free them afterwords

  MintHostSymToDevInitMap_t hostToDevVars_tmp; 
  for(MintHostSymToDevInitMap_t::iterator it = hostToDevVars.begin() ; it != hostToDevVars.end() ; it++ )
    {
      SgVariableSymbol* host_sym = it->first ; 
      SgInitializedName* dev_name = it->second;

      hostToDevVars_tmp [host_sym] = dev_name;
    }


  for(vector<SgStatement*>::iterator it=copyList.begin() ; it!= copyList.end(); it++ )
    {
      SgPragmaDeclaration* pragma = isSgPragmaDeclaration(*it);
      MintTrfParams_t params = MintPragmas::getTransferParameters(pragma, hostToDevVars);
	
      if(params.trfType == MINT_DATA_TRF_FROM_DEV_PRAGMA){
	CudaMemoryManagement::issueDataTransferFromDevice(pragma, params, hostToDevVars);
#ifdef VERBOSE_2
	  cout << "  INFO:Mint: @ Line "  << pragma->get_file_info()->get_line() << endl
	       << "  Issue Data Transfers form Device " << "(dest: "<< params.dst << " ,src: "<< params.src << ")"<< endl << endl;
#endif
      }
      /*      else 
	{
	  cout << "  WARNING:Mint: @ Line "  << pragma->get_file_info()->get_line() << endl
	       << "  Expecting a copy to device..." << endl 
	       << "  Want to merge two parallel regions? or change the transfer type?" << endl << endl;
	}
      */
    }

  MintHostSymToDevInitMap_t hostToDevVars_freeList; 
  for(MintHostSymToDevInitMap_t::iterator it = hostToDevVars.begin() ; it != hostToDevVars.end() ; it++ )
    {
      
      //this is new variable, need to free this one
      SgVariableSymbol* host_sym = it->first ; 
      
      if(hostToDevVars_tmp.find(host_sym) == hostToDevVars_tmp.end() )
	{
	  SgInitializedName* dev_name = it->second;
	  
	  hostToDevVars_freeList [host_sym] = dev_name;
	}
    }

  //here we want to free the allocated memory for all the hostToDevVars vars
  //post statement is where we are going to free them.
  CudaMemoryManagement::freeParallelRegionVars(post,  hostToDevVars_freeList);

}

/*
void MintCudaMidend::processDataTransferPragmas(SgNode* parallel_node,
					       MintHostSymToDevInitMap_t& hostToDevVars)
{
  
  Rose_STL_Container<SgNode*> nodeList = getListOfCopyPragmas(parallel_node);
  //NodeQuery::querySubTree(parallel_node, V_SgPragmaDeclaration);

  Rose_STL_Container<SgNode*>::iterator nodeListIterator = nodeList.begin();

  for ( ;nodeListIterator !=nodeList.end();  ++nodeListIterator)
    {
      SgPragmaDeclaration* node = isSgPragmaDeclaration(*nodeListIterator);
      ROSE_ASSERT(node != NULL);
      switch (node->variantT())
	{
	case V_SgPragmaDeclaration:
	  {
	    if(MintPragmas::isTransferToFromDevicePragma(node)){	     

	      MintTrfParams_t params = MintPragmas::getTransferParameters(node, hostToDevVars);
	      
	      if(params.trfType == MINT_DATA_TRF_TO_DEV_PRAGMA){
		CudaMemoryManagement::issueDataTransferToDevice(node, params, hostToDevVars);
#ifdef VERBOSE_2
		cout << "  INFO:Mint: @ Line "  << node->get_file_info()->get_line() << endl
		     << "  Issue Data Transfers to Device " << "(dest: "<< params.dst << " ,src: "<< params.src << ")"<< endl << endl;
#endif VERBOSE_2
	      }
	      else if (params.trfType == MINT_DATA_TRF_FROM_DEV_PRAGMA){
	     	CudaMemoryManagement::issueDataTransferFromDevice(node, params, hostToDevVars);
#ifdef VERBOSE_2
		cout << "  INFO:Mint: @ Line "  << node->get_file_info()->get_line() << endl
		     << "  Issue Data Transfers to Host " << "(dest: "<< params.dst << " ,src: "<< params.src << ")"<< endl << endl;
#endif VERBOSE_2
	      }
	      else 
		{
		  cout << "  ERROR:Mint: @ Line "  << node->get_file_info()->get_line() << endl
		       << "  Unrecognized transfer type " << endl << endl;
		  ROSE_ABORT();
		}
	    }
	    break;
	  }
	}
    }
}
*/

  //! Simply replace the pragma with a function call to __sync_synchronize ();
void MintCudaMidend::transOmpFlushToCudaBarrier(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpFlushStatement* target = isSgOmpFlushStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );

    SgExprStatement* func_call_stmt = buildFunctionCallStmt("cudaThreadSynchronize", buildVoidType(), NULL, scope);
    
    replaceStatement(target, func_call_stmt, true);

  }

//! Simply replace the pragma with a function call to void GOMP_barrier (void);                                                                                                                              
void MintCudaMidend::transOmpBarrierToCudaBarrier(SgNode * node)
{
  ROSE_ASSERT(node != NULL );
  SgOmpBarrierStatement* target = isSgOmpBarrierStatement(node);
  ROSE_ASSERT(target != NULL );
  SgScopeStatement * scope = target->get_scope();
  ROSE_ASSERT(scope != NULL );

  SgExprStatement* func_call_stmt = buildFunctionCallStmt("cudaThreadSynchronize", buildVoidType(), NULL, scope);

  replaceStatement(target, func_call_stmt, true);
}


//! Simply replace the pragma with a function call to void GOMP_barrier (void);                                                                                                                              
void MintCudaMidend::transOmpMaster(SgNode * node)
{
  ROSE_ASSERT(node != NULL );
  SgOmpMasterStatement* target = isSgOmpMasterStatement(node);
  ROSE_ASSERT(target != NULL );
  SgScopeStatement * scope = target->get_scope();
  ROSE_ASSERT(scope != NULL );

  SgStatement* body = target->get_body();
  bool isLast = isLastStatement(target); // check this now before any transformation    

  SgIfStmt* if_stmt = buildIfStmt(buildIntVal(1), body, NULL);

  replaceStatement(target, if_stmt,true);
  moveUpPreprocessingInfo (if_stmt, target, PreprocessingInfo::before);


  if (isLast) 
// the preprocessing info after the last statement may be attached to the inside of its parent scope                                       
    {
      // cout<<"Found a last stmt. scope is: "<<scope->class_name()<<endl;                                                    
      // dumpPreprocInfo(scope);                                                                                                         
      // move preprecessing info. from inside position to an after position                                                
      moveUpPreprocessingInfo (if_stmt, scope, PreprocessingInfo::inside, PreprocessingInfo::after);
    }

}


void MintCudaMidend::transOmpSingle(SgNode * node)
{
    //TODO: we need to check if the loop body becomes a cuda kernel or not. 

    ROSE_ASSERT(node != NULL );
    SgOmpSingleStatement* target = isSgOmpSingleStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );

    SgStatement* body = target->get_body();
    ROSE_ASSERT(body!= NULL );

    SgIfStmt* if_stmt = buildIfStmt(buildIntVal(1), body, NULL);

    replaceStatement(target, if_stmt,true);
    //SgBasicBlock* true_body = ensureBasicBlockAsTrueBodyOfIf (if_stmt);

    //transOmpVariables(target, true_body, func_exp);
    // handle nowait 
    if (!hasClause(target, V_SgOmpNowaitClause))
    {
      SgExprStatement* barrier_call = buildFunctionCallStmt("cudaThreadSynchronize", buildVoidType(), NULL, scope);
      insertStatementAfter(if_stmt, barrier_call);
    }
}


void MintCudaMidend::replaceMintParallelWithOmpParallel(SgSourceFile* file)
{
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(file, V_SgPragmaDeclaration);
  Rose_STL_Container<SgNode*>::reverse_iterator nodeListIterator = nodeList.rbegin();

  for ( ;nodeListIterator !=nodeList.rend();  ++nodeListIterator)
    {
      SgPragmaDeclaration* node = isSgPragmaDeclaration(*nodeListIterator);
      ROSE_ASSERT(node != NULL);

      //checks if the syntax is correct and the parallel region is followed by 
      //a basic block
      if(MintPragmas::isParallelRegionPragma(node))
	{
	  SgStatement* next = getNextStatement(node);
	  ROSE_ASSERT(next);
	  
	  if(isSgBasicBlock(next))
	    {
	      removeStatement(next);

	      SgOmpParallelStatement* omp_stmt = new SgOmpParallelStatement(NULL, next);
	      setOneSourcePositionForTransformation(omp_stmt);
	      next->set_parent(omp_stmt);

	      replaceStatement(node, omp_stmt);
	    }
	}
    }
}


//note that we keep mint for pragma as well 
//#pragma mint for
//#pragma omp for
void MintCudaMidend::replaceMintForWithOmpFor(SgSourceFile* file)
{
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(file, V_SgPragmaDeclaration);
  Rose_STL_Container<SgNode*>::reverse_iterator nodeListIterator = nodeList.rbegin();

  for ( ;nodeListIterator !=nodeList.rend();  ++nodeListIterator)
    {
      SgPragmaDeclaration* node = isSgPragmaDeclaration(*nodeListIterator);
      ROSE_ASSERT(node != NULL);

      //checks if the syntax is correct and the parallel region is followed by 
      //a basic block
      if(MintPragmas::isForLoopPragma(node))
	{
	  SgStatement* loop = getNextStatement(node);
	  ROSE_ASSERT(loop);
	  
	  if(isSgForStatement(loop))
	    {
	      removeStatement(loop);

	      SgOmpForStatement* omp_stmt = new SgOmpForStatement(NULL, loop);
	      setOneSourcePositionForTransformation(omp_stmt);
	      loop->set_parent(omp_stmt);

	      insertStatementAfter(node, omp_stmt);

	    }
	}
    }
}

void MintCudaMidend::lowerMinttoCuda(SgSourceFile* file)
{
  //Searches mint pragmas, and performs necessary transformation
  //We also check the mint pragmas syntactically 
  //At this point, we only care parallel regions and for loops
  //But do not process forloops at this point (note that this is bottom-up)
  //We process forloops when we see a parallel region pragma because they are always
  //inside of a parallel region.
  //TODO: Sometimes a forloop is merged with a parallel region. Need to handle these.

  ROSE_ASSERT(file != NULL);

  //replaces all the occurrences of mint parallel with omp parallel 
  replaceMintParallelWithOmpParallel(file);
  replaceMintForWithOmpFor(file);

  //adds the private and first private into private clause explicitly 
  patchUpPrivateVariables(file);  //uses ROSE's
  patchUpFirstprivateVariables(file); //uses ROSE's

  //insert openmp specific headers
  //insertRTLHeaders(file);

  //check if mint pragma declarations are correct
  mintPragmasFrontendProcessing(file);

  //the map has the mapping from the host variables to device variables
  //where we copy the data
  
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(file, V_SgStatement);
  Rose_STL_Container<SgNode*>::reverse_iterator nodeListIterator = nodeList.rbegin();

  for ( ;nodeListIterator !=nodeList.rend();  ++nodeListIterator)
    {
      SgStatement* node = isSgStatement(*nodeListIterator);
      ROSE_ASSERT(node != NULL);
      switch (node->variantT())
	{
        case V_SgOmpParallelStatement:
          {
	    //first we handle data transfer pragmas
	    MintHostSymToDevInitMap_t hostToDevVars;
	    processDataTransferPragmas(node, hostToDevVars);
#ifdef VERBOSE_2
	    cout << "  INFO:Mint: @ Line " << node->get_file_info()->get_line()  << endl;
	    cout << "  Processing Mint Parallel Statement" << endl << endl;
#endif
	    MintCudaMidend::transOmpParallel(node, hostToDevVars);
            break;
          }
        case V_SgOmpTaskStatement:
          {
            //transOmpTask(node);
            break;
          }
        case V_SgOmpForStatement:
          {
	    //cout << "INFO-mint: Omp For Statement (skipped processing it)" << endl;
	    //LoweringToCuda::transOmpFor(node);
	    //OmpSupport::transOmpFor(node);
            break;
          }
        case V_SgOmpBarrierStatement:
          {
#ifdef VERBOSE_2
	    cout << "  INFO:Mint: @ Line " << node->get_file_info()->get_line()  << endl;
	    cout << "  Processing Omp Barrier Statement" << endl;
#endif
            transOmpBarrierToCudaBarrier(node);
            break;
          }
        case V_SgOmpFlushStatement:
          {
	    cout << "  INFO:Mint: Processing Omp Flush Statement" << endl;
	    transOmpFlushToCudaBarrier(node);
            break;
          }
        case V_SgOmpThreadprivateStatement:
          {
            //transOmpThreadprivate(node);
            break;
          }
	case V_SgOmpTaskwaitStatement:
          {
            //transOmpTaskwait(node);
            break;
          }
        case V_SgOmpSingleStatement:
          {
	    //TODO: we need to check if the loop body becomes a cuda kernel or not. 
	    MintCudaMidend::transOmpSingle(node);
            break;
          }
	case V_SgOmpMasterStatement:
          {
	    //TODO: we need to check if the loop body becomes a cuda kernel or not. 
	    MintCudaMidend::transOmpMaster(node);
            break;
          }
	case V_SgOmpAtomicStatement:
          {
            //transOmpAtomic(node);
            break;
          }
	case V_SgOmpOrderedStatement:
          {
            //transOmpOrdered(node);
            break;
          }
	case V_SgOmpCriticalStatement:
          {
            //transOmpCritical(node);
            break;
          }
        default:
          {
            //This is any other statement in the source code which is not omp pragma
            //cout<< node->unparseToString()<<" at line:"<< (node->get_file_info())->get_line()<<endl;
            // do nothing here    
          }
	}// switch
    } 
#if 0
  //3. Special handling for files with main() 
  // rename main() to user_main()
  SgFunctionDeclaration * mainFunc = findMain(cur_file);
  if (mainFunc) 
    {
      renameMainToUserMain(mainFunc);
    }
#endif

}
