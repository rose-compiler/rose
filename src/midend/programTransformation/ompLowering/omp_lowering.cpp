
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include "Outliner.hh"
#include "omp_lowering.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

// This is a hack to pass the number of CUDA loop iteration count around
// When translating "omp target" , we need to calculate the number of thread blocks needed.
// To do that, we need to know how many CUDA threads are needed.
// We think the number of CUDA threads is the iteration count of the parallelized CUDA loop (peeled off), assuming increment is always 1
//TODO  Also, the incremental value should be irrevelvant?
// The loop will be transformed away when we call transOmpTargtLoop since we use bottom-up translation
// So the loop iteration count needs to be stored globally before transOmpTarget() is called. 
static SgExpression* cuda_loop_iter_count_1 = NULL;

// this is another hack to pass the reduction variables for accelerator model directives
// We use bottom-up translation for AST with both omp parallel and omp for.
// reduction is implemented using a two level reduction method: inner thread block level + beyond block level
// We save the per-block variable and its reduction type integer into a map when generating inner block level reduction.
// We use the map to help generate beyond block level reduction
static std::map<SgVariableSymbol* , int> per_block_reduction_map;

// we don't know where to insert the declarations when they are generated as part of transOmpTargetLoop
// we have to save them and insert them later when kernel launch statement is generated as part of transOmpTargetParallel
static std::vector<SgVariableDeclaration*> per_block_declarations;

static std::map<string , std::vector<SgExpression*> > offload_array_offset_map;
static std::map<string , std::vector<SgExpression*> > offload_array_size_map;

// Liao 1/23/2015
// when translating mapped variables using xomp_deviceDataEnvironmentPrepareVariable(), the original variable reference will be used as
// a parameter. 
// However, later replaceVariablesWithPointerDereference () will find it and replace it with a device version reference, which is not desired.
// In order to avoid this, we keep track of these few references to the original Host CPU side variables and don't replace them later on.
// This may not be elegant, but let's get something working first.
static set<SgVarRefExp* > preservedHostVarRefs; 

#define ENABLE_XOMP 1  // Enable the middle layer (XOMP) of OpenMP runtime libraries
  //! Generate a symbol set from an initialized name list, 
  //filter out struct/class typed names
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

namespace OmpSupport
{ 
  omp_rtl_enum rtl_type = e_gomp; /* default to  generate code targetting gcc's gomp */
  bool enable_accelerator = false; /* default is to not recognize and lowering OpenMP accelerator directives */

  // A flag to control if device data environment runtime functions are used to automatically manage data as much as possible.
  // instead of generating explicit data allocation, copy, free functions. 
  bool useDDE = true; 

  unsigned int nCounter = 0;
  //------------------------------------
  // Add include "xxxx.h" into source files, right before the first statement from users
  // Lazy approach: assume all files will contain OpenMP runtime library calls
  // TODO: (low priority) a better way is to only insert Headers when OpenMP is used.
  // 2/1/2008, try to use MiddleLevelRewrite to parse the content of the header, which
  //  should generate function symbols used for runtime function calls 
  //  But it is not stable!

  //! This makeDataSharingExplicit() is added by Hongyi on July/23/2012. 
  //! Consider private, firstprivate, lastprivate, shared, reduction  is it correct?@Leo
  //TODO: consider the initialized name of variable in function call or definitions  

  /** Algorithm for patchUpSharedVariables edited by Hongyi Ma on August 7th 2012
   *   1. find all variables references in  parallel region
   *   2. find all varibale declarations in this parallel region
   *   3. check whether these variables has been in private or shared clause already
   *   4. if not, add them into shared clause
   */

  //! function prototypes for  patch up shared variables 

  /*    Get name of varrefexp  */
  string getName( SgNode * n )
  {
    string name;
    SgVarRefExp * var = isSgVarRefExp( n );
    if ( var )
      name = var->get_symbol()->get_name().getString();

    return name;
  }

  /*    Remove duplicate list entries  */
  void getUnique( Rose_STL_Container< SgNode* > & list )
  {
    Rose_STL_Container< SgNode* >::iterator start = list.begin();
    unsigned int size = list.size();
    unsigned int i, j;

    if ( size > 1 )
    {
      for ( i = 0; i < size - 1; i++ )
      {
        j = i + 1;
        while ( j < size )
        {
          SgVarRefExp* iis = isSgVarRefExp( list.at(i) );
          SgVarRefExp* jjs = isSgVarRefExp( list.at(j) );

          SgInitializedName* is = isSgInitializedName( iis->get_symbol()->get_declaration() );
          SgInitializedName* js = isSgInitializedName( jjs->get_symbol()->get_declaration() );
          if ( is == js )
          {
            list.erase( start + j );
            size--;
            continue;
          }

          j++;
        }
      }
    }
  }
  /* the end of getUnique name */

  /* gather varaible references from remaining expressions */


  void gatherReferences( const Rose_STL_Container< SgNode* >& expr, Rose_STL_Container< SgNode* >& vars)
  {
    Rose_STL_Container< SgNode* >::const_iterator iter = expr.begin();

    while (iter != expr.end() )
    {

      Rose_STL_Container< SgNode* > tempList = NodeQuery::querySubTree(*iter, V_SgVarRefExp );

      Rose_STL_Container< SgNode* >::iterator ti = tempList.begin();
      while ( ti != tempList.end() )
      {
        vars.push_back( *ti );
        ti++;
      }
      iter++;


    }
    /* then remove the duplicate variables */

    getUnique( vars );



  } 

  /* the end of gatherReferences function*/

  //! end of function prototypes for patch up shared variables

  //! patch up all variables to make them explicit in data-sharing explicit 

  int patchUpSharedVariables(SgFile* file)
  {


    int result = 0; // record for the number of shared variables added

    ROSE_ASSERT( file != NULL );
    Rose_STL_Container< SgNode* > allParallelRegion = NodeQuery::querySubTree( file, V_SgOmpParallelStatement );
    Rose_STL_Container< SgNode* >::iterator  allParallelRegionItr = allParallelRegion.begin();

    for( ; allParallelRegionItr != allParallelRegion.end(); allParallelRegionItr++ )
    {
      //! gather all expressions statements
      Rose_STL_Container< SgNode* > expressions = NodeQuery::querySubTree( *allParallelRegionItr, V_SgExprStatement );
      //! store all variable references
      Rose_STL_Container< SgNode* > allRef;   
      gatherReferences(expressions, allRef );

      //!find all local variables in parallel region 
      Rose_STL_Container< SgNode* > localVariables = NodeQuery::querySubTree( *allParallelRegionItr, V_SgVariableDeclaration );

      //! check variables are not local, not variables in clauses already

      Rose_STL_Container< SgNode* >::iterator allRefItr = allRef.begin();
      while( allRefItr != allRef.end() )
      {
        SgVarRefExp* item = isSgVarRefExp( *allRefItr );
        string varName = item->get_symbol()->get_name().getString();

        Rose_STL_Container< SgNode* >::iterator localVariablesItr = localVariables.begin();

        bool isLocal = false; // record whether this variable should be added into shared clause

        while( localVariablesItr != localVariables.end() )
        {
          SgInitializedNamePtrList vars = ((SgVariableDeclaration*)(*localVariablesItr))->get_variables();

          string localName = vars.at(0)->get_name().getString();
          if( varName == localName )
          {
            isLocal = true;
          }             
          localVariablesItr++;

        }

        bool isInPrivate = false;
        SgInitializedName* reg = isSgInitializedName( item->get_symbol()->get_declaration() );

        isInPrivate = isInClauseVariableList( reg, isSgOmpClauseBodyStatement( *allParallelRegionItr ), V_SgOmpPrivateClause);

        bool isInShared = false;

        isInShared = isInClauseVariableList( reg, isSgOmpClauseBodyStatement( *allParallelRegionItr ), V_SgOmpSharedClause );

        bool isInFirstprivate = false;

        isInFirstprivate = isInClauseVariableList( reg, isSgOmpClauseBodyStatement( *allParallelRegionItr ), V_SgOmpFirstprivateClause );

        bool isInReduction = false;

        isInReduction = isInClauseVariableList( reg, isSgOmpClauseBodyStatement( *allParallelRegionItr ), V_SgOmpReductionClause );

        if( !isLocal && !isInShared && !isInPrivate && !isInFirstprivate && ! isInReduction )
        {
          std::cout<<" the insert variable is: "<<item->unparseToString()<<std::endl;
          addClauseVariable( reg, isSgOmpClauseBodyStatement( * allParallelRegionItr ), V_SgOmpSharedClause );
          result++; 
          std::cout<<" successfully !"<<std::endl;
        }
        allRefItr++;                 
      }          


    } // end of all parallel region  

    return result;
  } // the end of patchUpSharedVariables()











  //! make all data-sharing attribute explicit 

  int makeDataSharingExplicit(SgFile* file)
  {
    int result = 0; // to record the number of varbaile added 
    ROSE_ASSERT(file != NULL );

    int p = patchUpPrivateVariables(file); // private variable first

    int f = patchUpFirstprivateVariables(file);// then firstprivate variable

    int s = patchUpSharedVariables(file);// consider shared variables 


    //TODO:  patchUpDefaultVariables(file);  


    result = p + f  + s;
    return result;

  }  //! the end of makeDataSharingExplicit() 


  void insertRTLHeaders(SgSourceFile* file)
  {
    ROSE_ASSERT(file != NULL);    
    SgGlobal* globalscope = file->get_globalScope() ; //isSgGlobal(*i);
    ROSE_ASSERT (globalscope != NULL);
#ifdef ENABLE_XOMP
    SageInterface::insertHeader("libxomp.h",PreprocessingInfo::after,false,globalscope);
    if (enable_accelerator)  // include inlined CUDA device codes
      SageInterface::insertHeader("xomp_cuda_lib_inlined.cu",PreprocessingInfo::after,false,globalscope);
#else    
    if (rtl_type == e_omni)
      SageInterface::insertHeader("ompcLib.h",PreprocessingInfo::after,false,globalscope);
    else if (rtl_type == e_gomp)
      SageInterface::insertHeader("libgomp_g.h",PreprocessingInfo::after,false,globalscope);
    else
      ROSE_ASSERT(false);
#endif      
  }

  void insertAcceleratorInit(SgSourceFile* sgfile)
  {
#ifdef ENABLE_XOMP
    bool hasMain= false;
    //find the main entry
    SgFunctionDefinition* mainDef=NULL;
    string mainName = "::main";
    ROSE_ASSERT(sgfile != NULL);

    SgFunctionDeclaration * mainDecl=findMain(sgfile);
    if (mainDecl!= NULL)
    {
      // printf ("Found main function setting hasMain == true \n");
      mainDef = mainDecl->get_definition();
      hasMain = true;
    }

    //TODO declare pointers for threadprivate variables and global lock
    //addGlobalOmpDeclarations(ompfrontend, sgfile->get_globalScope(), hasMain );

    if (! hasMain) return ;
    ROSE_ASSERT (mainDef!= NULL); // Liao, at this point, we expect a defining declaration of main() is 
    // look up symbol tables for symbols
    SgScopeStatement * currentscope = mainDef->get_body();

    SgExprStatement * expStmt=  buildFunctionCallStmt (SgName("xomp_acc_init"),
        buildVoidType(), NULL,currentscope);
    prependStatement(expStmt,currentscope);
#endif  // ENABLE_XOMP

    return;
  }

  //----------------------------
  //tasks:
  // * find the main entry for the application
  // * add (int argc, char *argv[]) if not exist(?)
  // * add runtime system init code at the begin 
  // * find all return points and append cleanup code
  // * add global declarations for threadprivate variables
  // * add global declarations for lock variables

  void insertRTLinitAndCleanCode(SgSourceFile* sgfile)
  {
#ifdef ENABLE_XOMP
    bool hasMain= false;
    //find the main entry
    SgFunctionDefinition* mainDef=NULL;
    string mainName = "::main";
    ROSE_ASSERT(sgfile != NULL);

    SgFunctionDeclaration * mainDecl=findMain(sgfile);
    if (mainDecl!= NULL)
    {
      // printf ("Found main function setting hasMain == true \n");
      mainDef = mainDecl->get_definition();
      hasMain = true;
    }

    //TODO declare pointers for threadprivate variables and global lock
    //addGlobalOmpDeclarations(ompfrontend, sgfile->get_globalScope(), hasMain );

    if (! hasMain) return ;
    ROSE_ASSERT (mainDef!= NULL); // Liao, at this point, we expect a defining declaration of main() is found
    // add parameter  int argc , char* argv[] if not exist
    SgInitializedNamePtrList args = mainDef->get_declaration()->get_args();
    SgType * intType=  SgTypeInt::createType();
    SgType *charType=  SgTypeChar::createType();

#if 1   //patch up argc, argv if they do not exit yet
    if (args.size()==0){
      SgFunctionParameterList *parameterList = mainDef->get_declaration()->get_parameterList();
      ROSE_ASSERT(parameterList);

      // int argc
      SgName name1("argc");
      SgInitializedName *arg1 = buildInitializedName(name1,intType);

      //char** argv
      SgName name2("argv");
      SgPointerType *pType1= buildPointerType(charType);
      SgPointerType *pType2= buildPointerType(pType1);
      SgInitializedName *arg2 = buildInitializedName(name2,pType2);

      appendArg(parameterList,arg1);
      appendArg(parameterList,arg2);

    } // end if (args.size() ==0)
#endif
    // add statements to prepare the runtime system
    //int status=0;
    SgIntVal * intVal = buildIntVal(0);

    SgAssignInitializer * init2=buildAssignInitializer(intVal);
    SgName *name1 = new SgName("status"); 
    SgVariableDeclaration* varDecl1 = buildVariableDeclaration(*name1, SgTypeInt::createType(),init2, mainDef->get_body());

    // cout<<"debug:"<<varDecl1->unparseToString()<<endl;

    //_ompc_init(argc, argv);
    SgType* voidtype =SgTypeVoid::createType();
    SgFunctionType *myFuncType= new SgFunctionType(voidtype,false);
    ROSE_ASSERT(myFuncType != NULL);

    //SgExprListExp, two parameters (argc, argv)
    // look up symbol tables for symbols
    SgScopeStatement * currentscope = mainDef->get_body();

    SgInitializedNamePtrList mainArgs = mainDef->get_declaration()->get_parameterList()->get_args();
    Rose_STL_Container <SgInitializedName*>::iterator i= mainArgs.begin();
    ROSE_ASSERT(mainArgs.size()==2);

    SgExprListExp * exp_list_exp = buildExprListExp();
    if (!SageInterface::is_Fortran_language())
    {
      SgVarRefExp *var1 = buildVarRefExp(isSgInitializedName(*i), mainDef->get_body());
      SgVarRefExp *var2 = buildVarRefExp(isSgInitializedName(*++i), mainDef->get_body());

      appendExpression(exp_list_exp,var1);
      appendExpression(exp_list_exp,var2);
    }

    SgExprStatement * expStmt=  buildFunctionCallStmt (SgName("XOMP_init"),
        buildVoidType(), exp_list_exp,currentscope);
    //  cout<<"debug:"<<expStmt->unparseToString()<<endl;
    //prepend to main body
    // Liao 1/5/2011
    // This is not safe since it cannot be prepended to an implicit none statement in fortran
    //prependStatement(expStmt,currentscope);
    //prependStatement(varDecl1,currentscope);
    if (SageInterface::is_Fortran_language())
    {
      SgStatement *l_stmt = findLastDeclarationStatement(currentscope);
      if (l_stmt != NULL)
        insertStatementAfter (l_stmt, varDecl1);
      else
        prependStatement(varDecl1,currentscope);
    }
    else // C/C++, we can always prepend it.
      prependStatement(varDecl1,currentscope);

    insertStatementAfter (varDecl1, expStmt);
    //---------------------- termination part

    //  cout<<"debug:"<<mainDef->unparseToString()<<endl;

    // search all return statements and add terminate() before them
    //the body of this function is empty in the runtime library
    // _ompc_terminate(status);

    //SgExprListExp, 1 parameters (status) 
    SgInitializedName *initName1= varDecl1->get_decl_item(*name1);
    ROSE_ASSERT(initName1);

    SgVarRefExp *var3 = buildVarRefExp(initName1,currentscope);
    SgExprListExp * exp_list_exp2 = buildExprListExp();
    appendExpression(exp_list_exp2,var3);

    //build call exp stmt
    SgExprStatement * expStmt2= buildFunctionCallStmt (SgName("XOMP_terminate"),
        buildVoidType(),exp_list_exp2,mainDef->get_body());
    // find return statement, insert before it
    Rose_STL_Container<SgNode*> rtList = NodeQuery::querySubTree(mainDef, V_SgReturnStmt);
    if (rtList.size()>0)
    {
      for(Rose_STL_Container<SgNode*>::iterator i= rtList.begin();i!=rtList.end();i++)
      {
        SgStatement *targetBB= isSgStatement((*i)->get_parent());
        ROSE_ASSERT(targetBB != NULL);
        if (i!=rtList.begin()) // for 2nd, 3rd, etc occurrences. We should always build a new statement instead of sharing a statement! 
        {
          expStmt2= buildFunctionCallStmt (SgName("XOMP_terminate"),
              buildVoidType(),exp_list_exp2,mainDef->get_body());
        }
        insertStatement(isSgStatement(*i),expStmt2);
      }
    }
    else //if not found append to function body
    {
      appendStatement(expStmt2,currentscope);
    }
    // cout<<"debug terminate:"<<expStmt2->unparseToString()<<endl;
    //   AstPostProcessing(mainDef->get_declaration());
#endif  // ENABLE_XOMP

    return;
  }

  //! Replace references to oldVar within root with references to newVar
  int replaceVariableReferences(SgNode* root, SgVariableSymbol* oldVar, SgVariableSymbol* newVar)
  {
    ROSE_ASSERT(oldVar != NULL);
    ROSE_ASSERT(newVar!= NULL);

    VariableSymbolMap_t varRemap;
    varRemap.insert(  VariableSymbolMap_t::value_type(oldVar, newVar));
    return replaceVariableReferences(root, varRemap);
  }

  //! Replace variable references within root based on a map from old symbols to new symbols
  /* This function is mostly used by transOmpVariables() to handle private, firstprivate, reduction, etc.
   *  
   *   
   */
  int replaceVariableReferences(SgNode* root, VariableSymbolMap_t varRemap)
  {
    int result =0;
    typedef Rose_STL_Container<SgNode *> NodeList_t;
    NodeList_t refs = NodeQuery::querySubTree (root, V_SgVarRefExp);
    for (NodeList_t::iterator i = refs.begin (); i != refs.end (); ++i)
    {
      SgVarRefExp* ref_orig = isSgVarRefExp (*i);
      ROSE_ASSERT (ref_orig);
#if 0 // Liao 6/9/2010  , 
      if (SageInterface::isUseByAddressVariableRef(ref_orig))
      {
        //  cout<<"Skipping a variable replacement because the variable is used by its address:"<< ref_orig->unparseToString()<<endl;
        continue; //skip the replacement for variable used by addresses
      }
#endif      
      VariableSymbolMap_t::const_iterator iter = varRemap.find(ref_orig->get_symbol()); 
      if (iter != varRemap.end())
      {
        SgVariableSymbol* newSym = iter->second; 
        ref_orig->set_symbol( newSym);
        result ++;
      }
    }
    return result;
  }

  int replaceVariablesWithPointerDereference(SgNode* root, ASTtools::VarSymSet_t vars)
  {
    int result = 0;
    typedef Rose_STL_Container<SgNode *> NodeList_t;
    NodeList_t refs = NodeQuery::querySubTree (root, V_SgVarRefExp);
    for (NodeList_t::iterator i = refs.begin (); i != refs.end (); ++i)
    {
      SgVarRefExp* ref_orig = isSgVarRefExp (*i);
      ROSE_ASSERT (ref_orig);
      ASTtools::VarSymSet_t::const_iterator ii = vars.find( ref_orig->get_symbol( ) );
      if( ii != vars.end( ) )
      {
        SgExpression * ptr_ref = buildPointerDerefExp( copyExpression(ref_orig) );
        ptr_ref->set_need_paren(true);
        SageInterface::replaceExpression( ref_orig, ptr_ref );
        result ++;
      }
    }
    return result;
  }

  //! Create a stride expression from an existing stride expression based on the loop iteration's order (incremental or decremental)
  // The assumption is orig_stride is just the raw operand of the condition expression of a loop
  // so it has to be adjusted to reflect the real stride: *(-1) if decremental
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

  //! check if an omp for/do loop use static schedule or not
  // Static schedule include: default schedule, or schedule(static[,chunk_size]) 
  bool useStaticSchedule(SgOmpClauseBodyStatement* omp_loop)
  {
    ROSE_ASSERT(omp_loop);
    bool result= false; 
    Rose_STL_Container<SgOmpClause*> clauses = getClause(omp_loop, V_SgOmpScheduleClause);
    if (clauses.size()==0)
    {
      result = true; // default schedule is static
    }else
    {
      ROSE_ASSERT(clauses.size() ==1); 
      SgOmpScheduleClause* s_clause = isSgOmpScheduleClause(clauses[0]);
      ROSE_ASSERT(s_clause);
      if (s_clause->get_kind() == SgOmpClause::e_omp_schedule_static)
        result = true;
    }
    return result;
  }

  // Chunk size  for dynamic and guided schedule should be 1 if not specified.
  static SgExpression* createAdjustedChunkSize(SgExpression* orig_chunk_size )
  {
    SgExpression* result = NULL;
    if (orig_chunk_size)
      result = copyExpression(orig_chunk_size);
    else
      result = buildIntVal(1);
    ROSE_ASSERT(result != NULL);  
    return result;
  }
  // Convert a schedule kind enum value to a small case string
  string toString(SgOmpClause::omp_schedule_kind_enum s_kind)
  {
    string result ;
    if (s_kind == SgOmpClause::e_omp_schedule_static)
    {
      result = "static";
    } else if (s_kind == SgOmpClause::e_omp_schedule_dynamic)
    {
      result = "dynamic";
    }
    else if (s_kind == SgOmpClause::e_omp_schedule_guided)
    {
      result = "guided";
    }
    else if (s_kind == SgOmpClause::e_omp_schedule_runtime)
    {
      result = "runtime";
    }
    else if (s_kind == SgOmpClause::e_omp_schedule_auto)
    {
      //      cerr<<"GOMP does not provide an implementation for schedule(auto)....."<<endl;
      result = "auto";
    } else
    {
      cerr<<"Error: illegal or unhandled schedule kind:"<< s_kind<<endl;
      ROSE_ASSERT(false);
    }
    return result;
  }
#ifdef ENABLE_XOMP 
  //! Generate XOMP loop schedule init function's name, union from OMNI's 
  string generateGOMPLoopInitFuncName (bool isOrdered, SgOmpClause::omp_schedule_kind_enum s_kind)
  {
    // XOMP_loop_static_init() 
    // XOMP_loop_ordered_static_init ()
    // XOMP_loop_dynamic_init () 
    // XOMP_loop_ordered_dynamic_init ()
    // .....
    string result;
    result = "XOMP_loop_";
    //Handled ordered
    if (isOrdered)
      result +="ordered_";
    result += toString(s_kind);  
    result += "_init"; 
    return result;
  }
#endif

  //! Generate GOMP loop schedule start function's name
  string generateGOMPLoopStartFuncName (bool isOrdered, SgOmpClause::omp_schedule_kind_enum s_kind)
  {
    // GOMP_loop_static_start () 
    // GOMP_loop_ordered_static_start ()
    // GOMP_loop_dynamic_start () 
    // GOMP_loop_ordered_dynamic_start ()
    // .....
    string result;
#ifdef ENABLE_XOMP 
    result = "XOMP_loop_";
#else    
    result = "GOMP_loop_";
#endif    
    // Handled ordered
    if (isOrdered)
      result +="ordered_";
    result += toString(s_kind);  
    result += "_start"; 
    return result;
  }

  //! Generate GOMP loop schedule next function's name
  string generateGOMPLoopNextFuncName (bool isOrdered, SgOmpClause::omp_schedule_kind_enum s_kind)
  {
    string result;
    // GOMP_loop_static_next() 
    // GOMP_loop_ordered_static_next ()
    // GOMP_loop_dynamic_next () 
    // GOMP_loop_ordered_dynamic_next()
    // .....

#ifdef ENABLE_XOMP 
    result = "XOMP_loop_";
#else
    result = "GOMP_loop_";
#endif    
    if (isOrdered)
      result +="ordered_";
    result += toString(s_kind);  
    result += "_next"; 
    return result;
  }

  //! Fortran only action: insert include "libxompf.h" into the function body with calls to XOMP_loop_* functions
  // This is necessary since XOMP_loop_* functions will be treated as returning REAL by implicit rules (starting with X)
  // This function finds the function definition enclosing a start node, check if there is any existing include 'libxompf.h'
  // then insert one if there is none.
  static void insert_libxompf_h(SgNode* startNode)
  {
    ROSE_ASSERT (startNode != NULL);
    // This function should not be used for other than Fortran
    ROSE_ASSERT (SageInterface::is_Fortran_language()  == true);
    // we don't expect input node is a func def already
    ROSE_ASSERT (isSgFunctionDefinition(startNode)  == NULL);

#if 0
    //find enclosing parallel region's body
    SgBasicBlock * omp_body = NULL;
    SgOmpParallelStatement * omp_stmt = isSgOmpParallelStatement(getEnclosingNode<SgOmpParallelStatement>(startNode));
    if (omp_stmt)
    {
      omp_body= isSgBasicBlock(omp_stmt->get_body());
      ROSE_ASSERT(omp_body != NULL);
    }

    // Find enclosing function body
    SgFunctionDefinition* func_def = getEnclosingProcedure (startNode);
    ROSE_ASSERT (func_def != NULL);
    SgBasicBlock * f_body = func_def->get_body();

    SgBasicBlock* t_body = (omp_body!=NULL)?omp_body:f_body;
#endif  
    SgBasicBlock* t_body = getEnclosingRegionOrFuncDefinition (startNode);
    ROSE_ASSERT (t_body != NULL);
    // Try to find an existing include 'libxompf.h'
    // Assumptions: 
    //   1. It only shows up at the top level, not within other SgBasicBlock
    //   2. The startNode is after the include line
    SgStatement * s_include = NULL ; // existing include 
    SgStatementPtrList stmt_list = t_body->get_statements();
    SgStatementPtrList::iterator iter;
    for (iter = stmt_list.begin(); iter != stmt_list.end(); iter ++)
    {
      SgStatement* stmt = *iter;
      ROSE_ASSERT (stmt != NULL);
      SgFortranIncludeLine * f_inc = isSgFortranIncludeLine(stmt);
      if (f_inc)
      {
        string f_name = StringUtility::stripPathFromFileName(f_inc->get_filename());
        if (f_name == "libxompf.h")
        {
          s_include = f_inc;
          break;
        }
      } 
    }
    if (s_include == NULL)
    {
      s_include = buildFortranIncludeLine ("libxompf.h");
      SgStatement* l_stmt = findLastDeclarationStatement (t_body);
      if (l_stmt)
        insertStatementAfter(l_stmt,s_include);
      else
        prependStatement(s_include, t_body);
    } 
  }
  //! Translate an omp for loop with non-static scheduling clause or with ordered clause ()
  // bb1 is the basic block to insert the translated loop
  // bb1 already has compiler-generated variable declarations for new loop control variables
  /*
   * start, end, incremental, chunk_size, own_start, own_end            
   XOMP_loop_static_init(int lower, int upper, int stride, int chunk_size);

   if (GOMP_loop_dynamic_start (orig_lower, orig_upper, adj_stride, orig_chunk, &_p_lower, &_p_upper)) 
  //  if (GOMP_loop_ordered_dynamic_start (S, E, INCR, CHUNK, &_p_lower, &_p_upper))  
  { 
  do                                                       
  {                                                      
  for (_p_index = _p_lower; _p_index < _p_upper; _p_index += orig_stride)
  set_data (_p_index, iam);                                 
  }                                                      
  while (GOMP_loop_dynamic_next (&_p_lower, &_p_upper));                
  // while (GOMP_loop_ordered_dynamic_next (&_p_lower, &_p_upper));     
  }
  GOMP_loop_end ();                                          
  //  GOMP_loop_end_nowait (); 
  //
  // More explanation: -------------------------------------------
  // Omni uses the following translation 
  _ompc_dynamic_sched_init(_p_loop_lower,_p_loop_upper,_p_loop_stride,5);
  while(_ompc_dynamic_sched_next(&_p_loop_lower,&_p_loop_upper)){
  for (_p_loop_index = _p_loop_lower; (_p_loop_index) < _p_loop_upper; _p_loop_index += _p_loop_stride) {
  k_3++;
  }
  }
  // In order to merge two kinds of translations into one scheme.
  // we split 
  while(_ompc_dynamic_sched_next(&_p_loop_lower,&_p_loop_upper)){
  for (_p_loop_index = _p_loop_lower; (_p_loop_index) < _p_loop_upper; _p_loop_index += _p_loop_stride) {
  k_3++;
  }
  }

  // to 
  if (_ompc_dynamic_sched_next(&_p_loop_lower,&_p_loop_upper)){
  do {
  for (_p_loop_index = _p_loop_lower; (_p_loop_index) < _p_loop_upper; _p_loop_index += _p_loop_stride) {
  k_3++;
  }
  } while (_ompc_dynamic_sched_next(&_p_loop_lower,&_p_loop_upper));
  }
  // and XOMP layer will compensate for the difference.
  */
  static void transOmpLoop_others(SgOmpClauseBodyStatement* target,  
      SgVariableDeclaration* index_decl, SgVariableDeclaration* lower_decl,  SgVariableDeclaration* upper_decl, 
      SgBasicBlock* bb1)
  {
    ROSE_ASSERT (target != NULL);
    ROSE_ASSERT (index_decl != NULL);
    ROSE_ASSERT (lower_decl != NULL);
    ROSE_ASSERT (upper_decl != NULL);
    ROSE_ASSERT (bb1 != NULL);

    SgScopeStatement* p_scope = target->get_scope();
    ROSE_ASSERT (p_scope != NULL);
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
    // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
    SgForStatement * for_loop = isSgForStatement(body);
    SgFortranDo* do_loop = isSgFortranDo(body);
    SgStatement * loop = for_loop!=NULL? (SgStatement*)for_loop:(SgStatement*)do_loop;

    SgInitializedName* orig_index; 
    SgExpression* orig_lower, * orig_upper, * orig_stride; 
    bool isIncremental = true; // if the loop iteration space is incremental
    // grab the original loop 's controlling information
    bool is_canonical = false;
    if (for_loop)
      is_canonical = isCanonicalForLoop (for_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental);
    else if (do_loop)
    {
      is_canonical = isCanonicalDoLoop (do_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental, NULL);
      insert_libxompf_h (do_loop);
    }
    else
    {
      cerr<<"error! transOmpLoop_others(). loop is neither for_loop nor do_loop. Aborting.."<<endl;
      ROSE_ASSERT (false);
    }  
    ROSE_ASSERT(is_canonical == true);

    Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpScheduleClause);

    // the case of with the ordered schedule, but without any schedule policy specified
    // treat it as (static, 0) based on GCC's translation
    SgOmpClause::omp_schedule_kind_enum s_kind = SgOmpClause::e_omp_schedule_static;
    SgExpression* orig_chunk_size = NULL;
    bool hasOrder = false;
    if (hasClause(target, V_SgOmpOrderedClause))
      hasOrder = true;
    ROSE_ASSERT(hasOrder || clauses.size() !=0);
    // Most cases: with schedule(kind,chunk_size)
    if (clauses.size() !=0)
    {  
      ROSE_ASSERT(clauses.size() ==1);
      SgOmpScheduleClause* s_clause = isSgOmpScheduleClause(clauses[0]);
      ROSE_ASSERT(s_clause);
      s_kind = s_clause->get_kind();
      orig_chunk_size = s_clause->get_chunk_size();

      // chunk size is 1 for dynamic and guided schedule, if not specified. 
      if (s_kind == SgOmpClause::e_omp_schedule_dynamic|| s_kind == SgOmpClause::e_omp_schedule_guided)
      {
        orig_chunk_size = createAdjustedChunkSize(orig_chunk_size);
      }
    }
    else
      orig_chunk_size = buildIntVal(0);

    // schedule(auto) does not have chunk size 
    if (s_kind != SgOmpClause::e_omp_schedule_auto  && s_kind != SgOmpClause::e_omp_schedule_runtime)
      ROSE_ASSERT(orig_chunk_size != NULL);
    // (GOMP_loop_static_start (orig_lower, orig_upper, adj_stride, orig_chunk, &_p_lower, &_p_upper)) 
    // (GOMP_loop_ordered_static_start (orig_lower, orig_upper, adj_stride, orig_chunk, &_p_lower, &_p_upper)) 
    string func_start_name= generateGOMPLoopStartFuncName(hasOrder, s_kind);
    // Assembling function call expression's parameters
    // first three are identical for all cases: 
    // we generate inclusive upper (-1) bounds after loop normalization, gomp runtime calls expect exclusive upper bounds
    // so we +1 to adjust it back to exclusive.

#if 0 // Liao 1/11/2011. I changed XOMP loop functions to use inclusive upper bounds. All adjustments are done within XOMP from now on
    int upper_adjust = 1;  // we use inclusive bounds, adjust them accordingly 
    if (!isIncremental) 
      upper_adjust = -1;
#endif 

#ifdef ENABLE_XOMP
    // build function init stmt
    //  _ompc_dynamic_sched_init(_p_loop_lower,_p_loop_upper,_p_loop_stride,5);
    SgExprListExp* para_list_i = buildExprListExp(copyExpression(orig_lower), 
        //buildAddOp(copyExpression(orig_upper), buildIntVal(upper_adjust)),
        copyExpression(orig_upper), 
        createAdjustedStride(orig_stride, isIncremental)); 
    if (s_kind != SgOmpClause::e_omp_schedule_auto && s_kind != SgOmpClause::e_omp_schedule_runtime)
    {
      appendExpression(para_list_i, copyExpression(orig_chunk_size));
    }

    string func_init_name= generateGOMPLoopInitFuncName(hasOrder, s_kind);
    SgExprStatement* func_init_stmt = buildFunctionCallStmt(func_init_name, buildVoidType(), para_list_i, bb1);
    appendStatement(func_init_stmt, bb1);
#endif    
    //build function start
    SgExprListExp* para_list = buildExprListExp(copyExpression(orig_lower), 
        //buildAddOp(copyExpression(orig_upper), buildIntVal(upper_adjust)),
        copyExpression(orig_upper),
        createAdjustedStride(orig_stride, isIncremental)); 
    if (s_kind != SgOmpClause::e_omp_schedule_auto && s_kind != SgOmpClause::e_omp_schedule_runtime)
    {
      appendExpression(para_list, orig_chunk_size);
      //appendExpression(para_list, copyExpression(orig_chunk_size));
    }
    if (for_loop)
    {
      appendExpression(para_list, buildAddressOfOp(buildVarRefExp(lower_decl)));
      appendExpression(para_list, buildAddressOfOp(buildVarRefExp(upper_decl)));
    }
    else if (do_loop)
    {
      appendExpression(para_list, buildVarRefExp(lower_decl));
      appendExpression(para_list, buildVarRefExp(upper_decl));
    }
    SgFunctionCallExp* func_start_exp = NULL;
    SgBasicBlock * true_body = buildBasicBlock();
    SgIfStmt* if_stmt = NULL; 
    if (SageInterface::is_Fortran_language())
    {
      // Note for Fortran, we treat the function as returning integer, same type as the rhs of .eq.
      // Otherwise, unparser will complain.
      func_start_exp  = buildFunctionCallExp(func_start_name, buildIntType(), para_list, bb1);
      if_stmt = buildIfStmt(buildEqualityOp(func_start_exp,buildIntVal(1)), true_body, NULL);
      // if_stmt->set_use_then_keyword(true); 
      // if_stmt->set_has_end_statement(true); 
    }
    else 
    {
      func_start_exp  = buildFunctionCallExp(func_start_name, buildBoolType(), para_list, bb1);
      if_stmt = buildIfStmt(func_start_exp, true_body, NULL);
    }

    appendStatement(if_stmt, bb1);
    SgExprListExp * n_exp_list = NULL;
    if (for_loop)
    {
      n_exp_list = buildExprListExp(buildAddressOfOp(buildVarRefExp(lower_decl)), buildAddressOfOp(buildVarRefExp(upper_decl)));
    }
    else if (do_loop)
    {
      n_exp_list = buildExprListExp(buildVarRefExp(lower_decl), buildVarRefExp(upper_decl));
    }
    ROSE_ASSERT (n_exp_list!=NULL);
    SgExpression* func_next_exp = NULL; 

    // do {} while (GOMP_loop_static_next (&_p_lower, &_p_upper))
    if (for_loop)
    { 

      func_next_exp =buildFunctionCallExp(generateGOMPLoopNextFuncName(hasOrder, s_kind), buildBoolType(),
          n_exp_list, bb1);
      SgBasicBlock * do_body = buildBasicBlock();
      SgDoWhileStmt * do_while_stmt = buildDoWhileStmt(do_body, func_next_exp);
      appendStatement(do_while_stmt, true_body);
      // insert the loop into do-while
      appendStatement(loop, do_body);
    } 
    // Liao 1/7/2011, Fortran does not support SgDoWhileStmt
    // We use the following control flow as an alternative:  
    //   label  continue
    //          loop_here 
    //          if (GOMP_loop_static_next (&_p_lower, &_p_upper)) 
    //             goto label  
    else if (do_loop)
    {
      SgFunctionDefinition * funcDef = getEnclosingFunctionDefinition(bb1);
      ROSE_ASSERT (funcDef != NULL);
      // label  CONTINUE
      SgLabelStatement * label_stmt_1 = buildLabelStatement("", NULL);
      appendStatement(label_stmt_1, true_body);
      int l_val = suggestNextNumericLabel(funcDef);
      setFortranNumericLabel(label_stmt_1, l_val);
      // loop here
      appendStatement(loop, true_body);
      // if () goto label
      func_next_exp =buildFunctionCallExp(generateGOMPLoopNextFuncName(hasOrder, s_kind), buildIntType(),
          n_exp_list, bb1);
      SgIfStmt * if_stmt_2 = buildIfStmt(buildEqualityOp(func_next_exp,buildIntVal(1)), buildBasicBlock(), buildBasicBlock());
      SgGotoStatement* gt_stmt = buildGotoStatement(label_stmt_1->get_numeric_label()->get_symbol());
      appendStatement (gt_stmt, isSgScopeStatement(if_stmt_2->get_true_body()));
      appendStatement(if_stmt_2,true_body);
      // assertion from unparser
      SgStatementPtrList & statementList = isSgBasicBlock(if_stmt_2->get_true_body())->get_statements();
      ROSE_ASSERT(statementList.size() == 1);
    }

    // Rewrite loop control variables
    replaceVariableReferences(loop,isSgVariableSymbol(orig_index->get_symbol_from_symbol_table ()), 
        getFirstVarSym(index_decl));
#if 0 // Liao 1/11/2011. I changed XOMP loop functions to use inclusive upper bounds. All adjustments are done within XOMP from now on
    int upperAdjust;
    if (isIncremental)  // adjust the bounds again, inclusive bound so -1 for incremental loop
      upperAdjust = -1;
    else 
      upperAdjust = 1;
#endif      
    SageInterface::setLoopLowerBound(loop, buildVarRefExp(lower_decl));
    //SageInterface::setLoopUpperBound(loop, buildAddOp(buildVarRefExp(upper_decl),buildIntVal(upperAdjust)));
    SageInterface::setLoopUpperBound(loop, buildVarRefExp(upper_decl));
    ROSE_ASSERT (orig_upper != NULL);
    transOmpVariables(target, bb1, orig_upper); // This should happen before the barrier is inserted.
    // GOMP_loop_end ();  or GOMP_loop_end_nowait (); 
#ifdef ENABLE_XOMP
    string func_loop_end_name = "XOMP_loop_end"; 
#else    
    string func_loop_end_name = "GOMP_loop_end"; 
#endif    
    if (hasClause(target, V_SgOmpNowaitClause)) 
    {
      func_loop_end_name+= "_nowait";
    }
    SgExprStatement* end_func_stmt = buildFunctionCallStmt(func_loop_end_name, buildVoidType(), NULL, bb1);
    appendStatement(end_func_stmt, bb1);
  }

  // Expected AST
  // * SgOmpForStatement
  // ** SgForStatement
  // Algorithm:
  // Loop normalization first  for stop condition expressions
  //   <: for (i= 0;i <20; i++) --> for (i= 0;i <20; i+=1)  [0,20, +1] to pass to runtime calls
  //  <=: for (i= 0;i<=20; i++) --> for (i= 0;i <21; i+=1) 
  //   >: for (i=20;i >-1; i--) --> for (i=20;i >-1; i-=1) [20, -1, -1]
  //  >=: for (i=20;i>= 0; i--) --> for (i=20;i >-1; i-=1)
  // We have a SageInterface::forLoopNormalization() which does the opposite (normalizing a C loop to a Fortran style loop)
  // < --> <= and > --> >=, 
  // GCC-GOMP use compiler-generated statements to schedule loop iterations using static schedule
  // All other schedule policies use runtime calls instead.
  // We translate static schedule here and non-static ones in transOmpLoop_others()
  // 
  // Static schedule, including:
  //1. default (static even) case
  //2. schedule(static[, chunk_size]): == static even if chunk_size is not specified
  // gomp does not provide a runtime call to calculate loop control values 
  // for the default (static even) scheduling
  // compilers have to generate the statements to do this. I HATE THIS!!!
  // the loop scheduling algorithm for the default case is
  /*
  // calculate loop iteration count from lower, upper and stride , no -1 if upper is an inclusive bound
  int _p_iter_count = (stride + -1 + upper - lower )/stride;  
  // calculate a proper chunk size
  // two cases: evenly divisible  20/5 =4
  //   not evenly divisible 20/3= 6
  // Initial candidate  

  int _p_num_threads = omp_get_num_threads ();
  _p_chunk_size = _p_iter_count / _p_num_threads;
  int _p_ck_temp = (_p_chunk_size * _p_num_threads) != _p_iter_count;
  // increase the chunk size by 1 if not evenly divisible
  _p_chunk_size = _p_ck_temp + _p_chunk_size;

  // decide on the lower and upper bound for the current thread
  int _p_thread_id = omp_get_thread_num ();
  _p_lower = lower + _p_chunk_size * _p_thread_id * stride;a
  // -1 if upper is an inclusive bound
  _p_upper = _p_lower + _p_chunk_size * stride;

  // adjust the upper bound
  _p_upper = MIN_EXPR <_p_upper, upper>;
  // _p_upper = _p_upper<upper? _p_upper: upper; 
  // Note: decremental iteration space needs some minor changes to the algorithm above.
  // stride should be negated 
  // MIN_EXP should be MAX_EXP
  // upper bound adjustment should be +1 instead of -1
  */
  void transOmpLoop(SgNode* node)
    //void transOmpFor(SgNode* node)
  {
    ROSE_ASSERT(node != NULL);
    SgOmpForStatement* target1 = isSgOmpForStatement(node);
    SgOmpDoStatement* target2 = isSgOmpDoStatement(node);

    SgOmpClauseBodyStatement* target = (target1!=NULL?(SgOmpClauseBodyStatement*)target1:(SgOmpClauseBodyStatement*)target2);
    ROSE_ASSERT (target != NULL);

    SgScopeStatement* p_scope = target->get_scope();
    ROSE_ASSERT (p_scope != NULL);

    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
    // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
    SgForStatement * for_loop = isSgForStatement(body);
    SgFortranDo * do_loop = isSgFortranDo(body);

    SgStatement* loop = (for_loop!=NULL?(SgStatement*)for_loop:(SgStatement*)do_loop);
    ROSE_ASSERT (loop != NULL);
    // Step 1. Loop normalization
    // we reuse the normalization from SageInterface, though it is different from what gomp expects.
    // the point is to have a consistent loop form. We can adjust the difference later on.
    if (for_loop) 
      SageInterface::forLoopNormalization(for_loop);
    else if (do_loop)
      SageInterface::doLoopNormalization(do_loop);
    else
    {
      cerr<<"error! transOmpLoop(). loop is neither for_loop nor do_loop. Aborting.."<<endl;
      ROSE_ASSERT (false); 
    }

    SgInitializedName * orig_index = NULL;
    SgExpression* orig_lower = NULL;
    SgExpression* orig_upper= NULL;
    SgExpression* orig_stride= NULL;
    bool isIncremental = true; // if the loop iteration space is incremental
    // grab the original loop 's controlling information
    bool is_canonical = false;
    if (for_loop)
      is_canonical = isCanonicalForLoop (for_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental);
    else if (do_loop)
      is_canonical = isCanonicalDoLoop (do_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental, NULL);
    ROSE_ASSERT(is_canonical == true);

    // step 2. Insert a basic block to replace SgOmpForStatement
    // This newly introduced scope is used to hold loop variables, private variables ,etc
    SgBasicBlock * bb1 = SageBuilder::buildBasicBlock(); 


    //   fprintf(stderr, "target: %s\n", target->unparseToString().c_str() );

    replaceStatement(target, bb1, true);

    //TODO handle preprocessing information
    // Save some preprocessing information for later restoration. 
    //  AttachedPreprocessingInfoType ppi_before, ppi_after;
    //  ASTtools::cutPreprocInfo (s, PreprocessingInfo::before, ppi_before);
    //  ASTtools::cutPreprocInfo (s, PreprocessingInfo::after, ppi_after);

    // Declare local loop control variables: _p_loop_index _p_loop_lower _p_loop_upper , no change to the original stride
    SgType* loop_var_type  = NULL ;
#if 0    
    if (sizeof(void*) ==8 ) // xomp interface expects long* for some runtime calls. 
      loop_var_type = buildLongType();
    else 
      loop_var_type = buildIntType();
#endif
    // xomp interface expects long for some runtime calls now, 6/9/2010
    if (for_loop) 
      loop_var_type = buildLongType();
    else if (do_loop)  // No long integer in Fortran
      loop_var_type = buildIntType();
    SgVariableDeclaration* index_decl =  NULL; 
    SgVariableDeclaration* lower_decl =  NULL; 
    SgVariableDeclaration* upper_decl =  NULL;

    if (SageInterface::is_Fortran_language() )
    {// special rules to insert variable declarations in Fortran
      // They have to be inserted to enclosing function body or enclosing parallel region body
      // and after existing declaration statement sequence, if any.
      nCounter ++;
      index_decl = buildAndInsertDeclarationForOmp("p_index_"+StringUtility::numberToString(nCounter), loop_var_type , NULL,bb1); 
      lower_decl = buildAndInsertDeclarationForOmp("p_lower_"+StringUtility::numberToString(nCounter), loop_var_type , NULL,bb1); 
      upper_decl = buildAndInsertDeclarationForOmp("p_upper_"+StringUtility::numberToString(nCounter), loop_var_type , NULL,bb1); 
    }
    else
    {  
      index_decl = buildVariableDeclaration("p_index_", loop_var_type , NULL,bb1); 
      lower_decl = buildVariableDeclaration("p_lower_", loop_var_type , NULL,bb1); 
      upper_decl = buildVariableDeclaration("p_upper_", loop_var_type , NULL,bb1); 

      appendStatement(index_decl, bb1);
      appendStatement(lower_decl, bb1);
      appendStatement(upper_decl, bb1);
    } 

    bool hasOrder = false;
    if (hasClause(target, V_SgOmpOrderedClause))
      hasOrder = true;

    // Grab or calculate chunk_size
    SgExpression* my_chunk_size = NULL; 
    bool hasSpecifiedSize = false;
    Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpScheduleClause);
    if (clauses.size() !=0)
    {
      SgOmpScheduleClause* s_clause = isSgOmpScheduleClause(clauses[0]);
      ROSE_ASSERT(s_clause);
      //SgOmpClause::omp_schedule_kind_enum s_kind = s_clause->get_kind();
      // ROSE_ASSERT(s_kind == SgOmpClause::e_omp_schedule_static);
      SgExpression* orig_chunk_size = s_clause->get_chunk_size();  
      //  ROSE_ASSERT(orig_chunk_size->get_parent() != NULL);
      if (orig_chunk_size)
      {
        hasSpecifiedSize = true;
        my_chunk_size = orig_chunk_size;
      }
    }

    //  step 3. Translation for omp for 
    //if (hasClause(target, V_SgOmpScheduleClause)) 
    if (!useStaticSchedule(target) || hasOrder || hasSpecifiedSize) 
    {
      transOmpLoop_others( target,   index_decl, lower_decl,   upper_decl, bb1);
    }
    else 
    {
      //void XOMP_loop_default(int lower, int upper, int stride, long *n_lower, long * n_upper)
      // XOMP_loop_default (lower, upper, stride, &_p_lower, &_p_upper );
      // lower:  copyExpression(orig_lower)
      // upper: copyExpression(orig_upper)
      // stride: copyExpression(orig_stride)
      // n_lower: buildVarRefExp(lower_decl)
      // n_upper: buildVarRefExp(upper_decl)
      SgExpression* e4 = NULL; 
      SgExpression* e5 = NULL; 
      if (for_loop)
      {
        e4= buildAddressOfOp(buildVarRefExp(lower_decl));
        e5= buildAddressOfOp(buildVarRefExp(upper_decl));
      }
      else if (do_loop)
      {// Fortran, pass-by-reference by default
        e4= buildVarRefExp(lower_decl);
        e5= buildVarRefExp(upper_decl);
      }
      ROSE_ASSERT (e4&&e5);
      SgExprListExp* call_parameters = buildExprListExp(copyExpression(orig_lower), copyExpression(orig_upper), copyExpression(orig_stride), 
          e4, e5);
      SgStatement * call_stmt =  buildFunctionCallStmt ("XOMP_loop_default", buildVoidType(), call_parameters, bb1);
      appendStatement(call_stmt, bb1);

      // add loop here
      appendStatement(loop, bb1); 
      // replace loop index with the new one
      replaceVariableReferences(loop,
          isSgVariableSymbol(orig_index->get_symbol_from_symbol_table()), getFirstVarSym(index_decl))    ; 
      // rewrite the lower and upper bounds
      SageInterface::setLoopLowerBound(loop, buildVarRefExp(lower_decl)); 
      SageInterface::setLoopUpperBound(loop, buildVarRefExp(upper_decl)); 

      transOmpVariables(target, bb1,orig_upper); // This should happen before the barrier is inserted.
      // insert barrier if there is no nowait clause
      if (!hasClause(target, V_SgOmpNowaitClause)) 
      {
        //insertStatementAfter(for_loop, buildFunctionCallStmt("GOMP_barrier", buildVoidType(), NULL, bb1));
#ifdef ENABLE_XOMP
        appendStatement(buildFunctionCallStmt("XOMP_barrier", buildVoidType(), NULL, bb1), bb1);
#else   
        appendStatement(buildFunctionCallStmt("GOMP_barrier", buildVoidType(), NULL, bb1), bb1);
#endif  
      }
    }

    // handle variables 
    // transOmpVariables(target, bb1); // This should happen before the barrier is inserted.
  } // end trans omp for


  //! Translate omp for or omp do loops affected by the "omp target" directive, Liao 1/28/2013
  /*

Example: 
  // for (i = 0; i < N; i++)
  { // top level block, prepare to be outlined.
  // int i ; // = blockDim.x * blockIdx.x + threadIdx.x; // this CUDA declaration can be inserted later
  i = getLoopIndexFromCUDAVariables(1); 

  if (i<SIZE)  // boundary checking to avoid invalid memory accesses
  {
  for (j = 0; j < M; j++)
  for (k = 0; k < K; k++)
  c[i][j]= c[i][j]+a[i][k]*b[k][j];
  }
  } // end of top level block

Algorithm:
   * check if it is a OmpTargetLoop
   * loop normalization
   * replace OmpForStatement with a block: bb1
   * declare int _dev_i within bb1;  replace for loop body’s loop index with _dev_i;
   * build if stmt with correct condition
   * move loop body to if-stmt’s true body
   * remove for_loop
   */
  void transOmpTargetLoop(SgNode* node)
  {
    //step 0: Sanity check
    ROSE_ASSERT(node != NULL);
    SgOmpForStatement* target1 = isSgOmpForStatement(node);
    SgOmpDoStatement* target2 = isSgOmpDoStatement(node);

    SgOmpClauseBodyStatement* target = (target1!=NULL?(SgOmpClauseBodyStatement*)target1:(SgOmpClauseBodyStatement*)target2);
    ROSE_ASSERT (target != NULL);

    SgScopeStatement* p_scope = target->get_scope();
    ROSE_ASSERT (p_scope != NULL);

    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
    // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
    SgForStatement * for_loop = isSgForStatement(body);
    SgFortranDo * do_loop = isSgFortranDo(body);

    SgStatement* loop = (for_loop!=NULL?(SgStatement*)for_loop:(SgStatement*)do_loop);
    ROSE_ASSERT (loop != NULL);

    // make sure this is really a loop affected by "omp target"
    //bool is_target_loop = false;
    SgNode* parent = node->get_parent();
    ROSE_ASSERT (parent != NULL);
    if (isSgBasicBlock(parent)) // skip one possible BB between omp parallel and omp for.
      parent = parent->get_parent();
    SgNode* grand_parent = parent->get_parent();
    ROSE_ASSERT (grand_parent != NULL);
    SgOmpParallelStatement* parent_parallel = isSgOmpParallelStatement (parent) ;
    SgOmpTargetStatement* grand_target = isSgOmpTargetStatement(grand_parent);
    ROSE_ASSERT (parent_parallel !=NULL); 
    ROSE_ASSERT (grand_target !=NULL); 

    // Step 1. Loop normalization
    // For the init statement: for (int i=0;... ) becomes int i; for (i=0;..) 
    // For test expression: i<x is normalized to i<= (x-1) and i>x is normalized to i>= (x+1) 
    // For increment expression: i++ is normalized to i+=1 and i-- is normalized to i+=-1 i-=s is normalized to i+= -s 
    if (for_loop)
      SageInterface::forLoopNormalization(for_loop);
    else if (do_loop)
      SageInterface::doLoopNormalization(do_loop);
    else
    {
      cerr<<"error! transOmpLoop(). loop is neither for_loop nor do_loop. Aborting.."<<endl;
      ROSE_ASSERT (false);
    }

    SgInitializedName * orig_index = NULL;
    SgExpression* orig_lower = NULL;
    SgExpression* orig_upper= NULL;
    SgExpression* orig_stride= NULL;
    bool isIncremental = true; // if the loop iteration space is incremental
    // grab the original loop 's controlling information
    bool is_canonical = false;

    if (for_loop)
      is_canonical = isCanonicalForLoop (for_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental);
    else if (do_loop)
      is_canonical = isCanonicalDoLoop (do_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental, NULL);
    ROSE_ASSERT(is_canonical == true);

    // loop iteration space: upper - lower + 1
    // This expression will be later used to help generate xomp_get_max1DBlock(VEC_LEN), which needs iteration count to calculate max thread block numbers
    cuda_loop_iter_count_1 = buildAddOp(buildSubtractOp(deepCopy(orig_upper), deepCopy(orig_lower)), buildIntVal(1));

    // also make sure the loop body is a block
    // TODO: we consider peeling off 1 level loop control only, need to be conditional on what the spec. can provide at pragma level
    // TODO: Fortran support later on
    ROSE_ASSERT (for_loop != NULL);
    SgBasicBlock* loop_body = ensureBasicBlockAsBodyOfFor (for_loop);


    //Step 2. Insert a basic block to replace SgOmpForStatement
    // This newly introduced scope is used to hold loop variables ,etc
    SgBasicBlock * bb1 = SageBuilder::buildBasicBlock();
    replaceStatement(target, bb1, true);

    //Step 3. Using device thread id and replace reference of original loop index with the thread index
    // Declare device thread id variable
    //int i = blockDim.x * blockIdx.x + threadIdx.x;
    //SgAssignInitializer* init_idx =  buildAssignInitializer( 
    //                                     buildAddOp( buildMultiplyOp (buildVarRefExp("blockDim.x"), buildVarRefExp("blockIdx.x")) , 
    //                                      buildVarRefExp("threadIdx.x", bb1)));
    //Better build of CUDA variables within a runtime library call so these variables are hidden from the translation
    //  getLoopIndexFromCUDAVariables(1)
    SgAssignInitializer* init_idx =  buildAssignInitializer(buildFunctionCallExp(SgName("getLoopIndexFromCUDAVariables"), buildIntType(),buildExprListExp(buildIntVal(1)),bb1), buildIntType());

    SgVariableDeclaration* dev_i_decl = buildVariableDeclaration("_dev_i", buildIntType(), init_idx, bb1); 
    prependStatement (dev_i_decl, bb1);
    SgVariableSymbol* dev_i_symbol = getFirstVarSym (dev_i_decl);
    ROSE_ASSERT (dev_i_symbol != NULL);

#if 1 // test mysterious replace with _dev_i
    // replace reference to loop index with reference to device i variable
    ROSE_ASSERT (orig_index != NULL);
    SgSymbol * orig_symbol = orig_index ->get_symbol_from_symbol_table () ;
    ROSE_ASSERT (orig_symbol != NULL);

    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(loop_body, V_SgVarRefExp);
    for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
    {
      SgVarRefExp *vRef = isSgVarRefExp((*i));
      if (vRef->get_symbol() == orig_symbol)
        vRef->set_symbol(dev_i_symbol);
    }
#endif 

    // Step 4. build the if () condition statement, move the loop body into the true body
    // Liao, 2/21/2013. We must be accurate about the range of iterations or the computation may result in WRONG results!!
    // A classic example is the Jacobi iteration: in which the first and last iterations are not executed to make sure elements have boundaries.
    // After normalization, we have inclusive lower and upper bounds of the input loop
    // the condition of if() should look like something: if (_dev_i >=0+1 &&_dev_i <= (n - 1) - 1)  {...}
    SgBasicBlock* true_body = buildBasicBlock();
    SgExprStatement* cond_stmt = NULL;
    if (isIncremental)
    {
      SgExpression* lhs = buildGreaterOrEqualOp (buildVarRefExp(dev_i_symbol), deepCopy(orig_lower));
      SgExpression* rhs = buildLessOrEqualOp (buildVarRefExp(dev_i_symbol), deepCopy(orig_upper));
      cond_stmt = buildExprStatement (buildAndOp(lhs, rhs));
    }
    else
    {
      cerr<<"error. transOmpTargetLoop(): decremental case is not yet handled !"<<endl;
      ROSE_ASSERT (false);
    }
    SgIfStmt * if_stmt = buildIfStmt(cond_stmt, true_body, NULL);
    appendStatement(if_stmt, bb1);
    moveStatementsBetweenBlocks (loop_body, true_body);  
    // Peel off the original loop
    removeStatement (for_loop);

    // handle private variables at this loop level, mostly loop index variables.
    // TODO: this is not very elegant since the outer most loop's loop variable is still translated.
    //for reduction
    per_block_declarations.clear(); // must reset to empty or wrong reference to stale content generated previously
    transOmpVariables(target, bb1,NULL, true);
  }

  //! Translate omp for or omp do loops affected by the "omp target" directive, using a round robin-scheduler Liao 7/10/2014
  /*  Algorithm

  // original loop info. grab from the loop structure
  int orig_start =0;
  int orig_end = n-1; // inclusive upper bound
  int orig_step = 1; 
  int orig_chunk_size = 1;// fixed at 1

  // new lower and upper bound, to be filled out by the loop scheduler
  int _dev_lower;
  int _dev_upper;
  int _dev_loop_chunk_size;
  int _dev_loop_sched_index;
  int _dev_loop_stride;  

  // CUDA thread count and ID for the 1-D block
  int _dev_thread_num = getCUDABlockThreadCount(1);
  int _dev_thread_id = getLoopIndexFromCUDAVariables(1); 

  //initialize scheduler
  XOMP_static_sched_init (orig_start, orig_end, orig_step, orig_chunk_size, _dev_thread_num, _dev_thread_id, \
  & _dev_loop_chunk_size , & _dev_loop_sched_index, & _dev_loop_stride);

  while (XOMP_static_sched_next (&_dev_loop_sched_index, orig_end, orig_step,_dev_loop_stride, _dev_loop_chunk_size, _dev_thread_num, _dev_thread_id, & _dev_lower
  , & _dev_upper))
  {
  for (i= _dev_lower ; i <= _dev_upper; i ++ ) { // rewrite lower and upper bound and step normalized to 1
  // original loop body here
  }
  }
  }

 */
void transOmpTargetLoop_RoundRobin(SgNode* node)
{
  //step 0: Sanity check
  ROSE_ASSERT(node != NULL);
  SgOmpForStatement* target1 = isSgOmpForStatement(node);
  SgOmpDoStatement* target2 = isSgOmpDoStatement(node);

  // the target of the translation is a SgOmpForStatement
  SgOmpClauseBodyStatement* target = (target1!=NULL?(SgOmpClauseBodyStatement*)target1:(SgOmpClauseBodyStatement*)target2);
  ROSE_ASSERT (target != NULL);

  SgScopeStatement* p_scope = target->get_scope();
  ROSE_ASSERT (p_scope != NULL);

  SgStatement * body =  target->get_body();
  ROSE_ASSERT(body != NULL);
  // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
  SgForStatement * for_loop = isSgForStatement(body);
  SgFortranDo * do_loop = isSgFortranDo(body);

  SgStatement* loop = (for_loop!=NULL?(SgStatement*)for_loop:(SgStatement*)do_loop);
  ROSE_ASSERT (loop != NULL);

  // make sure this is really a loop affected by "omp target"
  //bool is_target_loop = false;
  SgNode* parent = node->get_parent();
  ROSE_ASSERT (parent != NULL);
  if (isSgBasicBlock(parent)) // skip one possible BB between omp parallel and omp for.
    parent = parent->get_parent();
  SgNode* grand_parent = parent->get_parent();
  ROSE_ASSERT (grand_parent != NULL);
  SgOmpParallelStatement* parent_parallel = isSgOmpParallelStatement (parent) ;
  SgOmpTargetStatement* grand_target = isSgOmpTargetStatement(grand_parent);
  ROSE_ASSERT (parent_parallel !=NULL);
  ROSE_ASSERT (grand_target !=NULL);

  // Step 1. Loop normalization
  // For the init statement: for (int i=0;... ) becomes int i; for (i=0;..) 
  // For test expression: i<x is normalized to i<= (x-1) and i>x is normalized to i>= (x+1) 
  // For increment expression: i++ is normalized to i+=1 and i-- is normalized to i+=-1 i-=s is normalized to i+= -s 
  if (for_loop)
    SageInterface::forLoopNormalization(for_loop);
  else if (do_loop)
    SageInterface::doLoopNormalization(do_loop);
  else
  {
    cerr<<"error! transOmpLoop(). loop is neither for_loop nor do_loop. Aborting.."<<endl;
    ROSE_ASSERT (false);
  }

  SgInitializedName * orig_index = NULL;
  SgExpression* orig_lower = NULL;
  SgExpression* orig_upper= NULL;
  SgExpression* orig_stride= NULL;
  bool isIncremental = true; // if the loop iteration space is incremental
  // grab the original loop 's controlling information
  bool is_canonical = false;

  if (for_loop)
    is_canonical = isCanonicalForLoop (for_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental);
  else if (do_loop)
    is_canonical = isCanonicalDoLoop (do_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental, NULL);
  ROSE_ASSERT(is_canonical == true);

  // loop iteration space: upper - lower + 1, not used within this function, but a global variable used later.
  // This expression will be later used to help generate xomp_get_max1DBlock(VEC_LEN), which needs iteration count to calculate max thread block numbers
  cuda_loop_iter_count_1 = buildAddOp(buildSubtractOp(deepCopy(orig_upper), deepCopy(orig_lower)), buildIntVal(1));

  // TODO: Fortran support later on
  ROSE_ASSERT (for_loop != NULL);
  //SgBasicBlock* loop_body = ensureBasicBlockAsBodyOfFor (for_loop);

  //Step 2. Insert a basic block to replace SgOmpForStatement
  // This newly introduced scope is used to hold loop variables ,etc
  SgBasicBlock * bb1 = SageBuilder::buildBasicBlock();
  replaceStatement(target, bb1, true);

  // Insert variables used by the two scheduler functions
  /* int _dev_lower;
     int _dev_upper;
     int _dev_loop_chunk_size;
     int _dev_loop_sched_index;
     int _dev_loop_stride;  
  */
  SgVariableDeclaration* dev_lower_decl = buildVariableDeclaration ("_dev_lower", buildIntType(), NULL, bb1); 
  appendStatement (dev_lower_decl, bb1);
  SgVariableDeclaration* dev_upper_decl = buildVariableDeclaration ("_dev_upper", buildIntType(), NULL, bb1); 
  appendStatement (dev_upper_decl, bb1);
  SgVariableDeclaration* dev_loop_chunk_size_decl = buildVariableDeclaration ("_dev_loop_chunk_size", buildIntType(), NULL, bb1); 
  appendStatement (dev_loop_chunk_size_decl, bb1);
  SgVariableDeclaration* dev_loop_sched_index_decl = buildVariableDeclaration ("_dev_loop_sched_index", buildIntType(), NULL, bb1); 
  appendStatement (dev_loop_sched_index_decl , bb1);
  SgVariableDeclaration* dev_loop_stride_decl = buildVariableDeclaration ("_dev_loop_stride", buildIntType(), NULL, bb1); 
  appendStatement (dev_loop_stride_decl, bb1);

  // Insert CUDA thread id and count declarations
  // int _dev_thread_num = getCUDABlockThreadCount(1);
  SgAssignInitializer* init_idx =  buildAssignInitializer(buildFunctionCallExp(SgName("getCUDABlockThreadCount"), buildIntType(),buildExprListExp(buildIntVal(1)),bb1), buildIntType());
  SgVariableDeclaration* dev_thread_num_decl = buildVariableDeclaration("_dev_thread_num", buildIntType(), init_idx, bb1);
  appendStatement (dev_thread_num_decl, bb1);
  SgVariableSymbol* dev_thread_num_symbol = getFirstVarSym (dev_thread_num_decl);
  ROSE_ASSERT (dev_thread_num_symbol!= NULL);
 
  // int _dev_thread_id = getLoopIndexFromCUDAVariables(1); 
  init_idx =  buildAssignInitializer(buildFunctionCallExp(SgName("getLoopIndexFromCUDAVariables"), buildIntType(),buildExprListExp(buildIntVal(1)),bb1), buildIntType());
  SgVariableDeclaration* dev_thread_id_decl = buildVariableDeclaration("_dev_thread_id", buildIntType(), init_idx, bb1);
  appendStatement (dev_thread_id_decl, bb1);
  SgVariableSymbol* dev_thread_id_symbol = getFirstVarSym (dev_thread_id_decl);
  ROSE_ASSERT (dev_thread_id_symbol!= NULL);
  
  // initialize scheduler
  // XOMP_static_sched_init (orig_start, orig_end, orig_step, orig_chunk_size, _dev_thread_num, _dev_thread_id, 
  //                       & _dev_loop_chunk_size , & _dev_loop_sched_index, & _dev_loop_stride);
  SgExprListExp* parameters  = buildExprListExp(copyExpression (orig_lower), copyExpression (orig_upper), copyExpression(orig_stride), buildIntVal(1), buildVarRefExp(dev_thread_num_symbol), buildVarRefExp (dev_thread_id_symbol) );
  appendExpression (parameters, buildAddressOfOp(buildVarRefExp (getFirstVarSym(dev_loop_chunk_size_decl))) ); 
  appendExpression (parameters, buildAddressOfOp(buildVarRefExp (getFirstVarSym(dev_loop_sched_index_decl))) ); 
  appendExpression (parameters, buildAddressOfOp(buildVarRefExp (getFirstVarSym(dev_loop_stride_decl))) ); 
  SgStatement* call_stmt = buildFunctionCallStmt ("XOMP_static_sched_init", buildVoidType(), parameters, bb1);
  appendStatement (call_stmt, bb1);

  // function call exp as while (condition)
  // XOMP_static_sched_next (&_dev_loop_sched_index, orig_end, orig_step,_dev_loop_stride, _dev_loop_chunk_size, 
  //                       _dev_thread_num, _dev_thread_id, & _dev_lower , & _dev_upper)
  parameters = buildExprListExp (buildAddressOfOp(buildVarRefExp (getFirstVarSym(dev_loop_sched_index_decl))), copyExpression (orig_upper), copyExpression(orig_stride),buildVarRefExp (getFirstVarSym(dev_loop_stride_decl)),  buildVarRefExp (getFirstVarSym(dev_loop_chunk_size_decl)));
  appendExpression (parameters,buildVarRefExp(dev_thread_num_symbol));
  appendExpression (parameters,buildVarRefExp(dev_thread_id_symbol));
  appendExpression (parameters, buildAddressOfOp(buildVarRefExp (getFirstVarSym(dev_lower_decl))) ); 
  appendExpression (parameters, buildAddressOfOp(buildVarRefExp (getFirstVarSym(dev_upper_decl))) ); 
  SgExpression* func_call_exp = buildFunctionCallExp ("XOMP_static_sched_next", buildBoolType(), parameters, bb1);

  SgStatement* new_loop = deepCopy (for_loop);
  SgWhileStmt* w_stmt = buildWhileStmt (func_call_exp, new_loop);
  appendStatement (w_stmt, bb1);
//  moveStatementsBetweenBlocks (loop_body, isSgBasicBlock(w_stmt->get_body()));

  // rewrite upper, lower bounds, TODO how about step? normalized to 1 already ?
  setLoopLowerBound (new_loop, buildVarRefExp (getFirstVarSym(dev_lower_decl)));
  setLoopUpperBound (new_loop, buildVarRefExp (getFirstVarSym(dev_upper_decl)));
  removeStatement (for_loop);
  


  // handle private variables at this loop level, mostly loop index variables.
  // TODO: this is not very elegant since the outer most loop's loop variable is still translated.
  //for reduction
  per_block_declarations.clear(); // must reset to empty or wrong reference to stale content generated previously
  transOmpVariables(target, bb1,NULL, true);

}



  //! Check if an OpenMP statement has a clause of type vt
  Rose_STL_Container<SgOmpClause*> getClause(SgOmpClauseBodyStatement* clause_stmt, const VariantT & vt)
  {
    ROSE_ASSERT(clause_stmt != NULL);
    Rose_STL_Container<SgOmpClause*> p_clause =
      NodeQuery::queryNodeList<SgOmpClause>(clause_stmt->get_clauses(),vt);
    return  p_clause;
  }

  //! Check if an OpenMP statement has a clause of type vt
  bool hasClause(SgOmpClauseBodyStatement* clause_stmt, const VariantT & vt)
  {
    ROSE_ASSERT(clause_stmt != NULL);
    Rose_STL_Container<SgOmpClause*> p_clause =
      NodeQuery::queryNodeList<SgOmpClause>(clause_stmt->get_clauses(),vt);
    return  (p_clause.size()!= 0) ;
  }

//! A helper function to generate implicit or explicit task for either omp parallel or omp task
//  Parameters:  SgNode* node: the OMP Parallel or OMP Parallel
//               std::string& wrapper_name: for C/C++, structure wrapper is used to wrap all parameters. This is to return the struct name
//               ASTtools::VarSymSet_t& syms :  all variables to be passed in/out the outlined function
//               ASTtools::VarSymSet_t&pdSyms3 : variables which must be passed by references, used to guide the creation of struct wrapper: member using base type vs. using pointer type.  The algorithm to generate this set is already very conservative: after transOmpVariables() , the only exclude firstprivate. In the context of OpenMP, it is equivalent to say this is a set of variables which are to be passed by references. 
// Algorithms:
//    Set flags of the outliner to indicate desired behaviors: parameter wrapping or not?
//    translate OpenMP variables (first private, private, reduction, etc) so the code to be outlined is already as simple as possible (without OpenMP-specific semantics)
//
// It calls the ROSE AST outliner internally. 
SgFunctionDeclaration* generateOutlinedTask(SgNode* node, std::string& wrapper_name, ASTtools::VarSymSet_t& syms, ASTtools::VarSymSet_t&pdSyms3)
//SgFunctionDeclaration* generateOutlinedTask(SgNode* node, std::string& wrapper_name, ASTtools::VarSymSet_t& syms, std::set<SgInitializedName*>& readOnlyVars, ASTtools::VarSymSet_t&pdSyms3)
{
  ROSE_ASSERT(node != NULL);
  SgOmpClauseBodyStatement* target = isSgOmpClauseBodyStatement(node);  
  ROSE_ASSERT (target != NULL);

  // must be either omp task or omp parallel
  SgOmpTaskStatement* target1 = isSgOmpTaskStatement(node);
  SgOmpParallelStatement* target2 = isSgOmpParallelStatement(node);
  ROSE_ASSERT (target1 != NULL || target2 != NULL);

  SgStatement * body =  target->get_body();
  ROSE_ASSERT(body != NULL);
  SgFunctionDeclaration* result= NULL;
  //Initialize outliner
  Outliner::enable_classic = false; // we need use parameter wrapping, which is not classic behavior of outlining
  if (SageInterface::is_Fortran_language())
  {
    //We pass one variable per parameter, at least for Fortran 77
    Outliner::useParameterWrapper = false;
//    Outliner::enable_classic = true; // use subroutine's parameters directly
  }
  else 
  {
    // C/C++ : always wrap parameters into a structure for outlining used during OpenMP translation
    Outliner::useParameterWrapper = true; 
    Outliner::useStructureWrapper = true;
  }

  //TODO there should be some semantics check for the regions to be outlined
  //for example, multiple entries or exists are not allowed for OpenMP
  //This is however of low priority since most vendor compilers have this already.
  SgBasicBlock* body_block = Outliner::preprocess(body);

  //---------------------------------------------------------------
  //  Key step: handling special variables BEFORE actual outlining is done!!
  // Variable handling is done after Outliner::preprocess() to ensure a basic block for the body,
  // but before calling the actual outlining 
  // This simplifies the outlining since firstprivate, private variables are replaced 
  //with their local copies before outliner is used 
  transOmpVariables (target, body_block); 

  // variable sets for private, firstprivate, reduction, and pointer dereferencing (pd)
  ASTtools::VarSymSet_t pSyms, fpSyms,reductionSyms, pdSyms;

  string func_name = Outliner::generateFuncName(target);
  SgGlobal* g_scope = SageInterface::getGlobalScope(body_block);
  ROSE_ASSERT(g_scope != NULL);

   //-----------------------------------------------------------------
  // Generic collection of variables to be passed as parameters of the outlined functions
  // semantically equivalent to shared variables in OpenMP
  Outliner::collectVars(body_block, syms);

   // Now decide on the parameter convention for all the parameters: pass-by-value vs. pass-by-reference (pointer dereferencing)
   
  //     SageInterface::collectReadOnlyVariables(body_block,readOnlyVars);
  // We choose to be conservative about the variables needing pointer dereferencing first
  // AllParameters - readOnlyVars  - private -firstprivate 
  // Union ASTtools::collectPointerDereferencingVarSyms(body_block, pdSyms) 

  // Assume all parameters need to be passed by reference/pointers first
  std::copy(syms.begin(), syms.end(), std::inserter(pdSyms,pdSyms.begin()));

  //exclude firstprivate variables: they are read only in fact
  //TODO keep class typed variables!!!  even if they are firstprivate or private!! 
  SgInitializedNamePtrList fp_vars = collectClauseVariables (target, V_SgOmpFirstprivateClause);
  ASTtools::VarSymSet_t fp_syms, pdSyms2;
  convertAndFilter (fp_vars, fp_syms);
  set_difference (pdSyms.begin(), pdSyms.end(),
      fp_syms.begin(), fp_syms.end(),
      std::inserter(pdSyms2, pdSyms2.begin()));
 //  ROSE_ASSERT (pdSyms.size() == pdSyms2.size());  this means the previous set_difference is neccesary !
 
#if 0
  // Similarly , exclude private variable, also read only
  // TODO: is this necessary? private variables should  be handled already in transOmpVariables(). So Outliner::collectVars() will not collect them at all!
  SgInitializedNamePtrList p_vars = collectClauseVariables (target, V_SgOmpPrivateClause);
  ASTtools::VarSymSet_t p_syms; //, pdSyms3;
  convertAndFilter (p_vars, p_syms);
  //TODO keep class typed variables!!!  even if they are firstprivate or private!! 
  set_difference (pdSyms2.begin(), pdSyms2.end(),
      p_syms.begin(), p_syms.end(),
      std::inserter(pdSyms3, pdSyms3.begin()));

  ROSE_ASSERT (pdSyms2.size() == pdSyms3.size());
#endif
  pdSyms3 = pdSyms2;

  // lastprivate and reduction variables cannot be excluded  since write access to their shared copies

  // Sara Royuela 24/04/2012
  // When unpacking array variables in the outlined function, it is needed to have access to the size of the array.
  // When this size is a variable (or a operation containing variables), this variable must be added to the arguments of the outlined function.
  // Example:
  //    Input snippet:                      Outlined function:
  //        int N = 1;                          static void OUT__1__5493__(void *__out_argv) {
  //        int a[N];                               int (*a)[N] = (int (*)[N])(((struct OUT__1__5493___data *)__out_argv) -> a_p);
  //        #pragma omp task shared(a)              ( *a)[0] = 1;
  //            a[0] = 1;                       }
  ASTtools::VarSymSet_t new_syms;
  for (ASTtools::VarSymSet_t::const_iterator i = syms.begin (); i != syms.end (); ++i)
  {
    SgType* i_type = (*i)->get_declaration()->get_type();

    while (isSgArrayType(i_type))
    {
      // Get most significant dimension
      SgExpression* index = ((SgArrayType*) i_type)->get_index();

      // Get the variables used to compute the dimension
      // FIXME We insert a new statement and delete it afterwards in order to use "collectVars" function
      //       Think about implementing an specific function for expressions
      ASTtools::VarSymSet_t a_syms, a_pSyms;
      SgExprStatement* index_stmt = buildExprStatement(index);
      appendStatement(index_stmt, body_block);
      Outliner::collectVars(index_stmt, a_syms);
      SageInterface::removeStatement(index_stmt);
      for(ASTtools::VarSymSet_t::iterator j = a_syms.begin(); j != a_syms.end(); ++j)
      {
        const SgVariableSymbol* s = *j;
        new_syms.insert(s);   // If the symbol is not in the symbol list, it is added
      }
      
      // Advance over the type
      i_type = ((SgArrayType*) i_type)->get_base_type();
    }
  }

  for (ASTtools::VarSymSet_t::const_iterator i = new_syms.begin (); i != new_syms.end (); ++i)
  {
    const SgVariableSymbol* s = *i;
    syms.insert(s);
  }

  
  // a data structure used to wrap parameters
  SgClassDeclaration* struct_decl = NULL; 
  if (SageInterface::is_Fortran_language())
    struct_decl = NULL;  // We cannot use structure for Fortran
  else  
     struct_decl = Outliner::generateParameterStructureDeclaration (body_block, func_name, syms, pdSyms3, g_scope);
  // ROSE_ASSERT (struct_decl != NULL); // can be NULL if no parameters to be passed

  //Generate the outlined function
  /* Parameter list
       SgBasicBlock* s,  // block to be outlined
       const string& func_name_str, // function name
       const ASTtools::VarSymSet_t& syms, // parameter list for all variables to be passed around
       const ASTtools::VarSymSet_t& pdSyms, // variables must use pointer dereferencing (pass-by-reference)
       const ASTtools::VarSymSet_t& psyms, // private or dead variables (not live-in, not live-out)
       SgClassDeclaration* struct_decl,  // an optional wrapper structure for parameters
    Depending on the internal flag, unpacking/unwrapping statements are generated inside the outlined function to use wrapper parameters.
  */
  std::set< SgInitializedName *> restoreVars;
  result = Outliner::generateFunction(body_block, func_name, syms, pdSyms3, restoreVars, struct_decl, g_scope);
  
#if 0
  // special handling for empty variables to be passed: test case helloNested.cpp
  // TODO: move this portion from GenerateFunc.cc variableHandling()
  //For OpenMP lowering, we have to have a void * parameter even if there is no need to pass any parameters 
  //in order to match the gomp runtime lib 's function prototype for function pointers
  SgFile* cur_file = getEnclosingFileNode(result);
  ROSE_ASSERT (cur_file != NULL);
  //if (cur_file->get_openmp_lowering () && ! SageInterface::is_Fortran_language())
  if (cur_file->get_openmp_lowering ())
  {
    if (syms.size() ==0)
    {
      SgFunctionParameterList* params = result->get_parameterList ();
      ROSE_ASSERT (params);
      SgFunctionDefinition* def = result->get_definition ();
      ROSE_ASSERT (def);
      SgBasicBlock* body = def->get_body ();
      ROSE_ASSERT (body);

      SgName var1_name = "__out_argv";
      SgType* ptype= NULL; 
      // A dummy integer parameter for Fortran outlined function
      if (SageInterface::is_Fortran_language() )
      {
        var1_name = "out_argv";
        ptype = buildIntType();
        SgVariableDeclaration *var_decl = buildVariableDeclaration(var1_name,ptype, NULL, body);
        prependStatement(var_decl, body);
      }
      else
      {
        ptype = buildPointerType (buildVoidType());
        ROSE_ASSERT (Outliner::useStructureWrapper);
      }
      SgInitializedName* parameter1=NULL;
      parameter1 = buildInitializedName(var1_name,ptype);
      appendArg(params,parameter1);
    }
  }
  else 
  {
   ROSE_ASSERT (false);
  }
#endif

  Outliner::insert(result, g_scope, body_block);

#if 0 //Liao 12/20/2012 this logic is moved into outliner since using static function is generally a good idea.
   // astOutlining/Insert.cc  GlobalProtoInserter::insertManually() has the code now.
   //
  // A fix from Tristan Ravitch travitch@cs.wisc.edu to make outlined functions static to avoid name conflicts
  if (result->get_definingDeclaration() != NULL)
    SageInterface::setStatic(result->get_definingDeclaration());
  if (result->get_firstNondefiningDeclaration() != NULL)
    SageInterface::setStatic(result->get_firstNondefiningDeclaration());
#endif
  // Generate packing statements
  // must pass target , not body_block to get the right scope in which the declarations are inserted
  if (!SageInterface::is_Fortran_language()) 
    wrapper_name= Outliner::generatePackingStatements(target,syms,pdSyms3, struct_decl);
  ROSE_ASSERT (result != NULL);

  // 12/7/2010
  // For Fortran outlined subroutines, 
  // add INCLUDE 'omp_lib.h' in case OpenMP runtime routines are called within the outlined subroutines
  if (SageInterface::is_Fortran_language() )
  {
    SgBasicBlock * body =  result->get_definition()->get_body();
    ROSE_ASSERT (body != NULL);
    SgFortranIncludeLine * inc_line = buildFortranIncludeLine("omp_lib.h");
    prependStatement(inc_line, body);
  }
  return result;
}

#if 0 // Moved to SageInterface
//! iterate through the statement within a scope, find the last declaration statement (if any) after which 
//  another declaration statement can be inserted.  
// This is useful to find a safe place to insert a declaration statement with special requirements about where it can be inserted.
// e.g. a variable declaration statement should not be inserted before IMPLICIT none in Fortran
// If it returns NULL, a declaration statement should be able to be prepended to the scope
static SgStatement* findLastDeclarationStatement(SgScopeStatement * scope)
{
  SgStatement* rt = NULL;
  ROSE_ASSERT (scope != NULL);

  SgStatementPtrList stmt_list = scope->generateStatementList ();

  for (size_t i = 0; i<stmt_list.size(); i++)
  {
    SgStatement* cur_stmt = stmt_list[i];
    if (isSgDeclarationStatement(cur_stmt))
      rt = cur_stmt;
    //if (isSgImplicitStatement(cur_stmt)) || isSgFortranIncludeLine(cur_stmt) || isSgDeclarationStatement
  }

  return rt;
}
#endif
  /* GCC's libomp uses the following translation method: 
   * 
   * 
#include "libgomp_g.h"
#include <stdio.h>
#include "omp.h"

  //void main_omp_fn_0 (struct _omp_data_s_0* _omp_data_i);
  void main_omp_fn_0 (void ** __out_argv);

  int main (void)
  {
  int i;
  //  struct _omp_data_s_0 _omp_data_o_1;

  i = 0;
  // wrap shared variables
  //  _omp_data_o_1.i = i;
  void *__out_argv1__5876__[1];
  __out_argv1__5876__[0] = ((void *)(&i));

  //GOMP_parallel_start (main_omp_fn_0, &_omp_data_o_1, 0);
  GOMP_parallel_start (main_omp_fn_0, &__out_argv1__5876__, 0); // must use & here!!!
  //main_omp_fn_0 (&_omp_data_o_1);
  //main_omp_fn_0 ((void *)__out_argv1__5876__); //best type match
  main_omp_fn_0 (__out_argv1__5876__);
  GOMP_parallel_end ();

  // grab the changed value
  //  i = _omp_data_o_1.i;
  return 0;
  }

  //void main_omp_fn_0(void *__out_argvp)
  void main_omp_fn_0(void **__out_argv)
  //void OUT__1__5876__(void **__out_argv)
  {
  // void **__out_argv = (void **) __out_argvp;
  int *i = (int *)(__out_argv[0]);
   *i = omp_get_thread_num();
   printf("Hello,world! I am thread %d\n", *i);
   }
   */
  void transOmpParallel (SgNode* node)
  {
    ROSE_ASSERT(node != NULL);
    SgOmpParallelStatement* target = isSgOmpParallelStatement(node);
    ROSE_ASSERT (target != NULL);

// Test if the file info has been corrected transferred to SgOmpParallelStatement
//    target->get_startOfConstruct()->display();
//    target->get_endOfConstruct()->display();
     
    // Liao 12/7/2010
    // For Fortran code, we have to insert EXTERNAL OUTLINED_FUNC into 
    // the function body containing the parallel region
    SgFunctionDefinition * func_def = NULL;
    if (SageInterface::is_Fortran_language() )
    {
      func_def = getEnclosingFunctionDefinition(target);
      ROSE_ASSERT (func_def != NULL);
    }
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
    // Save preprocessing info as early as possible, avoiding mess up from the outliner
    AttachedPreprocessingInfoType save_buf1, save_buf2, save_buf_inside;
    cutPreprocessingInfo(target, PreprocessingInfo::before, save_buf1) ;
    cutPreprocessingInfo(target, PreprocessingInfo::after, save_buf2) ;
   
    // some #endif may be attached to the body, we should not move it with the body into
    // the outlined funcion!!
     // cutPreprocessingInfo(body, PreprocessingInfo::before, save_buf_body) ;

    // 1/15/2009, Liao, also handle the last #endif, which is attached inside of the target
    cutPreprocessingInfo(target, PreprocessingInfo::inside, save_buf_inside) ;

    //-----------------------------------------------------------------
    // step 1: generated an outlined function as the task
    std::string wrapper_name;
    ASTtools::VarSymSet_t syms; // store all variables in the outlined task ???
    ASTtools::VarSymSet_t pdSyms3; // store all variables which should be passed by references (pd means pointer dereferencing)
    std::set<SgInitializedName*> readOnlyVars; // not used since OpenMP provides all variable controlling details already. side effect analysis is essentially not being used. 
    SgFunctionDeclaration* outlined_func = generateOutlinedTask (node, wrapper_name, syms, pdSyms3);

    if (SageInterface::is_Fortran_language() )
    { // EXTERNAL outlined_function , otherwise the function name will be interpreted as a integer/real variable
      ROSE_ASSERT (func_def != NULL);
      SgBasicBlock * func_body = func_def->get_body();
      ROSE_ASSERT (func_body != NULL);
      SgAttributeSpecificationStatement* external_stmt1 = buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_externalStatement); 
      SgFunctionRefExp *func_ref1 = buildFunctionRefExp (outlined_func); 
      external_stmt1->get_parameter_list()->prepend_expression(func_ref1);
      func_ref1->set_parent(external_stmt1->get_parameter_list());
      // must put it into the declaration statement part, after possible implicit/include statements, if any
      SgStatement* l_stmt = findLastDeclarationStatement (func_body); 
      if (l_stmt)
        insertStatementAfter(l_stmt,external_stmt1);
      else  
        prependStatement(external_stmt1, func_body);
    }

    SgScopeStatement * p_scope = target->get_scope();
    ROSE_ASSERT(p_scope != NULL);

    //-----------------------------------------------------------------
    // step 2: generate call to the outlined function
#ifndef ENABLE_XOMP  // direct use of gomp needs an explicit call to the task in the original sequential process
    // generate a function call to it
    SgStatement* func_call = Outliner::generateCall (outlined_func, syms, readOnlyVars, wrapper_name,p_scope);
    ROSE_ASSERT(func_call != NULL);  

    // Replace the parallel region with the function call statement
    // TODO should we introduce another level of scope here?
    SageInterface::replaceStatement(target,func_call, true);
#endif

    // Generate the parameter list for the call to the XOMP runtime function
    SgExprListExp* parameters  = NULL;
    // pass ifClauseValue: set to the expression of if-clause, otherwise set to 1
    SgExpression* ifClauseValue = NULL; 
    if (hasClause(target, V_SgOmpIfClause))
    {
      Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpIfClause);
      ROSE_ASSERT (clauses.size() ==1); // should only have one if ()
      SgOmpIfClause * if_clause = isSgOmpIfClause (clauses[0]);
      ROSE_ASSERT (if_clause->get_expression() != NULL);
      ifClauseValue = copyExpression(if_clause->get_expression());
    }
    else
      ifClauseValue = buildIntVal(1);  
    // pass num_threads_specified: 0 if not, otherwise set to the expression of num_threads clause  
    SgExpression* numThreadsSpecified = NULL;
    if (hasClause(target, V_SgOmpNumThreadsClause))
    {
      Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpNumThreadsClause);
      ROSE_ASSERT (clauses.size() ==1); // should only have one if ()
      SgOmpNumThreadsClause * numThreads_clause = isSgOmpNumThreadsClause (clauses[0]);
      ROSE_ASSERT (numThreads_clause->get_expression() != NULL);
      numThreadsSpecified = copyExpression(numThreads_clause->get_expression());
    }
    else
      numThreadsSpecified = buildIntVal(0);  

    if (SageInterface::is_Fortran_language())
    { // The parameter list for Fortran is little bit different from C/C++'s XOMP interface 
      // since we are forced to pass variables one by one in the parameter list to support Fortran 77
       // void xomp_parallel_start (void (*func) (void *), unsigned * ifClauseValue, unsigned* numThread, int * argcount, ...)
      //e.g. xomp_parallel_start(OUT__1__1527__,0,2,S,K)
      SgExpression * parameter4 = buildIntVal (pdSyms3.size()); //TODO double check if pdSyms3 is the right set of variables to be passed
      parameters = buildExprListExp(buildFunctionRefExp(outlined_func), ifClauseValue, numThreadsSpecified, parameter4);

      ASTtools::VarSymSet_t::iterator iter = pdSyms3.begin();
      for (; iter!=pdSyms3.end(); iter++)
      {
        const SgVariableSymbol * sb = *iter;
        appendExpression (parameters, buildVarRefExp(const_cast<SgVariableSymbol *>(sb)));
      }
    }
    else 
    { 
      // C/C++ case: 
      //add GOMP_parallel_start (OUT_func_xxx, &__out_argv1__5876__, 0);
      // or GOMP_parallel_start (OUT_func_xxx, 0, 0); // if no variables need to be passed
      SgExpression * parameter2 = NULL;
      if (syms.size()==0)
        parameter2 = buildIntVal(0);
      else
        parameter2 =  buildAddressOfOp(buildVarRefExp(wrapper_name, p_scope));
     parameters = buildExprListExp(buildFunctionRefExp(outlined_func), parameter2, ifClauseValue, numThreadsSpecified); 
    }

    ROSE_ASSERT (parameters != NULL);

#ifdef ENABLE_XOMP
  // extern void XOMP_parallel_start (void (*func) (void *), void *data, unsigned ifClauseValue, unsigned numThreadsSpecified);
  // * func: pointer to a function which will be run in parallel
  // * data: pointer to a data segment which will be used as the arguments of func
  // * ifClauseValue: set to if-clause-expression if if-clause exists, or default is 1.
  // * numThreadsSpecified: set to the expression of num_threads clause if the clause exists, or default is 0
  // Liao 3/11/2013, additional file location info, at least for C/C++  for now
  if (!SageInterface::is_Fortran_language())
  {
    string file_name = target->get_startOfConstruct()->get_filenameString();
    int line = target->get_startOfConstruct()->get_line();
    parameters->append_expression(buildStringVal(file_name));
    parameters->append_expression(buildIntVal(line));
  }

    SgExprStatement * s1 = buildFunctionCallStmt("XOMP_parallel_start", buildVoidType(), parameters, p_scope); 
    SageInterface::replaceStatement(target, s1 , true);
#else
   ROSE_ASSERT (false); //This portion of code should never be used anymore. Kept for reference only.
//    SgExprStatement * s1 = buildFunctionCallStmt("GOMP_parallel_start", buildVoidType(), parameters, p_scope); 
//    SageInterface::insertStatementBefore(func_call, s1); 
#endif
    // Keep preprocessing information
    // I have to use cut-paste instead of direct move since 
    // the preprocessing information may be moved to a wrong place during outlining
    // while the destination node is unknown until the outlining is done.
   // SageInterface::moveUpPreprocessingInfo(s1, target, PreprocessingInfo::before); 
   pastePreprocessingInfo(s1, PreprocessingInfo::before, save_buf1); 
    // add GOMP_parallel_end ();
#ifdef ENABLE_XOMP

    SgExprListExp*  parameters2 = buildExprListExp();
    if (!SageInterface::is_Fortran_language())
    {
      string file_name = target->get_endOfConstruct()->get_filenameString();
      int line = target->get_endOfConstruct()->get_line();
      parameters2->append_expression(buildStringVal(file_name));
      parameters2->append_expression(buildIntVal(line));
    }

    //SgExprStatement * s2 = buildFunctionCallStmt("XOMP_parallel_end", buildVoidType(), NULL, p_scope); 
    SgExprStatement * s2 = buildFunctionCallStmt("XOMP_parallel_end", buildVoidType(), parameters2, p_scope); 
    SageInterface::insertStatementAfter(s1, s2);  // insert s2 after s1
#else
    SgExprStatement * s2 = buildFunctionCallStmt("GOMP_parallel_end", buildVoidType(), NULL, p_scope); 
    SageInterface::insertStatementAfter(func_call, s2); 
#endif
   // SageInterface::moveUpPreprocessingInfo(s2, target, PreprocessingInfo::after); 
   pastePreprocessingInfo(s2, PreprocessingInfo::after, save_buf2); 
   // paste the preprocessing info with inside position to the outlined function's body
   pastePreprocessingInfo(outlined_func->get_definition()->get_body(), PreprocessingInfo::inside, save_buf_inside); 

    // some #endif may be attached to the body, we should not move it with the body into
    // the outlined funcion!!
   // move dangling #endif etc from the body to the end of s2
   movePreprocessingInfo(body,s2,PreprocessingInfo::before, PreprocessingInfo::after); 

   // SageInterface::deepDelete(target);
  }
  
//! A helper function to categorize variables collected from map clauses
static   
void categorizeMapClauseVariables( const SgInitializedNamePtrList & all_vars, // all variables collected from map clauses
          std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > >&  array_dimensions, // array bounds  info
                                    std::set<SgSymbol*>& array_syms, // variable symbols which are array types (explicit or as a pointer)
                                    std::set<SgSymbol*>& atom_syms) // variable symbols which are non-aggregate types: scalar, pointer, etc
  {
   // categorize the variables:
    for (SgInitializedNamePtrList::const_iterator iter = all_vars.begin(); iter != all_vars.end(); iter ++)
    {
      SgInitializedName* i_name = *iter;
      ROSE_ASSERT (i_name != NULL);

      // In C/C++, an array can have a pointer type or SgArrayType. 
      // We collect SgArrayType for sure. But for pointer type, we consult the array_dimension to decide.
      SgSymbol* sym = i_name ->get_symbol_from_symbol_table ();
      ROSE_ASSERT (sym != NULL);
      SgType * type = sym->get_type ();
      // TODO handle complex types like structure, typedef, cast, etc. here
      if (isSgArrayType(type))
        array_syms.insert (sym);
      else if (isSgPointerType(type))
      {
        if (array_dimensions[sym].size()!=0) // if we have bound information for the pointer type, it represents an array
          array_syms.insert (sym);
        else // otherwise a pointer pointing to non-array types
          atom_syms.insert (sym);
      }
      else if (isScalarType(type))
      {
        atom_syms.insert (sym);
      }
      else
      {
        cerr<<"Error. transOmpMapVariables() of omp_lowering.cpp: unhandled map clause variable type:"<<type->class_name()<<endl;
      }
    }
   // make sure the categorization is complete
    ROSE_ASSERT (all_vars.size() == (array_syms.size() + atom_syms.size()) );
  }

 //! Generate expression calculating the size of a linearized array
 // e.g. row_size * sizeof(double)* column_size
 static
 SgExpression * generateSizeCalculationExpression(SgSymbol* sym, SgType* element_type,  // element's type, used to generate sizeof(type)
                     const std::vector < std::pair <SgExpression*, SgExpression*> >& dimensions) // dimensions of an array, [lower:length] format
{
  SgExpression* result =NULL;
  ROSE_ASSERT (element_type != NULL);
  result = buildSizeOfOp(element_type);

  // situation 1: array section info is explicitly specified by the user code, we grab it.
  if (dimensions.size()>0)
  {
    for (std::vector < std::pair <SgExpression*, SgExpression*> >::const_iterator iter = dimensions.begin(); iter != dimensions.end(); iter++)
    {
      std::pair <SgExpression*, SgExpression*> bound_pair = *iter; 
      //     SgExpression* lower_exp = bound_pair.first;
      SgExpression* length_exp = bound_pair.second;
      //result = buildMultiplyOp(result,  buildSubtractOp (deepCopy(upper_bound), deepCopy(lower_bound)));
      result = buildMultiplyOp(result, deepCopy(length_exp));
    }
  }
  else // situation 2: for static arrays: users do not need to specify array section info. at all. We get from arrayType
  {
    ROSE_ASSERT (sym!= NULL);
    SgType* orig_type = sym->get_type();
    SgArrayType* a_type = isSgArrayType (orig_type);
    ROSE_ASSERT (a_type!= NULL);
    std::vector< SgExpression * > dims = get_C_array_dimensions (a_type);
    for (std::vector < SgExpression* >::const_iterator iter = dims.begin(); iter != dims.end(); iter++)
    {
      SgExpression* length_exp  = *iter; 
      //TODO: get_C_array_dimensions returns one extra null expression somehow.
      if (!isSgNullExpression(length_exp))
        result = buildMultiplyOp(result, deepCopy(length_exp));
    }
  }

  return result; 
}                    

  // Check if a variable is in the clause's variable list
// TODO: move to header
 static bool isInClauseVariableList(SgOmpClause* cls, SgSymbol* var)
{
  ROSE_ASSERT (cls && var); 
  SgOmpVariablesClause* var_cls = isSgOmpVariablesClause(cls);
  ROSE_ASSERT (var_cls); 
  SgVarRefExpPtrList refs = isSgOmpVariablesClause(var_cls)->get_variables();

  std::vector<SgSymbol*> var_list;
  for (size_t j =0; j< refs.size(); j++)
    var_list.push_back(refs[j]->get_symbol());

  if (find(var_list.begin(), var_list.end(), var) != var_list.end() )
    return true;
  else
    return false;
}

 // ! Replace all references to original symbol with references to new symbol
// return the number of references being replaced. 
 // TODO: move to SageInterface
//static int replaceVariableReferences(SgNode* subtree, const SgVariableSymbol* origin_sym, SgVariableSymbol* new_sym )
static int replaceVariableReferences(SgNode* subtree, std::map <SgVariableSymbol*, SgVariableSymbol*> symbol_map)
{
  int result = 0;
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(subtree, V_SgVarRefExp);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgVarRefExp *vRef = isSgVarRefExp((*i));
    // skip compiler generated references to the original variables which meant to be kept.
    // TODO: maybe a better way is to match a pattern: if it is the first parameter of xomp_deviceDataEnvironmentPrepareVariable()
    if (preservedHostVarRefs.find(vRef) != preservedHostVarRefs.end())
      continue; 
    SgVariableSymbol * orig_sym = vRef->get_symbol();
    if (symbol_map[orig_sym] != NULL)
    {
      result ++;
      vRef->set_symbol(symbol_map[orig_sym]);
    }
  }
  return result;
}

//TODO: move to sageinterface, the current one has wrong reference type, and has undesired effect!!
// grab the list of dimension sizes for an input array type, store them in the vector container
static void getArrayDimensionSizes(const SgArrayType*  array_type, std::vector<SgExpression*>& result)
{
  ROSE_ASSERT (array_type != NULL);

  const SgType* cur_type = array_type;
  do
  {
    ROSE_ASSERT (isSgArrayType(cur_type) != NULL);
    SgExpression* index_exp = isSgArrayType(cur_type)->get_index();
    result.push_back(index_exp); // could be NULL, especially for the first dimension
    cur_type = isSgArrayType(cur_type)->get_base_type();
  }
  while (isSgArrayType(cur_type));
}


//TODO move to SageInterface
// Liao 2/8/2013
// rewrite array reference using multiple-dimension subscripts to a reference using one-dimension subscripts
// e.g. a[i][j] is changed to a[i*col_size +j]
//      a [i][j][k]  is changed to a [(i*col_size + j)*K_size +k]
// The parameter is the array reference expression to be changed     
// Note the array reference expression must be the top one since there will be inner ones for a multi-dimensional array references in AST.
static void linearizeArrayAccess(SgPntrArrRefExp* top_array_ref)
{
  //Sanity check
  // TODO check language compatibility for C/C++ only: row major storage
  ROSE_ASSERT( top_array_ref != NULL);
  //ROSE_ASSERT (top_array_ref->get_lhs_operand_i() != NULL);
  ROSE_ASSERT (top_array_ref->get_parent() != NULL );
  ROSE_ASSERT (isSgPntrArrRefExp(top_array_ref->get_parent()) == NULL ); // top ==> must not be a child of a higher level array ref exp

  // must be a canonical array reference, not like (a+10)[10]
  SgExpression* arrayNameExp = NULL;
  std::vector<SgExpression*>* subscripts = new vector<SgExpression*>;
  bool is_array_ref = isArrayReference (top_array_ref, &arrayNameExp, &subscripts);
  ROSE_ASSERT (is_array_ref);
  SgInitializedName * i_name = convertRefToInitializedName(arrayNameExp);
  ROSE_ASSERT (i_name != NULL);
  SgType* var_type = i_name->get_type();
  SgArrayType * array_type = isSgArrayType(var_type);
  SgPointerType * pointer_type = isSgPointerType(var_type);
  // pointer type can also be used as pointer[i], which is represented as SgPntrArrRefExp.
  // In this case, we don't need to linearized it any more
  if (pointer_type != NULL)
     return; 
  if (array_type == NULL)
  {
    cerr<<"Error. linearizeArrayAccess() found unhandled variable type:"<<var_type->class_name()<<endl;
  }

  ROSE_ASSERT (array_type!= NULL);

  std::vector <SgExpression*> dimensions ; 
  getArrayDimensionSizes (array_type, dimensions);
  
  ROSE_ASSERT ((*subscripts).size() == dimensions.size());
  ROSE_ASSERT ((*subscripts).size()>1); // we only accept 2-D or above for processing. Caller should check this in advance

  // left hand operand
  SgExpression* new_lhs = buildVarRefExp(i_name);
  SgExpression* new_rhs = deepCopy((*subscripts)[0]); //initialized to be i; 

  // build rhs, like (i*col_size + j)*K_size +k
  for (size_t i =1; i<dimensions.size(); i++) // only repeat dimension count -1 times
  {
     new_rhs = buildAddOp( buildMultiplyOp(new_rhs, deepCopy(dimensions[i]) ) , deepCopy((*subscripts)[i]) ) ; 
  }

  // set new lhs and rhs for the top ref
  deepDelete(top_array_ref->get_lhs_operand_i()) ;  
  deepDelete(top_array_ref->get_rhs_operand_i()) ;  

  top_array_ref->set_lhs_operand_i(new_lhs);
  new_lhs->set_parent(top_array_ref);

  top_array_ref->set_rhs_operand_i(new_rhs);
  new_rhs->set_parent(top_array_ref);

}


// Find all top level array references within the body block, 
// we do the following:
//   if it is within the set of arrays (array_syms) to be rewritten: arrays on map() clause,
//   if it is more than 1-D
//   change it to be linearized subscript access
static void rewriteArraySubscripts(SgBasicBlock* body_block, const std::set<SgSymbol*> mapped_array_syms)
{
  std::vector<SgPntrArrRefExp* > candidate_refs; // store eligible references 
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(body_block, V_SgPntrArrRefExp);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgPntrArrRefExp* vRef = isSgPntrArrRefExp((*i));
    ROSE_ASSERT (vRef != NULL);
    SgNode* parent = vRef->get_parent();
    // if it is top level ref?
    if (isSgPntrArrRefExp(parent)) // has a higher level array ref, skip it
      continue;
   //TODO: move this logic into a function in SageInterface   
    // If it is a canonical array reference we can handle?
    vector<SgExpression *>  *subscripts = new vector<SgExpression*>;
    SgExpression* array_name_exp = NULL;
    isArrayReference(vRef, &array_name_exp, &subscripts);
    SgInitializedName* a_name = convertRefToInitializedName (array_name_exp);
    if  (a_name == NULL)
      continue; 
    // if it is within the mapped array set?  
    ROSE_ASSERT (a_name != NULL);
    SgSymbol* array_sym = a_name->get_symbol_from_symbol_table ();
    ROSE_ASSERT (array_sym != NULL);

    if (mapped_array_syms.find(array_sym)!= mapped_array_syms.end()) 
      candidate_refs.push_back(vRef);
  } 

  // To be safe, we use reverse order iteration when changing them
  for (std::vector<SgPntrArrRefExp* >::reverse_iterator riter = candidate_refs.rbegin(); riter != candidate_refs.rend(); riter ++)
  { 
    linearizeArrayAccess (*riter);
  }
}

// Liao, 2/28/2013
// A helper function to collect variables used within a code portion
// To facilitate faster query into the variable collection, we use a map.
// TODO : move to SageInterface ?
std::map <SgVariableSymbol *, bool> collectVariableAppearance (SgNode* root)
{
  std::map <SgVariableSymbol *, bool> result; 
  ROSE_ASSERT (root != NULL);
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(root, V_SgVarRefExp);
  for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
  {
    SgVarRefExp *vRef = isSgVarRefExp((*i));
    SgVariableSymbol * sym = vRef -> get_symbol();
    ROSE_ASSERT (sym != NULL);
    result [sym] = true;
  }
  return result;
}

// find different map clauses from the clause list, and all array information
// dimension map is the same for all the map clauses under the same omp target directive
static void extractMapClauses(Rose_STL_Container<SgOmpClause*> map_clauses, 
    std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > > & array_dimensions,
    SgOmpMapClause** map_alloc_clause, SgOmpMapClause** map_to_clause, SgOmpMapClause** map_from_clause, SgOmpMapClause** map_tofrom_clause
    )
{
  if ( map_clauses.size() == 0) return; // stop if no map clauses at all

#if 0
  // a map between original symbol and its device version (replacement) 
  std::map <SgVariableSymbol*, SgVariableSymbol*>  cpu_gpu_var_map; 

  // store all variables showing up in any of the map clauses
  SgInitializedNamePtrList all_vars ;
  if (target_data_stmt != NULL)
    all_vars = collectClauseVariables (target_data_stmt, VariantVector(V_SgOmpMapClause)); 
  else 
    all_vars = collectClauseVariables (target_directive_stmt, VariantVector(V_SgOmpMapClause));
#endif 
  for (Rose_STL_Container<SgOmpClause*>::const_iterator iter = map_clauses.begin(); iter != map_clauses.end(); iter++)
  {
    SgOmpMapClause* m_cls = isSgOmpMapClause (*iter);
    ROSE_ASSERT (m_cls != NULL);
    if (iter == map_clauses.begin()) // retrieve once is enough
      array_dimensions = m_cls->get_array_dimensions();

    SgOmpClause::omp_map_operator_enum map_operator = m_cls->get_operation();
    if (map_operator == SgOmpClause::e_omp_map_alloc)
      *map_alloc_clause = m_cls;
    else if (map_operator == SgOmpClause::e_omp_map_to)  
      *map_to_clause = m_cls;
    else if (map_operator == SgOmpClause::e_omp_map_from)  
      *map_from_clause = m_cls;
    else if (map_operator == SgOmpClause::e_omp_map_tofrom)  
      *map_tofrom_clause = m_cls;
    else 
    {
      cerr<<"Error. transOmpMapVariables() from omp_lowering.cpp: found unacceptable map operator type:"<< map_operator <<endl;
      ROSE_ASSERT (false);
    }
  }  // end for
}


// Translated a single mapped array variable, knowing the map clauses , where to insert, etc. 
// Only generate memory allocation, deallocation, copy, functions, not the declaration since decl involves too many variable bookkeeping.
// This is intended to be called by a for loop going through all mapped array variables. 
  //  Essentially, we have to decide if we need to do the following steps for each variable
  //
  //  Data handling: declaration, allocation, and copy
  //    1. declared a pointer type to the device copy : pass by pointer type vs. pass by value
  //    2. allocate device copy using the dimension bound info: for array types (pointers used for linearized arrays)
  //    3. copy the data from CPU to the device (GPU) copy: 
  //       
  //    4. replace references to the CPU copies with references to the GPU copy
  //    5. replace original multidimensional element indexing with linearized address indexing (for 2-D and more dimension arrays) 
  //
  //  Data handling: copy back, de-allocation
  //    6. copy GPU_copy back to CPU variables
  //    7. de-allocate the GPU variables
  //
  //   Step 1,2,3 and 6, 7 should generate statements before or after the SgOmpTargetStatement
  //   Step 4 and 5 should change the body of the affected SgOmpParallelStatement
  // Revised Algorithm (version 3)    1/23/2015, optionally use device data environment (DDE) functions to manage data automatically.
  // Instead of generate explicit data allocation, copy, free functions, using the following three DDE functions:
  //   1. xomp_deviceDataEnvironmentEnter()
  //   2. xomp_deviceDataEnvironmentPrepareVariable ()
  //   3. xomp_deviceDataEnvironmentExit()
  // This is necessary to have a consistent translation for mapped data showing up in both "target data" and "target" directives.
  // These DDE functions internally will keep track of data allocated and try to reuse enclosing data environment.
static void generateMappedArrayMemoryHandling(
    /* the array and the map information */
    SgSymbol* sym, 
    SgOmpMapClause* map_alloc_clause, SgOmpMapClause* map_to_clause, SgOmpMapClause* map_from_clause, SgOmpMapClause* map_tofrom_clause, 
    std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > > & array_dimensions, SgExpression* device_expression,
    /*Where to insert generated function calls*/
    SgBasicBlock* insertion_scope, SgStatement* insertion_anchor_stmt, 
    bool need_generate_data_stmt
    )
{
  ROSE_ASSERT (sym != NULL);
  ROSE_ASSERT (device_expression!= NULL); // runtime now needs explicit device ID to work
  SgType* orig_type = sym->get_type();

  // Step 1: declare a pointer type to array variables in map clauses, we linearize all arrays to be a 1-D pointer
  //   Element_type * _dev_var; 
  //   e.g.: double* _dev_array; 
  // I believe that all array variables need allocations on GPUs, regardless their map operations (alloc, to, from, or tofrom)

  // TODO: is this a safe assumption here??
  SgType* element_type = orig_type->findBaseType(); // recursively strip away non-base type to get the bottom type
  string orig_name = (sym->get_name()).getString();
  string dev_var_name = "_dev_"+ orig_name; 

  // Step 2.1  generate linear size calculation based on array dimension info
  // int dev_array_size = sizeof (double) *dim_size1 * dim_size2;
  string dev_var_size_name = "_dev_" + orig_name + "_size";  
  SgVariableDeclaration* dev_var_size_decl = NULL; 

  SgVariableSymbol* dev_var_size_sym = insertion_scope->lookup_variable_symbol(dev_var_size_name);
  std::vector<SgExpression*> v_size;
  if (dev_var_size_sym == NULL)
  {
//    SgExpression* initializer = generateSizeCalculationExpression (sym, element_type, array_dimensions[sym]);
    SgExprListExp* initializer = buildExprListExp();
    for (std::vector < std::pair <SgExpression*, SgExpression*> >::const_iterator iter = array_dimensions[sym].begin(); iter != array_dimensions[sym].end(); iter++)
    {
      std::pair <SgExpression*, SgExpression*> bound_pair = *iter; 
      initializer->append_expression(buildMultiplyOp(buildSizeOfOp(element_type),deepCopy(bound_pair.second)));
      v_size.push_back(deepCopy(bound_pair.second));
    } 
    dev_var_size_decl = buildVariableDeclaration (dev_var_size_name, buildArrayType(buildIntType(),buildIntVal(array_dimensions[sym].size())), buildAggregateInitializer(initializer), insertion_scope); 
    insertStatementBefore (insertion_anchor_stmt, dev_var_size_decl); 
  }
  else
    dev_var_size_decl = isSgVariableDeclaration(dev_var_size_sym->get_declaration()->get_declaration());

  ROSE_ASSERT (dev_var_size_decl != NULL);

  offload_array_size_map[dev_var_name] = v_size;

  // generate offset array
  string dev_var_offset_name = "_dev_" + orig_name + "_offset";  
  SgVariableDeclaration* dev_var_offset_decl = NULL; 

  SgVariableSymbol* dev_var_offset_sym = insertion_scope->lookup_variable_symbol(dev_var_offset_name);
  // vector to store all offset values
  std::vector<SgExpression*> v_offset;
  if (dev_var_offset_sym == NULL)
  {
    SgExprListExp* arrayInitializer = buildExprListExp();
    for (std::vector < std::pair <SgExpression*, SgExpression*> >::const_iterator iter = array_dimensions[sym].begin(); iter != array_dimensions[sym].end(); iter++)
    {
      std::pair <SgExpression*, SgExpression*> bound_pair = *iter; 
      arrayInitializer->append_expression(buildMultiplyOp(buildSizeOfOp(element_type),deepCopy(bound_pair.first)));
      v_offset.push_back(deepCopy(bound_pair.first));
    } 
    dev_var_offset_decl = buildVariableDeclaration (dev_var_offset_name, buildArrayType(buildIntType(),buildIntVal(array_dimensions[sym].size())), buildAggregateInitializer(arrayInitializer), insertion_scope); 
    insertStatementBefore (insertion_anchor_stmt, dev_var_offset_decl); 
  }
  else
    dev_var_offset_decl = isSgVariableDeclaration(dev_var_offset_sym->get_declaration()->get_declaration());

  ROSE_ASSERT (dev_var_offset_decl != NULL);

  offload_array_offset_map[dev_var_name] = v_offset;

  // generate Dim array
  string dev_var_Dim_name = "_dev_" + orig_name + "_Dim";  
  SgVariableDeclaration* dev_var_Dim_decl = NULL; 

  SgVariableSymbol* dev_var_Dim_sym = insertion_scope->lookup_variable_symbol(dev_var_Dim_name);
  if (dev_var_Dim_sym == NULL)
  {
    SgExprListExp* arrayInitializer = buildExprListExp();
    for (std::vector < std::pair <SgExpression*, SgExpression*> >::const_iterator iter = array_dimensions[sym].begin(); iter != array_dimensions[sym].end(); iter++)
    {
      std::pair <SgExpression*, SgExpression*> bound_pair = *iter; 
      arrayInitializer->append_expression(buildMultiplyOp(buildSizeOfOp(element_type),deepCopy(bound_pair.second)));
    } 
    dev_var_Dim_decl = buildVariableDeclaration (dev_var_Dim_name, buildArrayType(buildIntType(),buildIntVal(array_dimensions[sym].size())), buildAggregateInitializer(arrayInitializer), insertion_scope); 
    insertStatementBefore (insertion_anchor_stmt, dev_var_Dim_decl); 
  }
  else
    dev_var_Dim_decl = isSgVariableDeclaration(dev_var_Dim_sym->get_declaration()->get_declaration());

  ROSE_ASSERT (dev_var_Dim_decl != NULL);

  // Only if we are in the mode of inserting data handling statements
  if (!need_generate_data_stmt)
    return; 

  bool needCopyTo = false;
  bool needCopyFrom = false;
  if ( ((map_to_clause) && (isInClauseVariableList (map_to_clause,sym))) || 
      ((map_tofrom_clause) && (isInClauseVariableList (map_tofrom_clause,sym))) )
    needCopyTo = true;  

  if (( (map_from_clause) && (isInClauseVariableList (map_from_clause,sym))) ||
      ( (map_tofrom_clause) && (isInClauseVariableList (map_tofrom_clause,sym))))
    needCopyFrom = true;  

  if (useDDE)
  { 
    // a single function call does all things transparently: reuse first, if not then allocation, copy data
    // e.g. float* _dev_u = (float*) xomp_deviceDataEnvironmentPrepareVariable ((void*)u, _dev_u_size, true, false);
    SgExpression* copyToExp= NULL; 
    SgExpression* copyFromExp = NULL; 
    if (needCopyTo) copyToExp = buildBoolValExp(1);
    else copyToExp = buildBoolValExp(0);

    if (needCopyFrom) copyFromExp = buildBoolValExp(1);
    else copyFromExp = buildBoolValExp(0);

    SgVarRefExp* host_var_ref = buildVarRefExp(isSgVariableSymbol(sym));
    preservedHostVarRefs.insert (host_var_ref);
//cout<<"Debug: inserting var ref to be preserved:"<<sym->get_name()<<"@"<<host_var_ref <<endl;    

    SgExprListExp * parameters =
      buildExprListExp(device_expression, buildCastExp( host_var_ref, buildPointerType(buildVoidType()) ),buildIntVal(array_dimensions[sym].size()), 
          buildVarRefExp( dev_var_size_name, insertion_scope), buildVarRefExp( dev_var_offset_name, insertion_scope),
          buildVarRefExp( dev_var_Dim_name, insertion_scope), copyToExp, copyFromExp
          );

    SgExprStatement* dde_prep_stmt = buildAssignStatement (buildVarRefExp(dev_var_name, insertion_scope),
        buildCastExp ( buildFunctionCallExp(SgName("xomp_deviceDataEnvironmentPrepareVariable"),
            buildPointerType(buildVoidType()),
            parameters, 
            insertion_scope),
          buildPointerType(element_type)));
    insertStatementBefore (insertion_anchor_stmt, dde_prep_stmt); 

    // should not be done here. Only one call for a whole device data environment
    // Now insert xomp_deviceDataEnvironmentEnter() before xomp_deviceDataEnvironmentPrepareVariable()
    //SgExprStatement* dde_enter_stmt = buildFunctionCallStmt (SgName("xomp_deviceDataEnvironmentEnter"), buildVoidType(), NULL, insertion_scope);
   // insertStatementBefore (dde_prep_stmt, dde_enter_stmt); 
  }
  else
  {
    // Step 2.5 generate memory allocation on GPUs
    // e.g.:  _dev_m1 = (double *)xomp_deviceMalloc (_dev_m1_size);
    SgExprStatement* mem_alloc_stmt = buildAssignStatement(buildVarRefExp(dev_var_name, insertion_scope), 
        buildCastExp ( buildFunctionCallExp(SgName("xomp_deviceMalloc"), 
            buildPointerType(buildVoidType()), 
            buildExprListExp(buildVarRefExp( dev_var_size_name, insertion_scope)),
            insertion_scope), 
          buildPointerType(element_type))); 
    insertStatementBefore (insertion_anchor_stmt, mem_alloc_stmt); 

    // Step 3. copy the data from CPU to GPU
    // Only for variable in map(to:), or map(tofrom:) 
    // e.g. xomp_memcpyHostToDevice ((void*)dev_m1, (const void*)a, array_size);
    if (needCopyTo)
    {
      SgExprListExp * parameters = buildExprListExp (
          buildCastExp(buildVarRefExp(dev_var_name, insertion_scope), buildPointerType(buildVoidType())),
          buildCastExp(buildVarRefExp(orig_name, insertion_scope), buildPointerType(buildConstType(buildVoidType())) ),
          buildVarRefExp(dev_var_size_name, insertion_scope)
          );
      SgExprStatement* mem_copy_to_stmt = buildFunctionCallStmt (SgName("xomp_memcpyHostToDevice"), 
          buildPointerType(buildVoidType()),
          parameters,
          insertion_scope);
      insertStatementBefore (insertion_anchor_stmt, mem_copy_to_stmt); 
    }
  }

  if (useDDE)
  { // call xomp_deviceDataEnvironmentExit() and it will automatically copy back data and deallocate.
    //SgExprStatement* dde_exit_stmt = buildFunctionCallStmt (SgName("xomp_deviceDataEnvironmentExit"), buildVoidType(), NULL, insertion_scope);
    // appendStatement(dde_exit_stmt , insertion_anchor_stmt->get_scope()); 
    // do nothing here or we will get multiple exit() for a single DDE.  
  }
  else 
  { // or explicitly control copy back and deallocation
    // Step 6. copy back data from GPU to CPU, only for variable in map(out:var_list)
    // e.g. xomp_memcpyDeviceToHost ((void*)c, (const void*)dev_m3, array_size);
    // Note: insert this AFTER the target directive stmt
    // SgStatement* prev_stmt = target_parallel_stmt;
    if (needCopyFrom)
    {
      SgExprListExp * parameters = buildExprListExp (
          buildCastExp(buildVarRefExp(orig_name, insertion_scope), buildPointerType(buildVoidType()) ),
          buildCastExp(buildVarRefExp(dev_var_name, insertion_scope), buildPointerType(buildConstType(buildVoidType()))),
          buildVarRefExp( dev_var_size_name, insertion_scope)
          );
      SgExprStatement* mem_copy_back_stmt = buildFunctionCallStmt (SgName("xomp_memcpyDeviceToHost"), 
          buildPointerType(buildVoidType()),
          parameters, 
          insertion_scope);
      appendStatement(mem_copy_back_stmt, insertion_anchor_stmt->get_scope()); 
      // prev_stmt = mem_copy_back_stmt;
    }

    // Step 7, de-allocate GPU memory
    // e.g. xomp_freeDevice(dev_m1);
    // Note: insert this AFTER the target directive stmt or the copy back stmt
    SgExprStatement* mem_dealloc_stmt = 
      buildFunctionCallStmt(SgName("xomp_freeDevice"),
          buildBoolType(),
          buildExprListExp(buildVarRefExp( dev_var_name,insertion_scope)),
          insertion_scope);
    appendStatement(mem_dealloc_stmt, insertion_anchor_stmt->get_scope()); 
  }
}

// trans OpenMP map variables
// return all generated or remaining variables to be passed to the outliner  
  // Liao, 2/4/2013
  // Translate the map clause variables associated with "omp target parallel"
  // We only support combined "target parallel" or "parallel" immediately following "target"
  // So we handle outlining and data handling for two directives at the same time
  // TODO: move to the header
  // Input: 
  //
  //  map(alloc|to|from|tofrom:var_list)
  //  array variable in var_list should have dimension bounds information like [0:N-1][0:K-1]
  //  
  //  Essentially, we have to decide if we need to do the following steps for each variable
  //
  //  Data handling: declaration, allocation, and copy
  //    1. declared a pointer type to the device copy : pass by pointer type vs. pass by value
  //    2. allocate device copy using the dimension bound info: for array types (pointers used for linearized arrays)
  //    3. copy the data from CPU to the device (GPU) copy: 
  //       
  //    4. replace references to the CPU copies with references to the GPU copy
  //    5. replace original multidimensional element indexing with linearized address indexing (for 2-D and more dimension arrays) 
  //
  //  Data handling: copy back, de-allocation
  //    6. copy GPU_copy back to CPU variables
  //    7. de-allocate the GPU variables
  //
  //   Step 1,2,3 and 6, 7 should generate statements before or after the SgOmpTargetStatement
  //   Step 4 and 5 should change the body of the affected SgOmpParallelStatement
  //
  //  Algorithm 1: 
  //   collect all variables in map clauses: they should be either scalar or arrays with bound info.
  //   For each array variable, 
  //       we generate memory handling statements for them: declaration, allocation, copy back-forth, de-allocation
  //   For the use of array variable, 
  //       we replace the original references with references to new pointer typed variables
  //       Linearize the access when 2-D or more dimensions are used.
  //                        
  //   Based on the mapped variables, we output the variables to be passed to the outlined function to be generated later on
  //         variables which will be passed by their original data types
  //         variables which will be passed by their address of type: pointer type pointing to their original data type 
  //
  //  Revised Algorithm (version 2):  To translate "omp target" + "omp parallel for" enclosed within "omp target data" region:
  //  New facts:
  //        the map clauses are now associated with "omp target data" instead of "omp target"
  //        Only a subset of all mapped variables at "omp target data" level will be used within "omp target": 
  //           a single data region contains multiple "omp target" regions
  //        When translating "omp target" + "omp parallel for", we don't need to generate data handling statements
  //            but we need to refer to the declarations for device variables.
  //        Memory declaration, allocation, copy back-forth, de-allocation is generated within the body of the "omp target data" region.
  //            we can still try to generate them when translating "omp parallel for" under "omp target", if not yet generated before.
  //
  // Revised Algorithm (V3): using Device Data Environment (DDE) runtime support to manage nested data regions
  //       To simplify the handling, we assume
  //         1. Both "target data"  and "target parallel for " should have map() clauses
  //         2. Using DDE, the translation is simplified as is identical for both directive
ASTtools::VarSymSet_t transOmpMapVariables(SgStatement* target_data_or_target_parallel_stmt  // either "target data" or "target parallel" statement
                   ) 
{
  ASTtools::VarSymSet_t all_syms;
  ROSE_ASSERT  (target_data_or_target_parallel_stmt !=NULL);
  ROSE_ASSERT  (all_syms.size() == 0); // it should be empty

  SgOmpParallelStatement* target_parallel_stmt = NULL; 
  SgOmpTargetStatement* target_directive_stmt = NULL;
  SgOmpTargetDataStatement * target_data_stmt = NULL; 


  target_parallel_stmt = isSgOmpParallelStatement(target_data_or_target_parallel_stmt);
  target_data_stmt = isSgOmpTargetDataStatement(target_data_or_target_parallel_stmt);
 
  // the parallel directive must be combined with target directive
  if (target_parallel_stmt != NULL)
  {
   // must be a parallel region directly under "omp target"
    SgNode* parent = target_parallel_stmt->get_parent();
    ROSE_ASSERT (parent != NULL);
   if (isSgBasicBlock(parent)) //skip the possible block in between
      parent = parent->get_parent();
    target_directive_stmt = isSgOmpTargetStatement(parent);
    ROSE_ASSERT (target_directive_stmt != NULL);
  }

  // collect map clauses and their variables 
  // ----------------------------------------------------------
  // Some notes for the relevant AST input: 
  // we store a map clause for each variant/operator (alloc, to, from, and tofrom), so there should be up to 4 SgOmpMapClause.
  //    SgOmpClause::omp_map_operator_enum
  // each map clause has 
  //   a variable list (SgVarRefExp), accessible through get_variables()
  //   a pointer to array_dimensions, accessible through get_array_dimensions(). the array_dimensions is identical among all map clause of a same "omp target"
  //     std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > >  array_dimensions

  Rose_STL_Container<SgOmpClause*> map_clauses; 
  Rose_STL_Container<SgOmpClause*> device_clauses; 
  if (target_data_stmt)
  {
     map_clauses = getClause(target_data_stmt, V_SgOmpMapClause);
     device_clauses = getClause(target_data_stmt, V_SgOmpDeviceClause);
  }
  else if (target_directive_stmt)
  {
     map_clauses = getClause(target_directive_stmt, V_SgOmpMapClause);
     device_clauses = getClause(target_directive_stmt, V_SgOmpDeviceClause);
  }
  else 
    ROSE_ASSERT (false);

  if ( map_clauses.size() == 0) return all_syms; // stop if no map clauses at all

  // store each time of map clause explicitly
  SgOmpMapClause* map_alloc_clause = NULL;
  SgOmpMapClause* map_to_clause = NULL;
  SgOmpMapClause* map_from_clause = NULL;
  SgOmpMapClause* map_tofrom_clause = NULL;
  // dimension map is the same for all the map clauses under the same omp target directive
  std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > >  array_dimensions; 

  // a map between original symbol and its device version : used for variable replacement 
  std::map <SgVariableSymbol*, SgVariableSymbol*>  cpu_gpu_var_map; 

  // store all variables showing up in any of the map clauses
  SgInitializedNamePtrList all_mapped_vars ;
  if (target_data_stmt)
    all_mapped_vars = collectClauseVariables (target_data_stmt, VariantVector(V_SgOmpMapClause)); 
  else if (target_directive_stmt)
    all_mapped_vars = collectClauseVariables (target_directive_stmt, VariantVector(V_SgOmpMapClause)); 

  // store all variables showing up in any of the device clauses
  SgExpression* device_expression ;
  if (target_data_stmt)
    device_expression = getClauseExpression (target_data_stmt, VariantVector(V_SgOmpDeviceClause)); 
  else if (target_directive_stmt)
    device_expression = getClauseExpression (target_directive_stmt, VariantVector(V_SgOmpDeviceClause));

 
  extractMapClauses (map_clauses, array_dimensions, &map_alloc_clause, &map_to_clause, &map_from_clause, &map_tofrom_clause);
  std::set<SgSymbol*> array_syms; // store clause variable symbols which are array types (explicit or as a pointer)
  std::set<SgSymbol*> atom_syms; // store clause variable symbols which are non-aggregate types: scalar, pointer, etc

  // categorize the variables:
  categorizeMapClauseVariables (all_mapped_vars, array_dimensions, array_syms, atom_syms);

  // set the scope and anchor statement we will focus on based on the availability of an enclosing target data region
  SgBasicBlock* insertion_scope = NULL; // the body 
  SgStatement* insertion_anchor_stmt = NULL; // the single statement within the body
 if (target_data_stmt != NULL)
 {
   // at this point, the body should already be normalized to be a BB
   SgBasicBlock * body_block = ensureBasicBlockAsBodyOfOmpBodyStmt(target_data_stmt);
   ROSE_ASSERT (body_block!= NULL);

   SgStatement* target_data_child_stmt = NULL;
   // We cannot assert this since the body of "omp target data" may already be expanded as part of a previous translation    
   //    ROSE_ASSERT( (target_data_stmt_body->get_statements()).size() ==1);
   target_data_child_stmt = (body_block->get_statements())[0];

   insertion_scope = body_block;
   insertion_anchor_stmt = target_data_child_stmt;
 }
  else if (target_directive_stmt != NULL)
  {
   insertion_scope= isSgBasicBlock(target_directive_stmt->get_body());
   insertion_anchor_stmt = target_parallel_stmt;
  }

  ROSE_ASSERT (insertion_scope!= NULL);
  ROSE_ASSERT (insertion_anchor_stmt!= NULL);

  // collect used variables in the insertion scope
  std::map <SgVariableSymbol *, bool> variable_map = collectVariableAppearance (insertion_scope);

  // Now insert xomp_deviceDataEnvironmentEnter() before xomp_deviceDataEnvironmentPrepareVariable()
  SgExprListExp* argumentList = NULL;
  if(device_expression)
  {
    argumentList = buildExprListExp(deepCopy(device_expression)); 
  }
  else  // use default device ID 0 if device_expression is NULL
  {
    device_expression = buildIntVal(0);
    argumentList = buildExprListExp(device_expression);
  }

  SgExprStatement* dde_enter_stmt = buildFunctionCallStmt (SgName("xomp_deviceDataEnvironmentEnter"), buildVoidType(), argumentList, insertion_scope);
  prependStatement(dde_enter_stmt, insertion_scope); 

  // handle array variables showing up in the map clauses:   
  for (std::set<SgSymbol*>::const_iterator iter = array_syms.begin(); iter != array_syms.end(); iter ++)
  {
    SgSymbol* sym = *iter; 
    ROSE_ASSERT (sym != NULL);
    SgType* orig_type = sym->get_type();

    // Step 1: declare a pointer type to array variables in map clauses, we linearize all arrays to be a 1-D pointer
    //   Element_type * _dev_var; 
    //   e.g.: double* _dev_array; 
    // I believe that all array variables need allocations on GPUs, regardless their map operations (alloc, to, from, or tofrom)

    // TODO: is this a safe assumption here??
    SgType* element_type = orig_type->findBaseType(); // recursively strip away non-base type to get the bottom type
    string orig_name = (sym->get_name()).getString();
    string dev_var_name = "_dev_"+ orig_name; 

    SgVariableDeclaration* dev_var_decl = NULL; 
    dev_var_decl = buildVariableDeclaration(dev_var_name, buildPointerType(element_type), NULL, insertion_scope);
    insertStatementBefore (insertion_anchor_stmt, dev_var_decl); 
    ROSE_ASSERT (dev_var_decl != NULL);

    SgVariableSymbol* orig_sym = isSgVariableSymbol(sym);
    ROSE_ASSERT (orig_sym != NULL);
    SgVariableSymbol* new_sym = getFirstVarSym(dev_var_decl);
    cpu_gpu_var_map[orig_sym]= new_sym; // store the mapping, this is always needed to guide the outlining

    // Not all map variables from "omp target data" will be used within the current parallel region
    // We only need to find out the used one only.

    // linearized array pointers should be directly passed to the outliner later on, without adding & operator in front of them
    // we assume AST is normalized and all target regions have explicit and correct map() clauses
    // Still some transformation like loop collapse will change the variables
        if (variable_map[orig_sym])
          all_syms.insert(new_sym);
    // generate memory allocation, copy, free function calls.
    generateMappedArrayMemoryHandling (sym, map_alloc_clause, map_to_clause, map_from_clause, map_tofrom_clause,array_dimensions, device_expression, 
        insertion_scope, insertion_anchor_stmt, true);
  }  // end for

  // Generate a single DDE enter() call
  SgExprStatement* dde_exit_stmt = buildFunctionCallStmt (SgName("xomp_deviceDataEnvironmentExit"), buildVoidType(), argumentList, insertion_scope);
  appendStatement(dde_exit_stmt , insertion_anchor_stmt->get_scope()); 

  // Step 5. TODO  replace indexing element access with address calculation (only needed for 2/3 -D)
  // We switch the order of 4 and 5 since we want to rewrite the subscripts before the arrays are replaced
  rewriteArraySubscripts (insertion_scope, array_syms); 

  // Step 4. replace references to old with new variables, 
  replaceVariableReferences (insertion_scope , cpu_gpu_var_map);

  // TODO handle scalar, separate or merged into previous loop ?

  // store remaining variables so outliner can readily use this information
  // for pointers to linearized arrays, they should passed by their original form, not using & operator, regardless the map operator types (to|from|alloc|tofrom)
  // for a scalar, two cases: to vs. from | tofrom
  // if in only, pass by value is good
  // if either from or tofrom:  
  // two possible solutions:
  // 1) we need to treat it as an array of size 1 or any other choices. TODO!!
  //  we also have to replace the reference to scalar to the array element access: be cautious about using by value (a) vs. using by address  (&a)
  // 2) try to still pass by value, but copy the final value back to the CPU version 
  // right now we assume they are not on from|tofrom, until we face a real input applications with map(from:scalar_a)
  // For all scalars, we directly copy them into all_syms for now
  for (std::set<SgSymbol*> ::iterator iter = atom_syms.begin(); iter != atom_syms.end(); iter ++)
  {
    SgVariableSymbol * var_sym = isSgVariableSymbol(*iter);
    if (variable_map[var_sym] == true) // we should only collect map variables which show up in the current parallel region
      all_syms.insert (var_sym);
  }

  //Pei-Hung: subtract offset from the subscript in the offloaded array reference
  if(target_parallel_stmt)
  {
    // at this point, the body must be a BB now.
    SgBasicBlock* body_block = isSgBasicBlock(target_parallel_stmt->get_body()); // the body of the affected "omp parallel"
    ROSE_ASSERT (body_block!= NULL);
    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(body_block, V_SgVarRefExp);
    for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
    {
      SgVarRefExp *vRef = isSgVarRefExp((*i));
      SgVariableSymbol* sym = vRef->get_symbol();
      SgType* type = sym->get_type();
      if(offload_array_offset_map.find(vRef->get_symbol()->get_name().getString()) != offload_array_offset_map.end())
      {
        std::vector<SgExpression*> v_offset = offload_array_offset_map.find(vRef->get_symbol()->get_name().getString())->second;
        std::vector<SgExpression*> v_size = offload_array_size_map.find(vRef->get_symbol()->get_name().getString())->second;
        if(isSgPntrArrRefExp(vRef->get_parent()) == NULL)
          continue;
        //std::cout << "finding susbscript " << vRef->get_symbol()->get_name().getString() << " in " << offload_array_offset_map.size() << std::endl;
        SgPntrArrRefExp* pntrArrRef = isSgPntrArrRefExp(vRef->get_parent());
        std::vector<SgExpression*> arrayType =get_C_array_dimensions(type);
        //std::cout << "vector size = " << v_offset.size() << " array dim= " << arrayType.size() << std::endl;
        if(v_offset.size() == arrayType.size())
        {
          for(std::vector<SgExpression*>::reverse_iterator ir = v_offset.rbegin(); ir != v_offset.rend(); ir++)
          {
            ROSE_ASSERT(pntrArrRef);
            SgExpression* subscript = pntrArrRef->get_rhs_operand();  
            SgExpression* newsubscript = buildSubtractOp(deepCopy(subscript),deepCopy(*ir));
            replaceExpression(subscript,newsubscript,true); 
            pntrArrRef = isSgPntrArrRefExp(pntrArrRef->get_parent()); 
          } 
        }
        // collapsed case
        else
        {
          ROSE_ASSERT(pntrArrRef);
          SgExpression* subscript = pntrArrRef->get_rhs_operand();  
          SgExpression* newsubscript = deepCopy(subscript);
          std::vector<SgExpression*>::reverse_iterator irsize = v_size.rbegin();
          for(std::vector<SgExpression*>::reverse_iterator ir = v_offset.rbegin(); ir != v_offset.rend(); ir++)
          {
            if(ir ==v_offset.rbegin())
              newsubscript = buildSubtractOp(newsubscript,deepCopy(*ir));
            else
              newsubscript = buildSubtractOp(newsubscript,buildMultiplyOp(deepCopy(*ir),deepCopy(*irsize)));
            irsize++;  
          }
          replaceExpression(subscript,newsubscript,true); 
          pntrArrRef = isSgPntrArrRefExp(pntrArrRef->get_parent()); 
        }
      }
    }
  }
  return all_syms;
} // end transOmpMapVariables() for omp target data's map clauses for now


// old version: kept to occasionally see previous translation results.
// generating explicit data handling function calls
 void transOmpMapVariables_v1(
                         SgOmpParallelStatement* target_parallel_stmt, //the "omp parallel" with enclosing "omp target", or combined "target parallel"
                         ASTtools::VarSymSet_t & all_syms // collect all generated or remaining variables to be passed to the outliner
                        )
  {
    ROSE_ASSERT (target_parallel_stmt!= NULL);
    
   // must be a parallel region directly under "omp target"
    SgNode* parent = target_parallel_stmt->get_parent();
    ROSE_ASSERT (parent != NULL);
   if (isSgBasicBlock(parent)) //skip the possible block in between
      parent = parent->get_parent();
    SgOmpTargetStatement* target_directive_stmt = isSgOmpTargetStatement(parent);
    ROSE_ASSERT (target_directive_stmt != NULL);

   // at this point, the body must be a BB now.
   SgBasicBlock* body_block = isSgBasicBlock(target_parallel_stmt->get_body()); // the body of the affected "omp parallel"
    ROSE_ASSERT (body_block!= NULL);
   // we should use inner scope , instead of the scope of target-directive-stmt.
   // this will avoid name collisions when there are multiple "omp target" within one big scope
    SgScopeStatement* target_directive_body = isSgScopeStatement(target_directive_stmt->get_body()); 
    ROSE_ASSERT (target_directive_body != NULL);
    ROSE_ASSERT (isSgBasicBlock (target_directive_body));

    std::map <SgVariableSymbol *, bool> variable_map = collectVariableAppearance (target_parallel_stmt);
#if 1
    // two cases: map variables are provided by 
    // case 1: "omp target" 
    // case 2: "omp target data"
    // we have to figure which case is true for the current affected "omp parallel"
    SgOmpTargetDataStatement* target_data_stmt = getEnclosingNode <SgOmpTargetDataStatement> (target_directive_stmt);
    SgBasicBlock * target_data_stmt_body = NULL;
    SgStatement* target_data_child_stmt = NULL;
    if (!useDDE) // only in the old, explicit data management mode, we translate map variables differently depending on context
      if (target_data_stmt != NULL)
      {
        target_data_stmt_body = ensureBasicBlockAsBodyOfOmpBodyStmt (target_data_stmt);
        ROSE_ASSERT (target_data_stmt_body != NULL);
        // We cannot assert this since the body of "omp target data" may already be expanded as part of a previous translation    
        //    ROSE_ASSERT( (target_data_stmt_body->get_statements()).size() ==1);
        target_data_child_stmt = (target_data_stmt_body->get_statements())[0];
      }
#endif
    // collect map clauses and their variables 
    // ----------------------------------------------------------
    // Some notes for the relevant AST input: 
    // we store a map clause for each variant/operator (alloc, to, from, and tofrom), so there should be up to 4 SgOmpMapClause.
    //    SgOmpClause::omp_map_operator_enum
    // each map clause has 
    //   a variable list (SgVarRefExp), accessible through get_variables()
    //   a pointer to array_dimensions, accessible through get_array_dimensions(). the array_dimensions is identical among all map clause of a same "omp target"
    //     std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > >  array_dimensions
    Rose_STL_Container<SgOmpClause*> map_clauses; 
#if 1
    if (target_data_stmt != NULL && !useDDE)
    {
      map_clauses = getClause(target_data_stmt, V_SgOmpMapClause);
    }
    else   
#endif
     map_clauses = getClause(target_directive_stmt, V_SgOmpMapClause);

    if ( map_clauses.size() == 0) return; // stop if no map clauses at all

    // store each time of map clause explicitly
    SgOmpMapClause* map_alloc_clause = NULL;
    SgOmpMapClause* map_to_clause = NULL;
    SgOmpMapClause* map_from_clause = NULL;
    SgOmpMapClause* map_tofrom_clause = NULL;
    // dimension map is the same for all the map clauses under the same omp target directive
    std::map<SgSymbol*,  std::vector < std::pair <SgExpression*, SgExpression*> > >  array_dimensions; 

   // a map between original symbol and its device version (replacement) 
   std::map <SgVariableSymbol*, SgVariableSymbol*>  cpu_gpu_var_map; 
    
    // store all variables showing up in any of the map clauses
    SgInitializedNamePtrList all_mapped_vars ;
#if 1
    if (target_data_stmt != NULL && !useDDE)
      all_mapped_vars = collectClauseVariables (target_data_stmt, VariantVector(V_SgOmpMapClause)); 
    else 
#endif
  all_mapped_vars = collectClauseVariables (target_directive_stmt, VariantVector(V_SgOmpMapClause));

    extractMapClauses (map_clauses, array_dimensions, &map_alloc_clause, &map_to_clause, &map_from_clause, &map_tofrom_clause);
    std::set<SgSymbol*> array_syms; // store clause variable symbols which are array types (explicit or as a pointer)
    std::set<SgSymbol*> atom_syms; // store clause variable symbols which are non-aggregate types: scalar, pointer, etc

   // categorize the variables:
   categorizeMapClauseVariables (all_mapped_vars, array_dimensions, array_syms, atom_syms);

   // set the scope and anchor statement we will focus on based on the availability of an enclosing target data region
   SgBasicBlock* insertion_scope = NULL; // the body 
   SgStatement* insertion_anchor_stmt = NULL; // the single statement within the body
   bool need_generate_data_stmt  = false; // We don't always need to generate the declaration, allocation, copy, de-allocation of device data.
#if 1
   if (target_data_stmt != NULL && !useDDE)
   {
     insertion_scope = target_data_stmt_body;
     insertion_anchor_stmt = target_data_child_stmt;
   } 
   else
#endif
   {
     insertion_scope = isSgBasicBlock(target_directive_body);
     insertion_anchor_stmt = target_parallel_stmt;
   }
   ROSE_ASSERT (insertion_scope!= NULL);
   ROSE_ASSERT (insertion_anchor_stmt!= NULL);

  if (useDDE)
  {
    // Now insert xomp_deviceDataEnvironmentEnter() before xomp_deviceDataEnvironmentPrepareVariable()
    SgExprStatement* dde_enter_stmt = buildFunctionCallStmt (SgName("xomp_deviceDataEnvironmentEnter"), buildVoidType(), NULL, insertion_scope);
    prependStatement(dde_enter_stmt, insertion_scope); 
   }
  // handle array variables showing up in the map clauses:   
  for (std::set<SgSymbol*>::const_iterator iter = array_syms.begin(); iter != array_syms.end(); iter ++)
  {
    SgSymbol* sym = *iter; 
    ROSE_ASSERT (sym != NULL);
    SgType* orig_type = sym->get_type();

    // Step 1: declare a pointer type to array variables in map clauses, we linearize all arrays to be a 1-D pointer
    //   Element_type * _dev_var; 
    //   e.g.: double* _dev_array; 
    // I believe that all array variables need allocations on GPUs, regardless their map operations (alloc, to, from, or tofrom)

    // TODO: is this a safe assumption here??
    SgType* element_type = orig_type->findBaseType(); // recursively strip away non-base type to get the bottom type
    string orig_name = (sym->get_name()).getString();
    string dev_var_name = "_dev_"+ orig_name; 

    // Again, two cases: map clauses come from 1) omp target vs. 2) omp target data 
    // For the combined "omp target" + "omp parallel for " code portion
    // We generate declarations within the body of "omp target". So we don't concerned about name conflicts. 
    //
    // For "omp target data", 
    // It is possible that there are two consecutive "omp target"+"omp parallel for" regions.
    // Blindly generate data handling statements will introduce redundant definition and handling. 
    // So we have to check the existence of a declaration before creating a brand new one.
    SgVariableDeclaration* dev_var_decl = NULL; 
    SgVariableSymbol* dev_var_sym = insertion_scope ->lookup_variable_symbol(dev_var_name);
    if (dev_var_sym == NULL)
    {
      need_generate_data_stmt = true; // this will trigger a set to data handling statements to be generated later on
      dev_var_decl = buildVariableDeclaration(dev_var_name, buildPointerType(element_type), NULL, insertion_scope);
      insertStatementBefore (insertion_anchor_stmt, dev_var_decl); 
    }
    else 
      dev_var_decl = isSgVariableDeclaration(dev_var_sym->get_declaration()->get_declaration());

    ROSE_ASSERT (dev_var_decl != NULL);

    SgVariableSymbol* orig_sym = isSgVariableSymbol(sym);
    ROSE_ASSERT (orig_sym != NULL);
    SgVariableSymbol* new_sym = getFirstVarSym(dev_var_decl);
    cpu_gpu_var_map[orig_sym]= new_sym; // store the mapping, this is always needed to guide the outlining

    // Not all map variables from "omp target data" will be used within the current parallel region
    // We only need to find out the used one only.
#if 1
    // linearized array pointers should be directly passed to the outliner later on, without adding & operator in front of them
    if (target_data_stmt != NULL && !useDDE)
    {
      if (variable_map[orig_sym]) // this condition may miss out the temp variables generated for loop collapsing in DDE translation mode
        all_syms.insert(new_sym);
    }
    else
#endif
    {
      all_syms.insert(new_sym);
      ROSE_ASSERT (variable_map[orig_sym] == true);// the map variable must show up within the parallel region
    }

    // generate memory allocation, copy, free function calls.
    generateMappedArrayMemoryHandling (sym, map_alloc_clause, map_to_clause, map_from_clause, map_tofrom_clause,array_dimensions, NULL,  
        insertion_scope, insertion_anchor_stmt, need_generate_data_stmt);
  }  // end for

  // Generate a single DDE enter() call
  if (useDDE)
  {
    SgExprStatement* dde_exit_stmt = buildFunctionCallStmt (SgName("xomp_deviceDataEnvironmentExit"), buildVoidType(), NULL, insertion_scope);
    appendStatement(dde_exit_stmt , insertion_anchor_stmt->get_scope()); 
  }

   // Step 5. TODO  replace indexing element access with address calculation (only needed for 2/3 -D)
   // We switch the order of 4 and 5 since we want to rewrite the subscripts before the arrays are replaced
    rewriteArraySubscripts (body_block, array_syms); 
   
   // Step 4. replace references to old with new variables, 
    replaceVariableReferences (body_block, cpu_gpu_var_map);

   // TODO handle scalar, separate or merged into previous loop ?
    
   // prepare things  for outliner: 
  // store remaining variables so outliner can readily use this information
   // for pointers to linearized arrays, they should passed by their original form, not using & operator, regardless the map operator types (to|from|alloc|tofrom)
   // for a scalar, two cases: to vs. from | tofrom
   // if in only, pass by value is good
   // if either from or tofrom:  
   // two possible solutions:
   // 1) we need to treat it as an array of size 1 or any other choices. TODO!!
   //  we also have to replace the reference to scalar to the array element access: be cautious about using by value (a) vs. using by address  (&a)
   // 2) try to still pass by value, but copy the final value back to the CPU version 
   // right now we assume they are not on from|tofrom, until we face a real input applications with map(from:scalar_a)
   // For all scalars, we directly copy them into all_syms for now
   for (std::set<SgSymbol*> ::iterator iter = atom_syms.begin(); iter != atom_syms.end(); iter ++)
   {
     SgVariableSymbol * var_sym = isSgVariableSymbol(*iter);
     if (variable_map[var_sym] == true) // we should only collect map variables which show up in the current parallel region
     all_syms.insert (var_sym);
   }

  } // end transOmpMapVariables()

  // Translate a parallel region under "omp target"
  /*
    
   call customized outlining, the generateTask() for omp task or regular omp parallel is not compatible
   since we want to use the classic outlining support: each variable is passed as a separate parameter.

   We also use the revised generateFunc() to explicitly specify pass by original type vs. pass using pointer type

   */
  void transOmpTargetParallel (SgNode* node)
  {
    // Sanity check first
    ROSE_ASSERT(node != NULL);
    SgOmpParallelStatement* target = isSgOmpParallelStatement(node);
    ROSE_ASSERT (target != NULL);

    // must be a parallel region directly under "omp target"
    SgNode* parent = node->get_parent();
    ROSE_ASSERT (parent != NULL);
   if (isSgBasicBlock(parent)) //skip the possible block in between
      parent = parent->get_parent();
    SgOmpTargetStatement* target_directive_stmt = isSgOmpTargetStatement(parent);
    ROSE_ASSERT (target_directive_stmt != NULL);

    // device expression 
    SgExpression* device_expression =NULL ;
    device_expression = getClauseExpression (target_directive_stmt, VariantVector(V_SgOmpDeviceClause));
    // If not found, use the default ID 0
    if (device_expression == NULL)
      device_expression = buildIntVal(0); 

    // Now we need to ensure that "omp target " has a basic block as its body
   // so we can insert declarations into an inner block, instead of colliding declarations within the scope of "omp target"
   // This is important since we often have consecutive "omp target" regions within one big scope
   // We cannot just insert things into that big scope.
    SgBasicBlock* omp_target_stmt_body_block = ensureBasicBlockAsBodyOfOmpBodyStmt (target_directive_stmt);
    ROSE_ASSERT (isSgBasicBlock(target_directive_stmt->get_body()));
    ROSE_ASSERT (node->get_parent() == target_directive_stmt->get_body()); // OMP PARALLEL should be within the body block now

//    SgFunctionDefinition * func_def = NULL;

    // For Fortran code, we have to insert EXTERNAL OUTLINED_FUNC into 
    // the function body containing the parallel region
#if 0
    if (SageInterface::is_Fortran_language() )
    {
      cerr<<"Error. transOmpTargetParallel() does not support Fortran yet. "<<endl; 
      ROSE_ASSERT (false);
      func_def = getEnclosingFunctionDefinition(target);
      ROSE_ASSERT (func_def != NULL);
    }
#endif
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
    // Save preprocessing info as early as possible, avoiding mess up from the outliner
    AttachedPreprocessingInfoType save_buf1, save_buf2, save_buf_inside;
    cutPreprocessingInfo(target, PreprocessingInfo::before, save_buf1) ;
    cutPreprocessingInfo(target, PreprocessingInfo::after, save_buf2) ;

    // 1/15/2009, Liao, also handle the last #endif, which is attached inside of the target
    cutPreprocessingInfo(target, PreprocessingInfo::inside, save_buf_inside) ;

    //-----------------------------------------------------------------
    // step 1: generated an outlined function and make it a CUDA function
    SgOmpClauseBodyStatement * target_parallel_stmt = isSgOmpClauseBodyStatement(node);
    ROSE_ASSERT (target_parallel_stmt);

    // Prepare the outliner
    Outliner::enable_classic = true;
//    Outliner::useParameterWrapper = false; //TODO: better handling of the dependence among flags
    SgBasicBlock* body_block = Outliner::preprocess(body);
    // translator OpenMP 3.0 and earlier variables.
    transOmpVariables (target, body_block);

    ASTtools::VarSymSet_t all_syms; // all generated or remaining variables to be passed to the outliner
  // This addressOf_syms does not apply to CUDA kernel generation: since we cannot use pass-by-reference for CUDA kernel.
  // If we want to copy back value, we have to use memory copy  since they are in two different memory spaces. 
    ASTtools::VarSymSet_t addressOf_syms; // generated or remaining variables should be passed by using their addresses

    if (!useDDE)
    {   
      //transOmpMapVariables (target_directive_stmt, target, body_block, all_syms); //, addressOf_syms);
      transOmpMapVariables_v1 (target, all_syms); //, addressOf_syms);
    } else
      all_syms = transOmpMapVariables (target); //, addressOf_syms);

    ASTtools::VarSymSet_t per_block_reduction_syms; // translation generated per block reduction symbols with name like _dev_per_block within the enclosed for loop

    // collect possible per block reduction variables introduced by transOmpTargetLoop()
    // we rely on the pattern of such variables: _dev_per_block_*     
    // these variables are arrays already, we pass them by their original types, not addressOf types
    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(body_block,V_SgVarRefExp);
    for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
    {
      SgVarRefExp *vRef = isSgVarRefExp((*i));
      SgName var_name = vRef-> get_symbol()->get_name();
      string var_name_str = var_name.getString();
      if (var_name_str.find("_dev_per_block_",0) == 0)
      {
        all_syms.insert( vRef-> get_symbol());
        per_block_reduction_syms.insert (vRef-> get_symbol());
      }
    }

    string func_name = Outliner::generateFuncName(target);
    SgGlobal* g_scope = SageInterface::getGlobalScope(body_block);
    ROSE_ASSERT(g_scope != NULL);

    std::set< SgInitializedName *> restoreVars;
    SgFunctionDeclaration* result = Outliner::generateFunction(body_block, func_name, all_syms, addressOf_syms, restoreVars, NULL, g_scope);
    SgFunctionDeclaration* result_decl = isSgFunctionDeclaration(result->get_firstNondefiningDeclaration());
    ROSE_ASSERT (result_decl != NULL);
    result_decl->get_functionModifier().setCudaKernel(); // add __global__ modifier

    result->get_functionModifier().setCudaKernel();

     // This one is not desired. It inserts the function to the end and prepend a prototype
    // Outliner::insert(result, g_scope, body_block); 
    // TODO: better interface to specify where exactly to insert the function!
    //Custom insertion:  insert right before the enclosing function of "omp target"
    SgFunctionDeclaration* target_func = const_cast<SgFunctionDeclaration *>
       (SageInterface::getEnclosingFunctionDeclaration (target));
     ROSE_ASSERT(target_func!= NULL);
    insertStatementBefore (target_func, result);
    // TODO: this really should be done within Outliner::generateFunction()
    // TODO: we have to patch up first nondefining function declaration since custom insertion is used
    SgGlobal* glob_scope = getGlobalScope(target);
    ROSE_ASSERT (glob_scope!= NULL);
    SgFunctionSymbol * func_symbol = glob_scope->lookup_function_symbol(result->get_name());
    ROSE_ASSERT (func_symbol != NULL);
    //SgFunctionDeclaration * proto_decl = func_symbol->get_declaration();
    //ROSE_ASSERT (proto_decl != NULL);
    //ROSE_ASSERT (proto_decl != result );
    //result->set_firstNondefiningDeclaration(proto_decl);


#if 0 // it turns out we don't need satic keyword for CUDA kernel
    if (result->get_definingDeclaration() != NULL)
      SageInterface::setStatic(result->get_definingDeclaration());
    if (result->get_firstNondefiningDeclaration() != NULL)
      SageInterface::setStatic(result->get_firstNondefiningDeclaration());
#endif 

    //SgScopeStatement * p_scope = target_directive_stmt ->get_scope(); // the scope of "omp parallel" will be destroyed later, so we use scope of "omp target"
    SgScopeStatement * p_scope = omp_target_stmt_body_block ; // the scope of "omp parallel" will be destroyed later, so we use scope of "omp target"
    ROSE_ASSERT(p_scope != NULL);
   // insert dim3 threadsPerBlock(xomp_get_maxThreadsPerBlock()); 
   // TODO: for 1-D mapping, int type is enough,  //TODO: a better interface accepting expression as initializer!!
    SgVariableDeclaration* threads_per_block_decl = buildVariableDeclaration ("_threads_per_block_", buildIntType(), 
                  buildAssignInitializer(buildFunctionCallExp("xomp_get_maxThreadsPerBlock",buildIntType(), buildExprListExp(device_expression), p_scope)), 
                  p_scope);
    //insertStatementBefore (target_directive_stmt, threads_per_block_decl);
    insertStatementBefore (target, threads_per_block_decl);
    attachComment(threads_per_block_decl, string("Launch CUDA kernel ..."));

    // dim3 numBlocks (xomp_get_max1DBlock(VEC_LEN));
    // TODO: handle 2-D or 3-D using dim type
    ROSE_ASSERT (cuda_loop_iter_count_1 != NULL);
    SgVariableDeclaration* num_blocks_decl = buildVariableDeclaration ("_num_blocks_", buildIntType(), 
                  buildAssignInitializer(buildFunctionCallExp("xomp_get_max1DBlock",buildIntType(), buildExprListExp(device_expression, cuda_loop_iter_count_1), p_scope)),
                  p_scope);
    //insertStatementBefore (target_directive_stmt, num_blocks_decl);
    insertStatementBefore (target, num_blocks_decl);

    // Now we have num_block declaration, we can insert the per block declaration used for reduction variables
    SgExpression* shared_data = NULL; // shared data size expression for CUDA kernel execution configuration
    for (std::vector<SgVariableDeclaration*>::iterator iter = per_block_declarations.begin(); iter != per_block_declarations.end(); iter++)
    {
       SgVariableDeclaration* decl = *iter;
       insertStatementAfter (num_blocks_decl, decl);
       SgVariableSymbol* sym = getFirstVarSym (decl);
       SgPointerType * pointer_type = isSgPointerType(sym->get_type());
       ROSE_ASSERT (pointer_type != NULL);
       SgType* base_type = pointer_type->get_base_type();
       if (per_block_declarations.size()>1)
       {
        cerr<<"Error. multiple reduction variables are not yet handled."<<endl;
         ROSE_ASSERT (false);
         // threadsPerBlock.x*sizeof(REAL)  //TODO: how to handle multiple shared data blocks, each for a reduction variable??   
       }
       shared_data = buildMultiplyOp (buildVarRefExp(threads_per_block_decl), buildSizeOfOp (base_type) );
    }

    // generate the cuda kernel launch statement
    //e.g.  axpy_ompacc_cuda <<<numBlocks, threadsPerBlock>>>(dev_x,  dev_y, VEC_LEN, a);
   
    //func_symbol = isSgFunctionSymbol(result->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table ());
    ROSE_ASSERT (func_symbol != NULL);
    SgExprListExp* exp_list_exp = SageBuilder::buildExprListExp();

    std::set<SgInitializedName*>  varsUsingOriginalForm; 
    for (ASTtools::VarSymSet_t::const_iterator iter = all_syms.begin(); iter != all_syms.end(); iter ++)
    {
      const SgVariableSymbol * current_symbol = *iter;
  // this addressOf_syms does not apply to CUDA kernel generation: since we cannot use pass-by-reference for CUDA kernel.
  // If we want to copy back value, we have to use memory copy  since they are in two different memory spaces. 
  // So all variables should use original form in this context. 
      if (addressOf_syms.find(current_symbol) == addressOf_syms.end()) // not found in Address Of variable set
        varsUsingOriginalForm.insert (current_symbol->get_declaration());
    }
    // TODO: alternative mirror form using varUsingAddress as parameter
    Outliner::appendIndividualFunctionCallArgs (all_syms, varsUsingOriginalForm, exp_list_exp);
    // TODO: builder interface without _nfi, and match function call exp builder interface convention: 

    SgCudaKernelExecConfig * cuda_exe_conf = buildCudaKernelExecConfig_nfi (buildVarRefExp(num_blocks_decl), buildVarRefExp(threads_per_block_decl), shared_data, NULL);
    setOneSourcePositionForTransformation (cuda_exe_conf);
    // SgExpression* is not clear, change to SgFunctionRefExp at least!!
    SgExprStatement* cuda_call_stmt = buildExprStatement(buildCudaKernelCallExp_nfi (buildFunctionRefExp(result), exp_list_exp, cuda_exe_conf) );
    setSourcePositionForTransformation (cuda_call_stmt);
    //insertStatementBefore (target_directive_stmt, cuda_call_stmt);
    insertStatementBefore (target, cuda_call_stmt);

   // insert the beyond block level reduction statement
   // error = xomp_beyond_block_reduction_float (per_block_results, numBlocks.x, XOMP_REDUCTION_PLUS);
    for (ASTtools::VarSymSet_t::const_iterator iter = per_block_reduction_syms.begin(); iter != per_block_reduction_syms.end(); iter ++)
    {
      const SgVariableSymbol * current_symbol = *iter;
      SgPointerType* pointer_type = isSgPointerType(current_symbol->get_type());// must be a pointer to simple type
      ROSE_ASSERT (pointer_type != NULL);
      SgType * orig_type = pointer_type->get_base_type();
      ROSE_ASSERT (orig_type != NULL);

      string per_block_var_name = (current_symbol->get_name()).getString();
      // get the original var name by stripping of the leading "_dev_per_block_"
      string leading_pattern = string("_dev_per_block_");
      string orig_var_name = per_block_var_name.substr(leading_pattern.length(), per_block_var_name.length() - leading_pattern.length());
//      cout<<"debug: "<<per_block_var_name <<" after "<< orig_var_name <<endl;
      SgExprListExp * parameter_list = buildExprListExp (buildVarRefExp(const_cast<SgVariableSymbol*>(current_symbol)), buildVarRefExp("_num_blocks_",target_directive_stmt->get_scope()), buildIntVal(per_block_reduction_map[const_cast<SgVariableSymbol*>(current_symbol)]) );
      SgFunctionCallExp* func_call_exp = buildFunctionCallExp ("xomp_beyond_block_reduction_"+ orig_type->unparseToString(), buildVoidType(), parameter_list, target_directive_stmt->get_scope()); 
     //insertStatementBefore (target_directive_stmt, buildExprStatement(func_call_exp));
      SgStatement* assign_stmt = buildAssignStatement (buildVarRefExp(orig_var_name, omp_target_stmt_body_block )  ,func_call_exp);
     ROSE_ASSERT (target->get_scope () == target_directive_stmt->get_body()); // there is a block in between 
     ROSE_ASSERT (omp_target_stmt_body_block  == target_directive_stmt->get_body()); // just to make sure
     //insertStatementBefore (target_directive_stmt, buildExprStatement(func_call_exp2));
     insertStatementBefore (target, assign_stmt );

     // insert memory free for the _dev_per_block_variables
     // TODO: need runtime support to automatically free memory 
      SgFunctionCallExp* func_call_exp2 = buildFunctionCallExp ("xomp_freeDevice", buildVoidType(), buildExprListExp(buildVarRefExp(const_cast<SgVariableSymbol*>(current_symbol))),  omp_target_stmt_body_block);
     //insertStatementBefore (target_directive_stmt, buildExprStatement(func_call_exp2));
     insertStatementBefore (target, buildExprStatement(func_call_exp2));
    }

    // num_blocks is referenced before the declaration is inserted. So we must fix it, otherwise the symbol of unkown type will be cleaned up later.
    SageInterface::fixVariableReferences(num_blocks_decl->get_scope());
    //------------now remove omp parallel since everything within it has been outlined to a function
    removeStatement (target);
  }


  /*
   * Expected AST layout: 
   *  SgOmpSectionsStatement
   *    SgBasicBlock
   *      SgOmpSectionStatement (1 or more section statements here)
   *        SgBasicBlock
   *          SgStatement 
   *
   * Example translated code: 
      int _section_1 = XOMP_sections_init_next (3);
      while (_section_1 >=0) // This while loop is a must
      {
        switch (_section_1) {
          case 0:
            printf("hello from section 1\n");
            break;
          case 1:
            printf("hello from section 2\n");
            break;
          case 2:
            printf("hello from section 3\n");
            break;
          default:
            printf("fatal error: XOMP_sections_?_next() returns illegal value %d\n", _section_1);
            abort();
        }
        _section_1 = XOMP_sections_next ();  // next round for the current thread: deal with possible number of threads < number of sections
     }
    
      XOMP_sections_end();   // Or  XOMP_sections_end_nowait ();    
   * */
  void transOmpSections(SgNode* node)
  {
//    cout<<"Entering transOmpSections() ..."<<endl;
    ROSE_ASSERT(node != NULL );
    // verify the AST is expected
    SgOmpSectionsStatement * target = isSgOmpSectionsStatement(node); 
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );
    SgStatement * body = target->get_body();
    ROSE_ASSERT(body != NULL);

    SgBasicBlock *  bb1 = buildBasicBlock();
   
    SgBasicBlock * sections_block = isSgBasicBlock(body);
    ROSE_ASSERT (sections_block != NULL);
       // verify each statement under sections is SgOmpSectionStatement
    SgStatementPtrList section_list = sections_block-> get_statements();
    int section_count = section_list.size();
    for  (int i =0; i<section_count; i++)
    {
      SgStatement* stmt = section_list[i];
      ROSE_ASSERT (isSgOmpSectionStatement(stmt));
    }
   
    // int _section_1 = XOMP_sections_init_next (3);
    std::string sec_var_name;
    if (SageInterface::is_Fortran_language() )
      sec_var_name ="_section_";
    else  
      sec_var_name ="xomp_section_";

    sec_var_name += StringUtility::numberToString(++gensym_counter);
    
    SgAssignInitializer* initializer = buildAssignInitializer (
                         buildFunctionCallExp("XOMP_sections_init_next", buildIntType(),buildExprListExp(buildIntVal(section_count)), scope), 
                                        buildIntType());
    replaceStatement(target, bb1, true);
    //Declare a variable to store the current section id
    //Only used to support lastprivate
    SgVariableDeclaration* sec_var_decl_save = NULL;
    if (hasClause(target, V_SgOmpLastprivateClause))
    {
      sec_var_decl_save = buildVariableDeclaration(sec_var_name+"_save", buildIntType(), NULL, bb1);
      appendStatement(sec_var_decl_save, bb1);
    }

    SgVariableDeclaration* sec_var_decl = buildVariableDeclaration(sec_var_name, buildIntType(), initializer, bb1);
    appendStatement(sec_var_decl, bb1);

    // while (_section_1 >=0) {}
    SgWhileStmt * while_stmt = buildWhileStmt(buildGreaterOrEqualOp(buildVarRefExp(sec_var_decl), buildIntVal(0)), buildBasicBlock()); 
    insertStatementAfter(sec_var_decl, while_stmt);
    // switch () {}
    SgSwitchStatement* switch_stmt = buildSwitchStatement (buildExprStatement(buildVarRefExp(sec_var_decl)) , buildBasicBlock()); 
    appendStatement(switch_stmt, isSgBasicBlock(while_stmt->get_body()));
    // case 0, case 1, ...
    for (int i= 0; i<section_count; i++)
    {
      SgCaseOptionStmt* option_stmt = buildCaseOptionStmt (buildIntVal(i), buildBasicBlock());
      // Move SgOmpSectionStatement's body to Case OptionStmt's body
      SgOmpSectionStatement* section_statement = isSgOmpSectionStatement(section_list[i]);
      // Sara Royuela (Nov 19th, 2012)
      // The section statement might not be a Basic Block if there is only one statement and it is not wrapped with braces
      // In that case, we build here the Basic Block
      SgBasicBlock * src_bb = isSgBasicBlock(section_statement->get_body());
      if( src_bb == NULL )
      {
          src_bb = ensureBasicBlockAsBodyOfOmpBodyStmt( section_statement );
      }
      SgBasicBlock * target_bb =  isSgBasicBlock(option_stmt->get_body());
      moveStatementsBetweenBlocks(src_bb , target_bb);
      appendStatement (buildBreakStmt(), target_bb);

      // cout<<"source BB address:"<<isSgBasicBlock(isSgOmpSectionStatement(section_list[i])->get_body())<<endl;
      // Now we have to delete the source BB since its symbol table is moved into the target BB.
      SgBasicBlock * fake_src_bb = buildBasicBlock(); 
      isSgOmpSectionStatement(section_list[i])->set_body(fake_src_bb); 
      fake_src_bb->set_parent(section_list[i]);
      delete (src_bb);

      appendStatement (option_stmt,  isSgBasicBlock(switch_stmt->get_body()));
    } // end case 0, 1, ...  
    // default option: 
    SgDefaultOptionStmt* default_stmt = buildDefaultOptionStmt(buildBasicBlock(buildFunctionCallStmt("abort", buildVoidType(), NULL, scope))); 
    appendStatement (default_stmt,  isSgBasicBlock(switch_stmt->get_body()));

    // save the current section id before checking for next available one
    // This is only useful to support lastprivate clause
    if (hasClause(target, V_SgOmpLastprivateClause))
    {
      SgStatement* save_stmt = buildAssignStatement (buildVarRefExp(sec_var_decl_save), buildVarRefExp(sec_var_decl)); 
      appendStatement(save_stmt , isSgBasicBlock(while_stmt->get_body()));
    }
    // _section_1 = XOMP_sections_next ();
    SgStatement* assign_stmt = buildAssignStatement(buildVarRefExp(sec_var_decl), 
                                 buildFunctionCallExp("XOMP_sections_next", buildIntType(), buildExprListExp(), scope) ); 
    appendStatement(assign_stmt, isSgBasicBlock(while_stmt->get_body()));

    transOmpVariables(target, bb1, buildIntVal(section_count - 1)); // This should happen before the barrier is inserted.

    // XOMP_sections_end() or XOMP_sections_end_nowait ();
    SgExprStatement* end_call = NULL; 
    if (hasClause(target, V_SgOmpNowaitClause))
      end_call = buildFunctionCallStmt("XOMP_sections_end_nowait", buildVoidType(), NULL, scope);
    else
      end_call = buildFunctionCallStmt("XOMP_sections_end", buildVoidType(), NULL, scope);

    appendStatement(end_call,bb1);
//    removeStatement(target);
  }

  // Two ways 
  //1. builtin function TODO
  //    __sync_fetch_and_add_4(&shared, (unsigned int)local);
  //2. using atomic runtime call: 
  //    GOMP_atomic_start (); // void GOMP_atomic_start (void); 
  //    shared = shared op local;
  //    GOMP_atomic_end (); // void GOMP_atomic_end (void); 
  // We use the 2nd method only for now, for simplicity and portability
  void transOmpAtomic(SgNode* node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpAtomicStatement* target = isSgOmpAtomicStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );
    SgStatement * body = target->get_body();
    ROSE_ASSERT(body != NULL);
    
    replaceStatement(target, body, true);
#ifdef ENABLE_XOMP
    SgExprStatement* func_call_stmt1 = buildFunctionCallStmt("XOMP_atomic_start", buildVoidType(), NULL, scope);
    SgExprStatement* func_call_stmt2 = buildFunctionCallStmt("XOMP_atomic_end", buildVoidType(), NULL, scope);
#else
    SgExprStatement* func_call_stmt1 = buildFunctionCallStmt("GOMP_atomic_start", buildVoidType(), NULL, scope);
    SgExprStatement* func_call_stmt2 = buildFunctionCallStmt("GOMP_atomic_end", buildVoidType(), NULL, scope);
#endif
    insertStatementBefore(body, func_call_stmt1);
    // this is actually sensitive to the type of preprocessing Info
    // In most cases, we want to move up them (such as #ifdef etc)
    moveUpPreprocessingInfo (func_call_stmt1, body, PreprocessingInfo::before); 
    insertStatementAfter(body, func_call_stmt2);
  }


  //! Translate omp task
  /*
  The translation of omp task is similar to the one for omp parallel
  Please remember to call patchUpFirstprivateVariables() before this function to make implicit firstprivate
  variables explicit. 
  
  The gomp runtime function for omp task is:
  extern void GOMP_task (void (*fn) (void *), void *data, void (*cpyfn) (void *, void *), long arg_size, long arg_align, bool if_clause, unsigned flags) 
      1. void (*fn) (void *): the generated outlined function for the task body
      2. void *data: the parameters for the outlined function
      3. void (*cpyfn) (void *, void *): copy function to replace the default memcpy() from function data to each task's data
      4. long arg_size: specify the size of data
      5. long arg_align: alignment of the data
      6. bool if_clause: the value of if_clause. true --> 1, false -->0; default is set to 1 by GCC
      7. unsigned flags: untied (1) or not (0) 

   Since we use the ROSE outliner to generate the outlined function. The parameters are wrapped into an array of pointers to them
  So the calculation of data(parameter) size/align is simplified . They are all pointer types.
  */
  void transOmpTask(SgNode* node)
  {
    ROSE_ASSERT(node != NULL);
    SgOmpTaskStatement* target = isSgOmpTaskStatement(node);
    ROSE_ASSERT (target != NULL);

    // Liao 1/24/2011
    // For Fortran code, we have to insert EXTERNAL OUTLINED_FUNC into 
    // the function body containing the parallel region
    // TODO verify this is also necessary for OMP TASK
    SgFunctionDefinition * func_def = NULL;
    if (SageInterface::is_Fortran_language() )
    {
      func_def = getEnclosingFunctionDefinition(target);
      ROSE_ASSERT (func_def != NULL);
    }

    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
    // Save preprocessing info as early as possible, avoiding mess up from the outliner
    AttachedPreprocessingInfoType save_buf1, save_buf2;
    cutPreprocessingInfo(target, PreprocessingInfo::before, save_buf1) ;
    cutPreprocessingInfo(target, PreprocessingInfo::after, save_buf2) ;

    // generate and insert an outlined function as a task
    std::string wrapper_name;
    ASTtools::VarSymSet_t syms;
    ASTtools::VarSymSet_t pdSyms3; // store all variables which should be passed by reference
    std::set<SgInitializedName*> readOnlyVars;
    SgFunctionDeclaration* outlined_func = generateOutlinedTask (node, wrapper_name, syms, pdSyms3);

    if (SageInterface::is_Fortran_language() )
    { // EXTERNAL outlined_function , otherwise the function name will be interpreted as a integer/real variable
      ROSE_ASSERT (func_def != NULL);
      // There could be an enclosing parallel region
      //SgBasicBlock * func_body = func_def->get_body();
      SgBasicBlock * enclosing_body = getEnclosingRegionOrFuncDefinition (target);
      ROSE_ASSERT (enclosing_body != NULL);
      SgAttributeSpecificationStatement* external_stmt1 = buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_externalStatement)
;
      SgFunctionRefExp *func_ref1 = buildFunctionRefExp (outlined_func);
      external_stmt1->get_parameter_list()->prepend_expression(func_ref1);
      func_ref1->set_parent(external_stmt1->get_parameter_list());
      // must put it into the declaration statement part, after possible implicit/include statements, if any
      SgStatement* l_stmt = findLastDeclarationStatement (enclosing_body);
      if (l_stmt)
        insertStatementAfter(l_stmt,external_stmt1);
      else
        prependStatement(external_stmt1, enclosing_body);
    }


    SgScopeStatement * p_scope = target->get_scope();
    ROSE_ASSERT(p_scope != NULL);
    // Generate a call to it
   
    SgExprListExp* parameters =  NULL;
    //SgStatement* func_call = Outliner::generateCall (outlined_func, syms, readOnlyVars, wrapper_name,p_scope);
    //ROSE_ASSERT(func_call != NULL);

    // Replace the parallel region with the function call statement
    // TODO should we introduce another level of scope here?
    // SageInterface::replaceStatement(target,func_call, true);
    // hide this from the unparser TODO this call statement is not really necessary, only the call expression is needed
    //  Sg_File_Info* file_info = type_decl->get_file_info();
    //      file_info->unsetOutputInCodeGeneration ();
    //
    //func_call->get_file_info()->unsetOutputInCodeGeneration (); 
    SgExpression * parameter_data = NULL;
    SgExpression * parameter_cpyfn = NULL;
    SgExpression * parameter_arg_size = NULL;
    SgExpression * parameter_arg_align = NULL;
    SgExpression * parameter_if_clause =  NULL;
    SgExpression * parameter_untied = NULL;
    SgExpression * parameter_argcount = NULL;
    size_t parameter_count = syms.size();
   
    if (SageInterface::is_Fortran_language())
    { // Fortran case
    //  void xomp_task (void (*fn) (void *), void (*cpyfn) (void *, void *), int * arg_size, int * arg_align, 
    //                  int * if_clause, int * untied, int * argcount, ...)
      
        parameter_cpyfn=buildIntVal(0); // no special copy function for array of pointers
        parameter_arg_size = buildIntVal( parameter_count * sizeof(void*) );
        //  TODO get right alignment
        parameter_arg_align = buildIntVal(4);
    }
    else // C/C++ case
    //  void GOMP_task (void (*fn) (void *), void *data, void (*cpyfn) (void *, void *), long arg_size, long arg_align, 
    //                  bool if_clause, unsigned flags)
    { 
      if ( parameter_count == 0) // No parameters to be passed at all
      {
        parameter_data = buildIntVal(0);
        parameter_cpyfn=buildIntVal(0); // no copy function is needed
        parameter_arg_size = buildIntVal(0);
        parameter_arg_align = buildIntVal(0);
      }
      else
      {
        SgVarRefExp * data_ref = buildVarRefExp(wrapper_name, p_scope);
        ROSE_ASSERT (data_ref != NULL);
        SgType * data_type = data_ref->get_type();
        parameter_data =  buildAddressOfOp(data_ref);
        parameter_cpyfn=buildIntVal(0); // no special copy function for array of pointers
        // arg size of array of pointers = pointer_count * pointer_size
        // ROSE does not support cross compilation so sizeof(void*) can use as a workaround for now
        //we now use a structure containing pointers or non-pointer typed members to wrap parameters
        parameter_arg_size =  buildSizeOfOp(data_type);
        //  parameter_arg_size = buildIntVal( parameter_count* sizeof(void*));
        //  TODO get right alignment
        parameter_arg_align = buildIntVal(4);
        //parameter_arg_align = buildIntVal(sizeof(void*));
      }

    }

    if (hasClause(target, V_SgOmpIfClause))
    {
      Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpIfClause);
      ROSE_ASSERT (clauses.size() ==1); // should only have one if ()
      SgOmpIfClause * if_clause = isSgOmpIfClause (clauses[0]);
      ROSE_ASSERT (if_clause->get_expression() != NULL);
      parameter_if_clause = copyExpression(if_clause->get_expression());
    }
    else
      parameter_if_clause = buildIntVal(1);

    if (hasClause(target, V_SgOmpUntiedClause))
      parameter_untied = buildIntVal(1);
    else  
      parameter_untied = buildIntVal(0);

    
   // parameters are different between Fortran and C/C++
   // To support pass-by-value and pass-by-reference in the XOMP runtime
   // We use a triplet for each parameter to be passed to XOMP
   // <pass_by_value-ref, value-size, parameter-address>
   // e.g. if a integer i is intended to be passed by value in the task
   //   we generate three argument for it: 1, sizeof(int), i
    // similarly, for an array item[10], passed by reference in the task
    //   we generate: 0, sizeof(void*), item
    //   As a result, the variable length argument list is 3 times the count of original parameters long
    if (SageInterface::is_Fortran_language())
    {
      parameters = buildExprListExp(buildFunctionRefExp(outlined_func),
        parameter_cpyfn, parameter_arg_size, parameter_arg_align, parameter_if_clause, parameter_untied);

      parameter_argcount =  buildIntVal (syms.size()*3);
      appendExpression (parameters,parameter_argcount);
      ASTtools::VarSymSet_t::iterator iter = syms.begin();
      for (; iter!=syms.end(); iter++)
      {
        const SgVariableSymbol * sb = *iter;
        bool b_pass_value = true;
        // Assumption: 
        //   transOmpVariables() should already handled most private, reduction variables
        //    Anything left should be passed by reference by default , except for loop index variables. 
        // We check if a variable is a loop index, and pass it by value. 
        //   
        // TODO more accurate way to decide on pass-by-value or pass-by-reference in patchUpPrivateVariables()
        //    and patchUpFirstprivateVariables()
        if (isLoopIndexVariable (sb->get_declaration(), target))
        {
          b_pass_value = true;
          appendExpression (parameters,buildIntVal(1));
        }
        else
        { // all other should be treated as shared variables ( pass-by-reference )
          b_pass_value = false;
          appendExpression (parameters,buildIntVal(0));
        }

        //2nd of the triplet, the size of the parameter type, 
        // if pass-by-value, the actual size
        // if pass-by-reference, the pointer size
        if (b_pass_value)
        { //TODO accurate calculation of type size for Fortran, assume integer for now
           // Provide an interface function for this.
          // Is it feasible to calculate all sizes during compilation time ??
          SgType * s_type = sb->get_type();
          if (isSgTypeInt(s_type))
            appendExpression (parameters,buildIntVal(sizeof(int)));
          else
          {
            printf("Error. transOmpTask(): unhandled Fortran type  (%s) for pass-by-value.\n",s_type->class_name().c_str());
            ROSE_ASSERT (false);
          }
        }
        else  
        { // get target platform's pointer size 
          appendExpression (parameters,buildIntVal(sizeof(void*)));
        }
        
        // the third of the triplet
        appendExpression (parameters, buildVarRefExp(const_cast<SgVariableSymbol *>(sb)));
      }
    }  
    else
    {
      parameters = buildExprListExp(buildFunctionRefExp(outlined_func),
        parameter_data, parameter_cpyfn, parameter_arg_size, parameter_arg_align, parameter_if_clause, parameter_untied);
    }

#ifdef ENABLE_XOMP
    SgExprStatement * s1 = buildFunctionCallStmt("XOMP_task", buildVoidType(), parameters, p_scope);
#else    
    SgExprStatement * s1 = buildFunctionCallStmt("GOMP_task", buildVoidType(), parameters, p_scope);
#endif
    SageInterface::replaceStatement(target,s1, true);

    // Keep preprocessing information
    // I have to use cut-paste instead of direct move since 
    // the preprocessing information may be moved to a wrong place during outlining
    // while the destination node is unknown until the outlining is done.
    pastePreprocessingInfo(s1, PreprocessingInfo::before, save_buf1);
    pastePreprocessingInfo(s1, PreprocessingInfo::after, save_buf2);
  }

  //! Translate the ordered directive, (not the ordered clause)
  void transOmpOrdered(SgNode* node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpOrderedStatement* target = isSgOmpOrderedStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );
    SgStatement * body = target->get_body();
    ROSE_ASSERT(body != NULL);

    replaceStatement(target, body,true);
#ifdef ENABLE_XOMP
    SgExprStatement* func_call_stmt1 = buildFunctionCallStmt("XOMP_ordered_start", buildVoidType(), NULL, scope);
    SgExprStatement* func_call_stmt2 = buildFunctionCallStmt("XOMP_ordered_end", buildVoidType(), NULL, scope);
#else
    SgExprStatement* func_call_stmt1 = buildFunctionCallStmt("GOMP_ordered_start", buildVoidType(), NULL, scope);
    SgExprStatement* func_call_stmt2 = buildFunctionCallStmt("GOMP_ordered_end", buildVoidType(), NULL, scope);
#endif
    insertStatementBefore(body, func_call_stmt1);
    insertStatementAfter(body, func_call_stmt2);
  }

  // Two cases:
  // unnamed one
  //   GOMP_critical_start ();
  //   work()
  //   GOMP_critical_end ();
  //
  // named one: 
  //  static gomp_mutex_t  &gomp_critical_user_aaa;
  //  GOMP_critical_name_start (&gomp_critical_user_aaa);
  //  work()
  //  GOMP_critical_name_end (&gomp_critical_user_aaa);
  //
  void transOmpCritical(SgNode* node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpCriticalStatement* target = isSgOmpCriticalStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );

    SgStatement * body = target->get_body();
    ROSE_ASSERT(body != NULL);

    replaceStatement(target, body,true);

    SgExprStatement* func_call_stmt1=NULL, * func_call_stmt2 =NULL;
    string c_name = target->get_name().getString();
#ifdef ENABLE_XOMP    
    // assign a default name for the unnamed critical to simplify the translation
    // GOMP actually have a dedicated function to support unnamed critical
    // We generate a default name for it and use the named critical support function instead to
    // be consistent with OMNI
      string g_lock_name = "xomp_critical_user_" + c_name;
      SgGlobal* global = getGlobalScope(target);
      ROSE_ASSERT(global!=NULL);
      // the lock variable may already be declared.
      SgVariableSymbol* sym = lookupVariableSymbolInParentScopes(SgName(g_lock_name),global); 
      if (sym == NULL)
      {
        SgVariableDeclaration* vardecl = buildVariableDeclaration(g_lock_name, buildPointerType(buildVoidType()), NULL, global);
        setStatic(vardecl);
        prependStatement(vardecl,global);
        sym = getFirstVarSym(vardecl);
      }

      SgExprListExp * param1= buildExprListExp(buildAddressOfOp(buildVarRefExp(sym)));
      SgExprListExp * param2= buildExprListExp(buildAddressOfOp(buildVarRefExp(sym)));

      func_call_stmt1 = buildFunctionCallStmt("XOMP_critical_start", buildVoidType(), param1, scope);
      func_call_stmt2 = buildFunctionCallStmt("XOMP_critical_end", buildVoidType(), param2, scope);
#else    
    if (c_name.length()==0)
    {
      func_call_stmt1 = buildFunctionCallStmt("GOMP_critical_start", buildVoidType(), NULL, scope);
      func_call_stmt2 = buildFunctionCallStmt("GOMP_critical_end", buildVoidType(), NULL, scope);
    }
    else
    {
      string g_lock_name = "gomp_critical_user_" + c_name;
      SgGlobal* global = getGlobalScope(target);
      ROSE_ASSERT(global!=NULL);
      // gomp_mutex_t is not declared by the RTL header. We use int instead.
      SgVariableDeclaration* vardecl = buildVariableDeclaration(g_lock_name, buildPointerType(buildVoidType()), NULL, global);
      //SgVariableDeclaration* vardecl = buildVariableDeclaration(g_lock_name, buildOpaqueType("gomp_mutex_t",global), NULL, global);
      setStatic(vardecl);
      prependStatement(vardecl,global);
      SgExprListExp * param1= buildExprListExp(buildAddressOfOp(buildVarRefExp(vardecl)));
      SgExprListExp * param2= buildExprListExp(buildAddressOfOp(buildVarRefExp(vardecl)));
      func_call_stmt1 = buildFunctionCallStmt("GOMP_critical_name_start", buildVoidType(), param1, scope);
      func_call_stmt2 = buildFunctionCallStmt("GOMP_critical_name_end", buildVoidType(), param2, scope);
    }
#endif    

    insertStatementBefore(body, func_call_stmt1);
    insertStatementAfter(body, func_call_stmt2);
  }

  //! Simply replace the pragma with a function call to void GOMP_taskwait(void); 
  void transOmpTaskwait(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpTaskwaitStatement* target = isSgOmpTaskwaitStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );
#ifdef ENABLE_XOMP
    SgExprStatement* func_call_stmt = buildFunctionCallStmt("XOMP_taskwait", buildVoidType(), NULL, scope);
#else    
    SgExprStatement* func_call_stmt = buildFunctionCallStmt("GOMP_taskwait", buildVoidType(), NULL, scope);
#endif
    replaceStatement(target, func_call_stmt, true);
  }

  //! Simply replace the pragma with a function call to void GOMP_barrier (void); 
  void transOmpBarrier(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpBarrierStatement* target = isSgOmpBarrierStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );

#ifdef ENABLE_XOMP // test new translation targeting a middle layer of runtime library
    SgExprStatement* func_call_stmt = buildFunctionCallStmt("XOMP_barrier", buildVoidType(), NULL, scope);
#else
    SgExprStatement* func_call_stmt = buildFunctionCallStmt("GOMP_barrier", buildVoidType(), NULL, scope);
#endif    
    replaceStatement(target, func_call_stmt, true);
  }

  //! Simply replace the pragma with a function call to __sync_synchronize ();
  void transOmpFlush(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpFlushStatement* target = isSgOmpFlushStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );

#ifdef ENABLE_XOMP
    SgExprStatement* func_call_stmt = buildFunctionCallStmt("XOMP_flush_all", buildVoidType(), NULL, scope);
#else
    SgExprStatement* func_call_stmt = buildFunctionCallStmt("__sync_synchronize", buildVoidType(), NULL, scope);
#endif
    replaceStatement(target, func_call_stmt, true);
  }

  //! Simply move the body up and remove omp target directive since nothing to be done at this level
  // We essentially only support combined omp target parallel .... , even the code uses separated styles
  // outlining and data handling are handled by transOmpTargetParallel()
  void transOmpTarget(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpTargetStatement* target = isSgOmpTargetStatement(node);
    ROSE_ASSERT(target != NULL );

    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );

    SgBasicBlock* body = isSgBasicBlock(target->get_body());
    ROSE_ASSERT(body != NULL );
    body->set_parent(NULL);
    target->set_body(NULL);

    replaceStatement (target, body, true); 
   // removeStatement(target);
  }

  //! Simply move the body up and remove omp target data directive since nothing to be done at this level for now
  //  all map() clauses should already be handled when translating the inner "omp parallel" region
  // TODO: translate if() and device() clauses
  void transOmpTargetData(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpTargetDataStatement* target = isSgOmpTargetDataStatement(node);
    ROSE_ASSERT(target != NULL );

    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );

    if (useDDE)
      transOmpMapVariables (target);

    SgBasicBlock* body = isSgBasicBlock(target->get_body());
    ROSE_ASSERT(body != NULL );
    body->set_parent(NULL);
    target->set_body(NULL);


    replaceStatement (target, body, true); 
    attachComment (body, "Translated from #pragma omp target data ...");
   // removeStatement(target);
  }


  //! Add __thread for each threadprivate variable's declaration statement and remove the #pragma omp threadprivate(...) 
  void transOmpThreadprivate(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpThreadprivateStatement* target = isSgOmpThreadprivateStatement(node);
    ROSE_ASSERT(target != NULL );

    SgVarRefExpPtrList nameList = target->get_variables ();
    for (size_t i = 0; i<nameList.size(); i++)
    {
      SgInitializedName* init_name = nameList[i]->get_symbol()->get_declaration();
      ROSE_ASSERT(init_name != NULL);
      SgVariableDeclaration*  decl = isSgVariableDeclaration(init_name-> get_declaration());
      ROSE_ASSERT (decl != NULL);
     // cout<<"setting TLS for decl:"<<decl->unparseToString()<< endl;
      decl->get_declarationModifier().get_storageModifier().set_thread_local_storage(true);
      // choice between set TLS to declaration or init_name (not working) ?
     // init_name-> get_storageModifier ().set_thread_local_storage (true); 
    }

    // 6/8/2010, handling #if attached to #pragma omp threadprivate
    SgStatement* n_stmt = getNextStatement(target);
    if (n_stmt == NULL) 
    {
      cerr<<"Warning: found an omp threadprivate directive without a following statement."<<endl;
      cerr<<"Warning: the attached preprocessing information to the directive may get lost during translation!"<<endl;
    }
    else
    {
      // preserve preprocessing information attached to the pragma,
      // by moving it to the beginning of the preprocessing info list of the next statement .
      movePreprocessingInfo(target, n_stmt, PreprocessingInfo::before, PreprocessingInfo::before, true);
    }

    removeStatement(target);
  }


  //! Collect variables from OpenMP clauses: including private, firstprivate, lastprivate, reduction, etc.
  SgInitializedNamePtrList collectClauseVariables (SgOmpClauseBodyStatement * clause_stmt, const VariantT & vt)
  {
    return collectClauseVariables(clause_stmt, VariantVector(vt));
  }

  // Collect variables from an OpenMP clause: including private, firstprivate, lastprivate, reduction, etc.
  SgInitializedNamePtrList collectClauseVariables (SgOmpClauseBodyStatement * clause_stmt, const VariantVector & vvt)
  {
    SgInitializedNamePtrList result, result2;
    ROSE_ASSERT(clause_stmt != NULL);
    Rose_STL_Container<SgOmpClause*> p_clause =
      NodeQuery::queryNodeList<SgOmpClause>(clause_stmt->get_clauses(),vvt);
    for (size_t i =0; i< p_clause.size(); i++) // can have multiple reduction clauses of different reduction operations
    {  
      //result2 = isSgOmpVariablesClause(p_clause[i])->get_variables();  
      // get initialized name from varRefExp
      SgVarRefExpPtrList refs = isSgOmpVariablesClause(p_clause[i])->get_variables();
      result2.clear();
      for (size_t j =0; j< refs.size(); j++)
         result2.push_back(refs[j]->get_symbol()->get_declaration()); 
      std::copy(result2.begin(), result2.end(), back_inserter(result));
    }
    return result;
  }

  SgExpression* getClauseExpression(SgOmpClauseBodyStatement * clause_stmt, const VariantVector & vvt)
  {
     SgExpression* expr = NULL;
     ROSE_ASSERT(clause_stmt != NULL);
     Rose_STL_Container<SgOmpClause*> p_clause = 
       NodeQuery::queryNodeList<SgOmpClause>(clause_stmt->get_clauses(),vvt);
     //It is possible that the requested clauses are not found. We allow returning NULL expression.  
     //Liao, 6/16/2015
     if (p_clause.size()>=1)
       expr = isSgOmpExpressionClause(p_clause[0])->get_expression();
     return expr; 
  }

  //! Collect all variables from OpenMP clauses associated with an omp statement: private, reduction, etc 
  SgInitializedNamePtrList collectAllClauseVariables (SgOmpClauseBodyStatement * clause_stmt)
  {
    ROSE_ASSERT(clause_stmt != NULL);

    VariantVector vvt = VariantVector(V_SgOmpCopyinClause);
    vvt.push_back(V_SgOmpCopyprivateClause);
    vvt.push_back(V_SgOmpFirstprivateClause);
    vvt.push_back(V_SgOmpLastprivateClause);
    vvt.push_back(V_SgOmpPrivateClause);
    vvt.push_back(V_SgOmpReductionClause);
    // TODO : do we care about shared(var_list)?

    return collectClauseVariables(clause_stmt, vvt);
  }

  bool isInClauseVariableList(SgInitializedName* var, SgOmpClauseBodyStatement * clause_stmt, const VariantVector& vvt)
  {
    SgInitializedNamePtrList var_list = collectClauseVariables (clause_stmt, vvt);
    if (find(var_list.begin(), var_list.end(), var) != var_list.end() )
      return true;
    else
      return false;
  }

   //! Return a reduction variable's reduction operation type
   SgOmpClause::omp_reduction_operator_enum getReductionOperationType(SgInitializedName* init_name, SgOmpClauseBodyStatement* clause_stmt)
   {
     SgOmpClause::omp_reduction_operator_enum result = SgOmpClause::e_omp_reduction_unknown;
     bool found = false;
     ROSE_ASSERT(init_name != NULL);
     ROSE_ASSERT(clause_stmt!= NULL);
     Rose_STL_Container<SgOmpClause*> p_clause =
       NodeQuery::queryNodeList<SgOmpClause>(clause_stmt->get_clauses(),V_SgOmpReductionClause);
     ROSE_ASSERT(p_clause.size() >0); // must be have at least reduction clause

     for (size_t i =0; i< p_clause.size(); i++) // can have multiple reduction clauses of different reduction operations
     {
       SgOmpReductionClause* r_clause = isSgOmpReductionClause(p_clause[i]);
       ROSE_ASSERT(r_clause != NULL );
       SgVarRefExpPtrList refs = isSgOmpVariablesClause(r_clause)->get_variables();
       SgInitializedNamePtrList var_list ; //= isSgOmpVariablesClause(r_clause)->get_variables();
       for (size_t j=0; j< refs.size(); j++)
         var_list.push_back (refs[j]->get_symbol()->get_declaration());
       SgInitializedNamePtrList::const_iterator iter = find (var_list.begin(), var_list.end(), init_name);
       if (iter != var_list.end())
       {
         result = r_clause->get_operation();
         found = true;
         break;
       }
     }
     // Must have a hit
     ROSE_ASSERT(found == true);
     return result;
   }  

   //! Create an initial value according to reduction operator type
   SgExpression* createInitialValueExp(SgOmpClause::omp_reduction_operator_enum r_operator)
   {
     SgExpression * result = NULL;
     switch (r_operator )
     {
       // 0: + - ! ^ ||  ior ieor
       case SgOmpClause::e_omp_reduction_plus:
       case SgOmpClause::e_omp_reduction_minus:
       case SgOmpClause::e_omp_reduction_bitor:
       case SgOmpClause::e_omp_reduction_bitxor:
       case SgOmpClause::e_omp_reduction_or:
       case SgOmpClause::e_omp_reduction_ior:
       case SgOmpClause::e_omp_reduction_ieor:
          result = buildIntVal(0);
          break;
       // 1: * &&
       case SgOmpClause::e_omp_reduction_mul:
       case SgOmpClause::e_omp_reduction_bitand:
          result = buildIntVal(1);
          break;
        // TODO
       case SgOmpClause::e_omp_reduction_logand:
       case SgOmpClause::e_omp_reduction_logor:
       case SgOmpClause::e_omp_reduction_and:
       case SgOmpClause::e_omp_reduction_eqv:
       case SgOmpClause::e_omp_reduction_neqv:
       case SgOmpClause::e_omp_reduction_max:
       case SgOmpClause::e_omp_reduction_min:
       case SgOmpClause::e_omp_reduction_iand:
          
       case SgOmpClause::e_omp_reduction_unknown:
       case SgOmpClause::e_omp_reduction_last:
       default:
         cerr<<"Illegal or unhandled reduction operator kind: "<< r_operator <<endl;
         ROSE_ASSERT(false);
     }

     return result; 
   }

  //! Check if a variable is in a variable list of a given clause type
  bool isInClauseVariableList(SgInitializedName* var, SgOmpClauseBodyStatement * clause_stmt, const VariantT& vt)
  {
    return isInClauseVariableList(var, clause_stmt, VariantVector(vt));
  }

 // lastprivate can be used with loop constructs or sections.
   /* if (i is the last iteration)
   *   *shared_i_p = local_i
   *
   * The judge of last iteration is based on the iteration space increment direction and loop stop conditions
   * Incremental loops
   *      < upper:   last iteration ==> i >= upper
   *      <=     :                      i> upper
   * Decremental loops     
   *      > upper:   last iteration ==> i <= upper
   *      >=     :                      i < upper
   * AST: Orphaned worksharing OmpStatement is SgOmpForStatement->get_body() is SgForStatement
   *     
   *  We use bottom up traversal, the inner omp for loop has already been translated, so we have to get the original upper bound via parameter
   *
   *  Another tricky case is that when some threads don't get any iterations to work on, the initial _p_index may still trigger the lastprivate 's 
   *     if (_p_index>orig_bound) statement
   *  We add a condition to test if the thread really worked on at least on iteration before compare the _p_index and the original boundary
   *     if (_p_index != p_lower_ && _p_index>orig_bound) 
   *       statement
   *
   *  Parameters:
   *    ompStmt: the OpenMP statement node with a lastprivate clause
   *    end_stmt_list: a list of statement which will be append to the end of bb1. The generated if-stmt will be added to the end of this list
   *    bb1: the basic block affected by the lastprivate clause
   *    orig_var: the initialized name for the original lastprivate variable. Necessary since transOmpLoop will replace loop index with changed one
   *    local_decl: the variable declaration for the local copy of the lastprivate variable
   *    orig_loop_upper: the worksharing construct's upper limit: 
   *       for-loop: the loop upper value, 
   *       sections: the section count - 1
   *
   * */
static void insertOmpLastprivateCopyBackStmts(SgStatement* ompStmt, vector <SgStatement* >& end_stmt_list,  SgBasicBlock* bb1, 
                              SgInitializedName* orig_var, SgVariableDeclaration* local_decl, SgExpression* orig_loop_upper)
{
  SgStatement* save_stmt = NULL;
  if (isSgOmpForStatement(ompStmt))
  {
    ROSE_ASSERT (orig_loop_upper != NULL);
    Rose_STL_Container <SgNode*> loops = NodeQuery::querySubTree (bb1, V_SgForStatement);
    ROSE_ASSERT (loops.size() != 0); // there must be 1 for loop under SgOmpForStatement
    SgForStatement* top_loop = isSgForStatement(loops[0]);
    ROSE_ASSERT (top_loop != NULL);
    //Get essential loop information
    SgInitializedName* loop_index;
    SgExpression* loop_lower, *loop_upper, *loop_step;
    SgStatement* loop_body;
    bool  isIncremental;
    bool isInclusiveBound;
    bool isCanonical = SageInterface::isCanonicalForLoop (top_loop, &loop_index, & loop_lower, & loop_upper, & loop_step, &loop_body, & isIncremental, & isInclusiveBound);
    ROSE_ASSERT (isCanonical == true);
    SgExpression* if_cond= NULL;
    SgStatement* if_cond_stmt = NULL;
    // we need the original upper bound!!
    if (isIncremental)
    {
      if (isInclusiveBound) // <= --> >
      {
        if_cond= buildGreaterThanOp(buildVarRefExp(loop_index, bb1), copyExpression(orig_loop_upper));
      }
      else // < --> >=
      {
        if_cond= buildGreaterOrEqualOp(buildVarRefExp(loop_index, bb1), copyExpression(orig_loop_upper));
      }
    }
    else
    { // decremental loop
      if (isInclusiveBound) // >= --> <
      {
        if_cond= buildLessThanOp(buildVarRefExp(loop_index, bb1), copyExpression(orig_loop_upper));
      }
      else // > --> <=
      {
        if_cond= buildLessOrEqualOp(buildVarRefExp(loop_index, bb1), copyExpression(orig_loop_upper));
      }
    }
    // Add (_p_index != _p_lower) as another condition, making sure the current thread really worked on at least one iteration
    // Otherwise some thread which does not run any iteration may have a big initial _p_index and trigger the if statement's condition
    if_cond_stmt = buildExprStatement(buildAndOp(buildNotEqualOp(buildVarRefExp(loop_index, bb1), copyExpression(loop_lower)), if_cond)) ;
    SgStatement* true_body = buildAssignStatement(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl));
    save_stmt = buildIfStmt(if_cond_stmt, true_body, NULL);
  }
  else if (isSgOmpSectionsStatement(ompStmt))
  {
    ROSE_ASSERT (orig_loop_upper != NULL);
    Rose_STL_Container <SgNode*> while_stmts = NodeQuery::querySubTree (bb1, V_SgWhileStmt);
    ROSE_ASSERT  (while_stmts.size()!=0);
    SgWhileStmt * top_while_stmt = isSgWhileStmt(while_stmts[0]);
    ROSE_ASSERT(top_while_stmt != NULL);
    //Get the section id variable from while-stmt  while(section_id >= 0) {}
    // SgWhileStmt -> SgExprStatement -> SgGreaterOrEqualOp-> SgVarRefExp
    SgExprStatement* exp_stmt = isSgExprStatement(top_while_stmt->get_condition());
    ROSE_ASSERT (exp_stmt != NULL);
    SgGreaterOrEqualOp* ge_op = isSgGreaterOrEqualOp(exp_stmt->get_expression());
    ROSE_ASSERT (ge_op != NULL);
    SgVarRefExp* var_ref = isSgVarRefExp(ge_op->get_lhs_operand()); 
    ROSE_ASSERT (var_ref != NULL);
    string switch_index_name = (var_ref->get_symbol()->get_name()).getString();
    SgExpression* if_cond= NULL;
    SgStatement* if_cond_stmt = NULL;
    if_cond= buildEqualityOp(buildVarRefExp((switch_index_name+"_save"), bb1), orig_loop_upper);// no need copy orig_loop_upper here
    if_cond_stmt = buildExprStatement(if_cond) ;
    SgStatement* true_body = buildAssignStatement(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl));
    save_stmt = buildIfStmt(if_cond_stmt, true_body, NULL);
  }
  else  
  {
    cerr<<"Illegal SgOmpxx for lastprivate variable: \nOmpStatement is:"<< ompStmt->class_name()<<endl;
    cerr<<"lastprivate variable is:"<<orig_var->get_name().getString()<<endl;
    ROSE_ASSERT (false);
  }
  end_stmt_list.push_back(save_stmt);

}

  //!Generate copy-back statements for reduction variables
  // end_stmt_list: the statement lists to be appended
  // bb1: the affected code block by the reduction clause
  // orig_var: the reduction variable's original copy
  // local_decl: the local copy of the reduction variable
  // Two ways to do the reduction operation: 
  //1. builtin function TODO
  //    __sync_fetch_and_add_4(&shared, (unsigned int)local);
  //2. using atomic runtime call: 
  //    GOMP_atomic_start ();
  //    shared = shared op local;
  //    GOMP_atomic_end ();
  // We use the 2nd method only for now for simplicity and portability
static void insertOmpReductionCopyBackStmts (SgOmpClause::omp_reduction_operator_enum r_operator, vector <SgStatement* >& end_stmt_list,  SgBasicBlock* bb1, SgInitializedName* orig_var, SgVariableDeclaration* local_decl)
{
#ifdef ENABLE_XOMP
  SgExprStatement* atomic_start_stmt = buildFunctionCallStmt("XOMP_atomic_start", buildVoidType(), NULL, bb1); 
#else  
  SgExprStatement* atomic_start_stmt = buildFunctionCallStmt("GOMP_atomic_start", buildVoidType(), NULL, bb1); 
#endif  
  end_stmt_list.push_back(atomic_start_stmt);   
  SgExpression* r_exp = NULL;
  switch (r_operator) 
  {
    case SgOmpClause::e_omp_reduction_plus:
      r_exp = buildAddOp(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl)); 
      break;
    case SgOmpClause::e_omp_reduction_mul:
      r_exp = buildMultiplyOp(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl)); 
      break;
    case SgOmpClause::e_omp_reduction_minus:
      r_exp = buildSubtractOp(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl)); 
      break;
    case SgOmpClause::e_omp_reduction_bitand:
      r_exp = buildBitAndOp(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl)); 
      break;
    case SgOmpClause::e_omp_reduction_bitor:
      r_exp = buildBitOrOp(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl)); 
      break;
    case SgOmpClause::e_omp_reduction_bitxor: 
      r_exp = buildBitXorOp(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl)); 
      break;
    case SgOmpClause::e_omp_reduction_logand:
      r_exp = buildAndOp(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl)); 
      break;
    case SgOmpClause::e_omp_reduction_logor:
      r_exp = buildOrOp(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl)); 
      break;
      // TODO Fortran operators.   
    case SgOmpClause::e_omp_reduction_and: // Fortran .and.
    case SgOmpClause::e_omp_reduction_or: // Fortran .or.
    case SgOmpClause::e_omp_reduction_eqv: 
    case SgOmpClause::e_omp_reduction_neqv:
    case SgOmpClause::e_omp_reduction_max:
    case SgOmpClause::e_omp_reduction_min:
    case SgOmpClause::e_omp_reduction_iand:
    case SgOmpClause::e_omp_reduction_ior:
    case SgOmpClause::e_omp_reduction_ieor:
    case SgOmpClause::e_omp_reduction_unknown: 
    case SgOmpClause::e_omp_reduction_last:
    default:
        cerr<<"Illegal or unhandled reduction operator type:"<< r_operator<<endl;
    }
    SgStatement* reduction_stmt = buildAssignStatement(buildVarRefExp(orig_var, bb1), r_exp);
    end_stmt_list.push_back(reduction_stmt);   
#ifdef ENABLE_XOMP
    SgExprStatement* atomic_end_stmt = buildFunctionCallStmt("XOMP_atomic_end", buildVoidType(), NULL, bb1);  
#else    
    SgExprStatement* atomic_end_stmt = buildFunctionCallStmt("GOMP_atomic_end", buildVoidType(), NULL, bb1);  
#endif    
    end_stmt_list.push_back(atomic_end_stmt);   
  }

//! Liao 2/12/2013. Insert the thread-block inner level reduction statement into the end of the end_stmt_list
// e.g.  xomp_inner_block_reduction_float (local_error, per_block_error, XOMP_REDUCTION_PLUS);
static void insertInnerThreadBlockReduction(SgOmpClause::omp_reduction_operator_enum r_operator, vector <SgStatement* >& end_stmt_list,  SgBasicBlock* bb1
, SgInitializedName* orig_var, SgVariableDeclaration* local_decl, SgVariableDeclaration* per_block_decl)
{
   ROSE_ASSERT (bb1 && orig_var && local_decl && per_block_decl);  
   // the integer value representing different reduction operations, defined within libxomp.h for accelerator model
   // TODO refactor the code to have a function converting operand types to integers
  int op_value = -1;
  switch (r_operator)
  {
    case SgOmpClause::e_omp_reduction_plus:
      op_value = 6;
      break;
    case SgOmpClause::e_omp_reduction_minus:
      op_value = 7;
      break;
    case SgOmpClause::e_omp_reduction_mul:
      op_value = 8;
      break;
    case SgOmpClause::e_omp_reduction_bitand:
      op_value = 9;
      break;
    case SgOmpClause::e_omp_reduction_bitor:
      op_value = 10;
      break;
    case SgOmpClause::e_omp_reduction_bitxor:
      op_value = 11;
      break;
    case SgOmpClause::e_omp_reduction_logand:
      op_value = 12;
      break;
    case SgOmpClause::e_omp_reduction_logor:
      op_value = 13;
      break;
      //TODO: more operation types
    case SgOmpClause::e_omp_reduction_and: // Fortran .and.
    case SgOmpClause::e_omp_reduction_or: // Fortran .or.
    case SgOmpClause::e_omp_reduction_eqv:
    case SgOmpClause::e_omp_reduction_neqv:
    case SgOmpClause::e_omp_reduction_max:
    case SgOmpClause::e_omp_reduction_min:
    case SgOmpClause::e_omp_reduction_iand:
    case SgOmpClause::e_omp_reduction_ior:
    case SgOmpClause::e_omp_reduction_ieor:
    case SgOmpClause::e_omp_reduction_unknown:
    case SgOmpClause::e_omp_reduction_last:
    default:
      cerr<<"Error. insertThreadBlockReduction() in omp_lowering.cpp: Illegal or unhandled reduction operator type:"<< r_operator<<endl;
  }

  SgVariableSymbol* var_sym = getFirstVarSym(per_block_decl);
  ROSE_ASSERT (var_sym != NULL);
  SgPointerType* var_type = isSgPointerType(var_sym->get_type());
  ROSE_ASSERT (var_type != NULL);
  //TODO: this could be risky. It is better to have our own conversion function to have full control over it.
  string type_str = var_type->get_base_type()->unparseToString();
  per_block_reduction_map[var_sym] = op_value; // save the per block symbol and its corresponding reduction integer value defined in the libxomp.h 
  SgIntVal* reduction_op = buildIntVal (op_value);
  SgExprListExp * parameter_list = buildExprListExp (buildVarRefExp(local_decl), buildVarRefExp(per_block_decl), reduction_op);
  SgStatement* func_call_stmt = buildFunctionCallStmt("xomp_inner_block_reduction_"+type_str, buildVoidType(),parameter_list ,bb1);
  end_stmt_list.push_back(func_call_stmt);
}
   //TODO move to sageInterface advanced transformation ???
   //! Generate element-by-element assignment from a right-hand array to left_hand array variable. 
   //
   //e.g.  for int a[M][N], b[M][N],  a=b is implemented as follows:
   //
   //  int element_count = ...;
   //  int *a_ap = (int *)a;
   //  int *b_ap = (int *)b;
   //  int i;
   //  for (i=0;i<element_count; i++) 
   //    *(b_ap+i) = *(a_ap+i);
   //
   static  SgBasicBlock* generateArrayAssignmentStatements
   (SgInitializedName* left_operand, SgInitializedName* right_operand, SgScopeStatement* scope)
   {
     // parameter validation
      ROSE_ASSERT(scope != NULL); // enforce top-down AST construction here for simplicity
      ROSE_ASSERT (left_operand != NULL);
      ROSE_ASSERT (right_operand != NULL);

      SgType* left_type = left_operand->get_type();
      SgType* right_type = right_operand->get_type();
      SgArrayType* left_array_type = isSgArrayType(left_type);
      SgArrayType* right_array_type = isSgArrayType(right_type);

      ROSE_ASSERT (left_array_type != NULL);
      ROSE_ASSERT (right_array_type != NULL);
      // make sure two array are compatible: same dimension, bounds, and element types, etc.
      ROSE_ASSERT (getElementType(left_array_type) == getElementType(right_array_type));
      int dim_count = getDimensionCount(left_array_type);
      ROSE_ASSERT (dim_count == getDimensionCount(right_array_type));
      int element_count = getArrayElementCount (left_array_type); 
      ROSE_ASSERT (element_count == (int) getArrayElementCount (right_array_type));

     SgBasicBlock* bb = buildBasicBlock();
       // front_stmt_list.push_back() will handle this later on.
       // Keep this will cause duplicated appendStatement()
      // appendStatement(bb, scope);
     
         // int *a_ap = (int*) a;
      string right_name = right_operand->get_name().getString(); 
      string right_name_p = right_name+"_ap"; // array pointer (ap)
      SgType* elementPointerType = buildPointerType(buildIntType());
      SgAssignInitializer * initor = buildAssignInitializer
             (buildCastExp(buildVarRefExp(right_operand,scope),elementPointerType),elementPointerType);
      SgVariableDeclaration* decl_right = buildVariableDeclaration (right_name_p, elementPointerType, initor, bb );
     appendStatement(decl_right, bb);
      
      // int *b_ap = (int*) b;
      string left_name = left_operand->get_name().getString(); 
      string left_name_p = left_name+"_ap";
      SgAssignInitializer * initor2 = buildAssignInitializer
             (buildCastExp(buildVarRefExp(left_operand,scope),elementPointerType),elementPointerType);
      SgVariableDeclaration* decl_left = buildVariableDeclaration (left_name_p, elementPointerType, initor2, bb );
     appendStatement(decl_left, bb);
   
     // int i;
     SgVariableDeclaration* decl_i = buildVariableDeclaration("_p_i", buildIntType(), NULL, bb);
     appendStatement(decl_i, bb);

   //  for (i=0;i<element_count; i++) 
   //    *(b_ap+i) = *(a_ap+i);
    SgStatement* init_stmt = buildAssignStatement(buildVarRefExp(decl_i), buildIntVal(0));
    SgStatement* test_stmt = buildExprStatement(buildLessThanOp(buildVarRefExp(decl_i),buildIntVal(element_count)));
    SgExpression* incr_exp = buildPlusPlusOp(buildVarRefExp(decl_i),SgUnaryOp::postfix);
    SgStatement* loop_body = buildAssignStatement(
         buildPointerDerefExp(buildAddOp(buildVarRefExp(decl_left),buildVarRefExp(decl_i))),
         buildPointerDerefExp(buildAddOp(buildVarRefExp(decl_right),buildVarRefExp(decl_i)))
          );
    SgForStatement* for_stmt = buildForStatement(init_stmt, test_stmt, incr_exp, loop_body);
     appendStatement(for_stmt, bb);

     return bb;
   }

  //SgBasicBlock * getEnclosingRegionOrFuncDefinition(SgBasicBlock *orig_scope)
  SgBasicBlock * getEnclosingRegionOrFuncDefinition(SgNode *orig_scope)
  {
    ROSE_ASSERT (SageInterface::is_Fortran_language() == true);
    // find the right scope (target body) to insert the declaration, start from the original scope
    SgBasicBlock* t_body = NULL;
  
    //find enclosing parallel region's body
    SgOmpParallelStatement * omp_stmt = isSgOmpParallelStatement(getEnclosingNode<SgOmpParallelStatement>(orig_scope));
    if (omp_stmt)
    {
      SgBasicBlock * omp_body = isSgBasicBlock(omp_stmt->get_body());
      ROSE_ASSERT(omp_body != NULL);
      t_body = omp_body;
    }
    else
    {
      // Find enclosing function body
      SgFunctionDefinition* func_def = getEnclosingProcedure (orig_scope);
      ROSE_ASSERT (func_def != NULL);
      SgBasicBlock * f_body = func_def->get_body();
      ROSE_ASSERT(f_body!= NULL);
      t_body = f_body;
    }
    ROSE_ASSERT (t_body != NULL);
    return t_body;
  }
#if 0 // moved to sageInterface.C
   // Insert a statement right after the last declaration with a target block.
   // Useful to insert the last declaration or the first non-declaration statement
   // to conform Fortran standard.
   void insertStatementAfterLastDeclaration(SgStatement* stmt, SgBasicBlock * target_block)
{
  // Insert to be the declaration after current declaration sequence, if any
  SgStatement* l_stmt = findLastDeclarationStatement (target_block);
  if (l_stmt)
    insertStatementAfter(l_stmt,stmt);
  else
    prependStatement(stmt, target_block);
}
// Insert a set of statement after the last declaration of a target block
// TODO refactor to SageInterface
    void insertStatementAfterLastDeclaration (std::vector<SgStatement*> stmt_list, SgBasicBlock * target_block)
{
  vector <SgStatement* >::iterator iter;
  SgStatement* prev_stmt = NULL;
  for (iter= stmt_list.begin(); iter != stmt_list.end(); iter++)
  {
    if (iter == stmt_list.begin())
    {
      insertStatementAfterLastDeclaration (*iter, target_block);
    }
    else
    {
      ROSE_ASSERT (prev_stmt != NULL);
      insertStatementAfter (prev_stmt, *iter);
    }
    prev_stmt = *iter; 
  }
}
#endif

//! This is a highly specialized operation which can find the right place to insert a Fortran variable declaration
//  during OpenMP lowering.
//
//  The reasons are: 
//    1)Fortran (at least F77) requires declaration statements to be consecutive within an enclosing function definition.
//    The C99-style generation of 'int loop_index' within a SgBasicBlock in the middle of some executable statement is illegal
//     for Fortran. We have to find the enclosing function body, located the declaration sequence, and add the new declaration 
//     after it. 
//
//    2) When translating OpenMP constructs within a parallel region, the declaration (such as those for private variables of the construct ) 
//       should be inserted into the declaration part of the body of the parallel region, which will become function body of the outlined
//       function when translating the region later on.
//       Insert the declaration to the current enclosing function definition is not correct. 
//
// Liao 1/12/2011
  SgVariableDeclaration * buildAndInsertDeclarationForOmp(const std::string &name, SgType *type, SgInitializer *varInit, SgBasicBlock *orig_scope)
{
  ROSE_ASSERT (SageInterface::is_Fortran_language() == true);
  SgVariableDeclaration * result = NULL;

  // find the right scope (target body) to insert the declaration, start from the original scope

  SgBasicBlock* t_body = NULL; 

#if 0
  //find enclosing parallel region's body
  SgOmpParallelStatement * omp_stmt = isSgOmpParallelStatement(getEnclosingNode<SgOmpParallelStatement>(orig_scope));
  if (omp_stmt)
  {
    SgBasicBlock * omp_body = isSgBasicBlock(omp_stmt->get_body());
    ROSE_ASSERT(omp_body != NULL);
    t_body = omp_body; 
  }
  else
  {
    // Find enclosing function body
    SgFunctionDefinition* func_def = getEnclosingProcedure (orig_scope);
    ROSE_ASSERT (func_def != NULL);
    SgBasicBlock * f_body = func_def->get_body();
    ROSE_ASSERT(f_body!= NULL);
    t_body = f_body; 
  }
  ROSE_ASSERT (t_body != NULL);  
#else
   t_body = getEnclosingRegionOrFuncDefinition(orig_scope);
#endif
  // Build the required variable declaration
  result = buildVariableDeclaration (name, type, varInit, t_body);

  // Insert to be the declaration after current declaration sequence, if any
#if 0 
  SgStatement* l_stmt = findLastDeclarationStatement (t_body);
  if (l_stmt)
    insertStatementAfter(l_stmt,result);
  else
    prependStatement(result, t_body);
#else
    insertStatementAfterLastDeclaration (result, t_body);
#endif    
  ROSE_ASSERT (result != NULL);
  return result;
}
    //! Translate clauses with variable lists, such as private, firstprivate, lastprivate, reduction, etc.
    //bb1 is the affected code block by the clause.
    //Command steps are: insert local declarations for the variables:(all)
    //                   initialize the local declaration:(firstprivate, reduction)
    //                   variable substitution for the variables:(all)
    //                   save local copy back to its global one:(reduction, lastprivate)
    // Note that a variable could be both firstprivate and lastprivate                  
    // Parameters:
    //     ompStmt: the OpenMP statement node with variable clauses
    //     bb1: the translation-generated basic block to implement ompStmt
    //     orig_loop_upper: 
    //       if ompStmt is loop construct, pass the original loop upper bound
    //       if ompStmt is omp sections, pass the section count - 1
    // This function is later extended to support OpenMP accelerator model. In this model,
    //    We have no concept of firstprivate or lastprivate
    //    reduction is implemented using a two-level reduction algorithm
    void transOmpVariables(SgStatement* ompStmt, SgBasicBlock* bb1, SgExpression * orig_loop_upper/*= NULL*/, bool isAcceleratorModel /*= false*/)
    {
      ROSE_ASSERT( ompStmt != NULL);
      ROSE_ASSERT( bb1 != NULL);
      SgOmpClauseBodyStatement* clause_stmt = isSgOmpClauseBodyStatement(ompStmt);
      ROSE_ASSERT( clause_stmt!= NULL);

      // collect variables 
     SgInitializedNamePtrList var_list = collectAllClauseVariables(clause_stmt);
     // Only keep the unique ones
     sort (var_list.begin(), var_list.end());;
     SgInitializedNamePtrList:: iterator new_end = unique (var_list.begin(), var_list.end());
     var_list.erase(new_end, var_list.end());
     VariableSymbolMap_t var_map; 
     ASTtools::VarSymSet_t var_set;
     
     vector <SgStatement* > front_stmt_list, end_stmt_list, front_init_list;  
    
// this is call by both transOmpTargetParallel and transOmpTargetLoop, we should move this to the correct caller place 
//      per_block_declarations.clear(); // must reset to empty or wrong reference to stale content generated previously
     for (size_t i=0; i< var_list.size(); i++)
     {
       SgInitializedName* orig_var = var_list[i];
       ROSE_ASSERT(orig_var != NULL);
       string orig_name = orig_var->get_name().getString();
       SgType* orig_type =  orig_var->get_type();
       SgVariableSymbol* orig_symbol = isSgVariableSymbol(orig_var->get_symbol_from_symbol_table());
       ROSE_ASSERT(orig_symbol!= NULL);

       VariantVector vvt (V_SgOmpPrivateClause);
       vvt.push_back(V_SgOmpReductionClause);

      //TODO: No such concept of firstprivate and lastprivate in accelerator model??
       if (!isAcceleratorModel) // we actually already has enable_accelerator, but it is too global for handling both CPU and GPU translation
       {
         vvt.push_back(V_SgOmpFirstprivateClause);
         vvt.push_back(V_SgOmpLastprivateClause);
       }
   
      // a local private copy
      SgVariableDeclaration* local_decl = NULL;
      SgOmpClause::omp_reduction_operator_enum r_operator = SgOmpClause::e_omp_reduction_unknown  ;
      bool isReductionVar = isInClauseVariableList(orig_var, clause_stmt,V_SgOmpReductionClause);

      // step 1. Insert local declaration for private, firstprivate, lastprivate and reduction
      // Sara, 5/31/2013: if variable is in Function Scope ( a parameter ) and array, 
      // we don't want a private copy, since the only thing private is the pointer, not the pointed data
      // We had a variable passed as private that has to be used as shared
      // We create a pointer to the variable and replace all the occurrences of the variable by the pointer
      // Example:
      // source code: 
      // void outlining( int M[10][10] ) {
      //   #pragma omp task firstprivate( M )
      //   M[0][0] = 4;
      // }
      // outlined parameters struct
      // struct OUT__17__7038___data {
      //   int (*M)[10UL];
      // };
      // outlined function:
      // static void OUT__17__7038__(void *__out_argv) {
      //   int (**M)[10UL] = (int (**)[10UL])(&(((struct OUT__17__7038___data *)__out_argv) -> M));
      //   (*M)[0][0] = 4;
      // }
      if (isInClauseVariableList(orig_var, clause_stmt, vvt))
      {
        if( !(isSgArrayType(orig_type) && isSgFunctionDefinition (orig_var->get_scope ())) )
        {
          SgInitializer * init = NULL;
          // use copy constructor for firstprivate on C++ class object variables
          // For simplicity, we handle C and C++ scalar variables the same way
          //
          // But here is one exception: an array type firstprivate variable should
          // be initialized element-by-element
          // Liao, 4/12/2010
          if (isInClauseVariableList(orig_var, clause_stmt,V_SgOmpFirstprivateClause) && !isSgArrayType(orig_type) )
          {  
            init = buildAssignInitializer(buildVarRefExp(orig_var, bb1));
          }
          
          string private_name;
          if (SageInterface::is_Fortran_language() )
          {
            // leading _ is not allowed in Fortran
            private_name = "i_"+orig_name;
            nCounter ++; // Fortran does not have basic block as a scope at source level
            // I have to generated all declarations at the same flat level under function definitions
            // So a name counter is needed to avoid name collision
            private_name = private_name + "_" + StringUtility::numberToString(nCounter);

            // Special handling for variable declarations in Fortran
            local_decl = buildAndInsertDeclarationForOmp (private_name, orig_type, init, bb1);
          }
          else
          {
            private_name = "_p_"+orig_name;
            local_decl = buildVariableDeclaration(private_name, orig_type, init, bb1);
            //ROSE_ASSERT (getFirst);isSgFunctionDefinition (orig_var->get_scope (
            //   prependStatement(local_decl, bb1);
            front_stmt_list.push_back(local_decl);   
          }
          // record the map from old to new symbol
          var_map.insert( VariableSymbolMap_t::value_type( orig_symbol, getFirstVarSym(local_decl)) ); 
        }
        else
        {
            var_set.insert(orig_symbol);
        }
      }
      // step 2. Initialize the local copy for array-type firstprivate variables TODO copyin, copyprivate
#if 1
      if (isInClauseVariableList(orig_var, clause_stmt,V_SgOmpFirstprivateClause) && 
          isSgArrayType(orig_type) && !isSgFunctionDefinition (orig_var->get_scope ()))
      {
        // SgExprStatement* init_stmt = buildAssignStatement(buildVarRefExp(local_decl), buildVarRefExp(orig_var, bb1));
        SgInitializedName* leftArray = getFirstInitializedName(local_decl); 
        SgBasicBlock* arrayAssign = generateArrayAssignmentStatements (leftArray, orig_var, bb1); 
       front_stmt_list.push_back(arrayAssign);   
      } 
#endif    
      if (isReductionVar) // create initial value assignment for the local reduction variable
      {
        r_operator = getReductionOperationType(orig_var, clause_stmt);
        SgExprStatement* init_stmt = buildAssignStatement(buildVarRefExp(local_decl), createInitialValueExp(r_operator));
        if (SageInterface::is_Fortran_language() )
        {
          // Fortran initialization statements  cannot be interleaved with declaration statements.
          // We save them here and insert them after all declaration statements are inserted.
          front_init_list.push_back(init_stmt);
        }
        else
        {
          front_stmt_list.push_back(init_stmt);   
        }
     }

      // Liao, 2/12/2013. For an omp for loop within "omp target". We translate its reduction variable by using 
      // a two-level reduction method: thread-block level (within kernel) and beyond-block level (done on CPU side).
      // So we have to insert a pointer to the array of per-block reduction results right before its enclosing "omp target" directive
      // The insertion point is decided so that the outliner invoked by transOmpTargetParallel() can later catch this newly introduced variable
      // and handle it in the parameter list properly. 
      //
      // e.g. REAL* per_block_results = (REAL *)xomp_deviceMalloc (numBlocks.x* sizeof(REAL));
       SgVariableDeclaration* per_block_decl = NULL; 
      if (isReductionVar && isAcceleratorModel)
      {
        SgOmpParallelStatement* enclosing_omp_parallel = getEnclosingNode<SgOmpParallelStatement> (ompStmt);
        ROSE_ASSERT (enclosing_omp_parallel!= NULL);
        //SgScopeStatement* scope_for_insertion = enclosing_omp_target->get_scope();
        SgScopeStatement* scope_for_insertion = isSgScopeStatement(enclosing_omp_parallel->get_scope());
        ROSE_ASSERT (scope_for_insertion != NULL);
        SgVarRefExp* blk_ref = buildVarRefExp("_num_blocks_", scope_for_insertion);
        SgExpression* multi_exp = buildMultiplyOp( blk_ref, buildSizeOfOp(orig_type) );
        SgExprListExp* parameter_list = buildExprListExp(multi_exp);
        SgExpression* init_exp = buildCastExp(buildFunctionCallExp(SgName("xomp_deviceMalloc"), buildPointerType(buildVoidType()), parameter_list, scope_for_insertion),  
                                              buildPointerType(orig_type));
       // the prefix of "_dev_per_block_" is important for later handling when calling outliner: add them into the parameter list
        per_block_decl = buildVariableDeclaration ("_dev_per_block_"+orig_name, buildPointerType(orig_type), buildAssignInitializer(init_exp), scope_for_insertion);
        // this statement refers to _num_blocks_, which will be declared later on when translating "omp parallel" enclosed in "omp target"
        // so we insert it  later when the kernel launch statement is inserted. 
        // insertStatementAfter(enclosing_omp_parallel, per_block_decl);
        per_block_declarations.push_back(per_block_decl);
        // store all reduction variables at the loop level, they will be used later when translating the enclosing "omp target" to help decide on the variables being passed
      }

      // step 3. Save the value back for lastprivate and reduction
      if (isInClauseVariableList(orig_var, clause_stmt,V_SgOmpLastprivateClause))
      {
        insertOmpLastprivateCopyBackStmts (ompStmt, end_stmt_list, bb1, orig_var, local_decl, orig_loop_upper);
      } else if (isReductionVar)
      {
        // two-level reduction is used for accelerator model 
        if (isAcceleratorModel)
          insertInnerThreadBlockReduction (r_operator, end_stmt_list, bb1, orig_var, local_decl, per_block_decl); 
        else 
          insertOmpReductionCopyBackStmts(r_operator, end_stmt_list, bb1, orig_var, local_decl);
      }

     } // end for (each variable)

   // step 4. Variable replacement for all original bb1
   replaceVariableReferences(bb1, var_map); 
   replaceVariablesWithPointerDereference(bb1, var_set); // Variables that must be replaced by a pointer to the variable

   // We delay the insertion of declaration, initialization , and save-back statements until variable replacement is done
   // in order to avoid replacing variables of these newly generated statements.
   prependStatementList(front_stmt_list, bb1); 
   // Fortran: add initialization statements after all front statements are inserted
  if (SageInterface::is_Fortran_language() )
  {
    SgBasicBlock * target_bb = getEnclosingRegionOrFuncDefinition (bb1);
    insertStatementAfterLastDeclaration (front_init_list, target_bb);
  }
   else
   {
     ROSE_ASSERT (front_init_list.size() ==0);
   }
   appendStatementList(end_stmt_list, bb1); 
#if 1
   // Liao 1/7/2010 , add assertion here, useful when generating outlined functions by moving statements to a function body
   SgStatementPtrList & srcStmts = bb1->get_statements(); 
   for (SgStatementPtrList::iterator i = srcStmts.begin(); i != srcStmts.end(); i++)
   {
     SgDeclarationStatement* declaration = isSgDeclarationStatement(*i);
     if (declaration != NULL)
       switch(declaration->variantT())
       {
         case V_SgVariableDeclaration:
           {
             // Reset the scopes on any SgInitializedName objects.
             SgVariableDeclaration* varDecl = isSgVariableDeclaration(declaration);
             SgInitializedNamePtrList & l = varDecl->get_variables();
             for (SgInitializedNamePtrList::iterator i = l.begin(); i != l.end(); i++)
             {
               // This might be an issue for extern variable declaration that have a scope
               // in a separate namespace of a static class member defined external to
               // its class, etc. I don't want to worry about those cases right now.
               ROSE_ASSERT((*i)->get_scope() == bb1);
             }
             break;
           }

         default:
           break;
       }

   } // end for
#endif    
  } // end void transOmpVariables()

  //  if (omp_get_thread_num () == 0) 
  //     { ... }
  //  Or if (XOMP_master())   
  //     { ...  }
  void transOmpMaster(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpMasterStatement* target = isSgOmpMasterStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );
    bool isLast = isLastStatement(target); // check this now before any transformation

    SgStatement* body = target->get_body();
    ROSE_ASSERT(body!= NULL );

#ifdef ENABLE_XOMP
   SgFunctionCallExp * func_call = buildFunctionCallExp("XOMP_master", buildIntType(), NULL, scope); 
   SgIfStmt* if_stmt = NULL; 
   if (SageInterface::is_Fortran_language())
      if_stmt = buildIfStmt(buildEqualityOp(func_call,buildIntVal(1)), body, NULL); 
   else   
      if_stmt = buildIfStmt(func_call, body, NULL); 
#else
    SgExpression* func_exp = buildFunctionCallExp("omp_get_thread_num", buildIntType(), NULL, scope);
    SgIfStmt* if_stmt = buildIfStmt(buildEqualityOp(func_exp,buildIntVal(0)), body, NULL); 
#endif
    replaceStatement(target, if_stmt,true);
    moveUpPreprocessingInfo (if_stmt, target, PreprocessingInfo::before);
    if (isLast) // the preprocessing info after the last statement may be attached to the inside of its parent scope
    {
  //    cout<<"Found a last stmt. scope is: "<<scope->class_name()<<endl;
  //    dumpPreprocInfo(scope);
  // move preprecessing info. from inside position to an after position    
      moveUpPreprocessingInfo (if_stmt, scope, PreprocessingInfo::inside, PreprocessingInfo::after);
    }
  }


  // Two cases: without or with copyprivate clause
  // without it: 
  //  if (GOMP_single_start ()) //bool GOMP_single_start (void)
  //     { ...       }
  // with it: TODO
  // TODO other clauses
  void transOmpSingle(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpSingleStatement* target = isSgOmpSingleStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );

    SgStatement* body = target->get_body();
    ROSE_ASSERT(body!= NULL );

   SgIfStmt* if_stmt = NULL; 

   if (SageInterface::is_Fortran_language())
   {
#ifdef ENABLE_XOMP
     SgExpression* func_exp = buildFunctionCallExp("XOMP_single", buildIntType(), NULL, scope);
#else
     //     SgExpression* func_exp = buildFunctionCallExp("GOMP_single_start", buildIntType(), NULL, scope);
     cerr<<"Fortran with Omni runtime is not yet implemented!"<<endl;
     ROSE_ASSERT (false);
#endif

     if_stmt = buildIfStmt(buildEqualityOp(func_exp,buildIntVal(1)), body, NULL); 
   }
   else // C/C++
   {
#ifdef ENABLE_XOMP
     SgExpression* func_exp = buildFunctionCallExp("XOMP_single", buildBoolType(), NULL, scope);
#else
     SgExpression* func_exp = buildFunctionCallExp("GOMP_single_start", buildBoolType(), NULL, scope);
#endif

     if_stmt = buildIfStmt(func_exp, body, NULL); 
   }

   replaceStatement(target, if_stmt,true);
    SgBasicBlock* true_body = ensureBasicBlockAsTrueBodyOfIf (if_stmt);
   if (SageInterface::is_Fortran_language())
     insert_libxompf_h (if_stmt); // need prototype for xomp runtime function
    transOmpVariables(target, true_body);
    // handle nowait 
    if (!hasClause(target, V_SgOmpNowaitClause))
    {
#ifdef ENABLE_XOMP
      SgExprStatement* barrier_call= buildFunctionCallStmt("XOMP_barrier", buildVoidType(), NULL, scope);
#else
      SgExprStatement* barrier_call= buildFunctionCallStmt("GOMP_barrier", buildVoidType(), NULL, scope);
#endif
      insertStatementAfter(if_stmt, barrier_call);
    }
  }

  // two cases: omp parallel and  omp task
  // Reused Outliner for this. 
  // void SgFunctionDeclaration* generateOutlinedFunction(SgNode* node)
  // {
  //   
  // }
  //
  //! Translation driver
  void translationDriver::visit(SgNode* node)
  {
#if 0 // not in use
    switch (node->variantT())
    {
      case V_SgOmpParallelStatement:
        {
          transOmpParallel(node);
          break;
        }
      case V_SgOmpForStatement:
        {
          transOmpFor(node);
          break;
        }
      case V_SgOmpBarrierStatement:
        {
          transOmpBarrier(node);
          break;
        }
      case V_SgOmpSingleStatement:
        {
          transOmpSingle(node);
          break;
        }

      default:
        {
          // do nothing here    
        }
    }// switch
#endif

  }//translationDriver::visit()

 //! Build a non-reduction variable clause for a given OpenMP directive. It directly returns the clause if the clause already exists
  SgOmpVariablesClause* buildOmpVariableClause(SgOmpClauseBodyStatement * clause_stmt, const VariantT& vt)
  {
    SgOmpVariablesClause* result = NULL;
    ROSE_ASSERT(clause_stmt != NULL);
    ROSE_ASSERT(vt != V_SgOmpReductionClause);
    Rose_STL_Container<SgOmpClause*> clauses = getClause(clause_stmt,vt);

    if (clauses.size()==0)
    {
      switch(vt)
      {
        case V_SgOmpCopyinClause:
          result = new SgOmpCopyinClause();
          break;
        case V_SgOmpCopyprivateClause:
          result = new SgOmpCopyprivateClause();
          break;
        case V_SgOmpFirstprivateClause:
          result = new SgOmpFirstprivateClause();
          break;
        case V_SgOmpLastprivateClause:
          result = new SgOmpLastprivateClause();
          break;
        case V_SgOmpPrivateClause:
          result = new SgOmpPrivateClause();
          break;
        case V_SgOmpSharedClause:
          result = new SgOmpSharedClause();
          break;
        case V_SgOmpReductionClause:
        default:
          cerr<<"Unacceptable clause type in OmpSupport::buildOmpVariableClause(): "<<vt<<endl;
          ROSE_ASSERT(false);
      }
    }
    else
    {
      result = isSgOmpVariablesClause(clauses[0]);
    }
    ROSE_ASSERT(result != NULL); 
    setOneSourcePositionForTransformation(result);

    clause_stmt->get_clauses().push_back(result);
    result->set_parent(clause_stmt); // is This right?

    return result;
  }

  //! Remove one or more clauses of type vt
  int removeClause (SgOmpClauseBodyStatement * clause_stmt, const VariantT& vt)
  {
    ROSE_ASSERT(clause_stmt != NULL);
    SgOmpClausePtrList& clause_list= clause_stmt->get_clauses ();  
    std::vector< Rose_STL_Container<SgOmpClause*>::iterator > iter_vec; 
    Rose_STL_Container<SgOmpClause*>::iterator iter ;
    // collect iterators pointing the matching clauses
    for (iter = clause_list.begin(); iter != clause_list.end(); iter ++)
    {
      SgOmpClause* c_clause = *iter;
      if (c_clause->variantT() == vt)
        iter_vec.push_back(iter);
    }

    //erase them one by one
   std::vector< Rose_STL_Container<SgOmpClause*>::iterator >::reverse_iterator r_iter;
   for (r_iter = iter_vec.rbegin(); r_iter!= iter_vec.rend();r_iter ++)
     clause_list.erase (*r_iter);
   return iter_vec.size();  
  }
   
  //! Add a variable into a non-reduction clause of an OpenMP statement, create the clause transparently if it does not exist
    void addClauseVariable(SgInitializedName* var, SgOmpClauseBodyStatement * clause_stmt, const VariantT& vt)
    {
      ROSE_ASSERT(var != NULL);
      ROSE_ASSERT(clause_stmt!= NULL);
      ROSE_ASSERT(vt != V_SgOmpReductionClause);
      Rose_STL_Container<SgOmpClause*> clauses = getClause(clause_stmt,vt );
      SgOmpVariablesClause* target_clause = NULL;
       // create the clause if it does not exist
      if (clauses.size()==0)
      {
        target_clause = buildOmpVariableClause (clause_stmt, vt);
      }
      else
      {
        target_clause = isSgOmpVariablesClause(clauses[0]);
      }
      ROSE_ASSERT(target_clause != NULL);

      // Insert only if the variable is not in the list
      if (!isInClauseVariableList(var, clause_stmt, vt)) 
      {
        target_clause->get_variables().push_back(buildVarRefExp(var));
      }
    }
// Patch up private variables for a single OpenMP For or DO loop    
// return the number of private variables added.
int patchUpPrivateVariables(SgStatement* omp_loop)
{
  int result = 0;
  ROSE_ASSERT ( omp_loop != NULL);
  SgOmpForStatement* for_node = isSgOmpForStatement(omp_loop);
  SgOmpDoStatement* do_node = isSgOmpDoStatement(omp_loop);
  if (for_node)
    omp_loop = for_node;
  else if (do_node)
    omp_loop = do_node;
  else
    ROSE_ASSERT (false);

  SgScopeStatement* directive_scope = omp_loop->get_scope();
  ROSE_ASSERT(directive_scope != NULL);
  // Collected nested loops and their indices
  // skip the top level loop?
  Rose_STL_Container<SgNode*> loops;
  if (for_node)
    loops = NodeQuery::querySubTree(for_node->get_body(), V_SgForStatement);
  else if (do_node)
    loops = NodeQuery::querySubTree(do_node->get_body(), V_SgFortranDo);
  else
    ROSE_ASSERT (false);
  // For all loops within the OpenMP loop
  Rose_STL_Container<SgNode*>::iterator loopIter = loops.begin();
  for (; loopIter!= loops.end(); loopIter++)
  {
    SgInitializedName* index_var = getLoopIndexVariable(*loopIter);
    ROSE_ASSERT (index_var != NULL);
    SgScopeStatement* var_scope = index_var->get_scope();
    // Only loop index variables declared in higher  or the same scopes matter
    if (isAncestor(var_scope, directive_scope) || var_scope==directive_scope)
    {
      // Grab possible enclosing parallel region
      bool isPrivateInRegion = false;
      SgOmpParallelStatement * omp_stmt = isSgOmpParallelStatement(getEnclosingNode<SgOmpParallelStatement>(omp_loop));
      if (omp_stmt)
      {
        isPrivateInRegion = isInClauseVariableList(index_var, isSgOmpClauseBodyStatement(omp_stmt), V_SgOmpPrivateClause);
      }
      // add it into the private variable list only if it is not specified as private in both the loop and region levels. 
      if (! isPrivateInRegion && !isInClauseVariableList(index_var, isSgOmpClauseBodyStatement(omp_loop), V_SgOmpPrivateClause))
      {
        result ++;
        addClauseVariable(index_var,isSgOmpClauseBodyStatement(omp_loop), V_SgOmpPrivateClause);
      }
    }

  } // end for loops
  return result;
} 
  //! Patch up private variables for omp for. The reason is that loop indices should be private by default and this function will make this explicit. This should happen before the actual translation is done.
  int patchUpPrivateVariables(SgFile* file)
  {
    int result = 0;
    ROSE_ASSERT(file != NULL);
    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(file, V_SgOmpForStatement);
    Rose_STL_Container<SgNode*> nodeList2 = NodeQuery::querySubTree(file, V_SgOmpDoStatement);
    
    Rose_STL_Container<SgNode*> nodeList_merged (nodeList.size() + nodeList2.size()) ;
    
    std::sort(nodeList.begin(), nodeList.end());
    std::sort(nodeList2.begin(), nodeList2.end());
    std::merge (nodeList.begin(), nodeList.end(), nodeList2.begin(), nodeList2.end(), nodeList_merged.begin());

    Rose_STL_Container<SgNode*>::iterator nodeListIterator = nodeList_merged.begin();
    // For each omp for/do statement
    for ( ;nodeListIterator !=nodeList_merged.end();  ++nodeListIterator)
    {
      SgStatement * omp_loop = NULL;
      SgOmpForStatement* for_node = isSgOmpForStatement(*nodeListIterator);
      SgOmpDoStatement* do_node = isSgOmpDoStatement(*nodeListIterator);
      if (for_node)
        omp_loop = for_node;
      else if (do_node)
        omp_loop = do_node;
      else
        ROSE_ASSERT (false);
    result +=  patchUpPrivateVariables (omp_loop);
#if 0
      SgScopeStatement* directive_scope = omp_loop->get_scope();
      ROSE_ASSERT(directive_scope != NULL);
      // Collected nested loops and their indices
      // skip the top level loop?
      Rose_STL_Container<SgNode*> loops; 
      if (for_node)
        loops = NodeQuery::querySubTree(for_node->get_body(), V_SgForStatement);
      else if (do_node)
        loops = NodeQuery::querySubTree(do_node->get_body(), V_SgFortranDo);
      else
        ROSE_ASSERT (false);

      Rose_STL_Container<SgNode*>::iterator loopIter = loops.begin();
      for (; loopIter!= loops.end(); loopIter++)
      {
        SgInitializedName* index_var = getLoopIndexVariable(*loopIter);
        ROSE_ASSERT (index_var != NULL);
        SgScopeStatement* var_scope = index_var->get_scope();
        // Only loop index variables declared in higher  or the same scopes matter
        if (isAncestor(var_scope, directive_scope) || var_scope==directive_scope)
        {
          // Grab possible enclosing parallel region
          bool isPrivateInRegion = false; 
          SgOmpParallelStatement * omp_stmt = isSgOmpParallelStatement(getEnclosingNode<SgOmpParallelStatement>(omp_loop)); 
          if (omp_stmt)
          {
            isPrivateInRegion = isInClauseVariableList(index_var, isSgOmpClauseBodyStatement(omp_stmt), V_SgOmpPrivateClause);
          }
          // add it into the private variable list only if it is not specified as private in both the loop and region levels. 
          if (! isPrivateInRegion && !isInClauseVariableList(index_var, isSgOmpClauseBodyStatement(omp_loop), V_SgOmpPrivateClause)) 
          {
            result ++;
            addClauseVariable(index_var,isSgOmpClauseBodyStatement(omp_loop), V_SgOmpPrivateClause);
          }
        }

      } // end for loops
#endif
    }// end for omp for statments
   return result;
  } // end patchUpPrivateVariables()

   //! Collect threadprivate variables within the current project, return a set to avoid duplicated elements
   std::set<SgInitializedName*> collectThreadprivateVariables()
   {
     // Do the actual collection only once
     static bool calledOnce = false;
     static set<SgInitializedName*> result;

     if (calledOnce)
       return result;
     calledOnce = true;
     std::vector<SgOmpThreadprivateStatement*> tp_stmts = getSgNodeListFromMemoryPool<SgOmpThreadprivateStatement> ();
     std::vector<SgOmpThreadprivateStatement*>::const_iterator c_iter;
     for (c_iter = tp_stmts.begin(); c_iter != tp_stmts.end(); c_iter ++)
     {
       SgVarRefExpPtrList refs = (*c_iter)->get_variables();
       SgInitializedNamePtrList var_list; // = (*c_iter)->get_variables();
       for (size_t j =0; j<refs.size(); j++)
         var_list.push_back(refs[j]->get_symbol()->get_declaration());
       std::copy(var_list.begin(), var_list.end(), std::inserter(result, result.end()));
     }
     return result;
   }
     
//Check if a variable that is determined to be shared in all enclosing constructs, up to and including the innermost enclosing
//parallel construct, is shared
// start_stmt is the start point to find enclosing OpenMP constructs. It is excluded as an enclosing construct for itself.
// TODO: we only check if it is shared to the innermost enclosing parallel construct for now
  static bool isSharedInEnclosingConstructs (SgInitializedName* init_var, SgStatement* start_stmt) 
  {
    bool result = false;
    ROSE_ASSERT(init_var != NULL);
    ROSE_ASSERT(start_stmt != NULL);
    SgScopeStatement* var_scope = init_var->get_scope();
//    SgScopeStatement* directive_scope = start_stmt->get_scope();
    // locally declared variables are private to the start_stmt
    // We should not do this here. It is irrelevant to this function.
   // if (isAncestor(start_stmt, init_var))
   //   return false;

//   cout<<"Debug omp_lowering.cpp isSharedInEnclosingConstructs() SgInitializedName name = "<<init_var->get_name().getString()<<endl;
    SgOmpParallelStatement* enclosing_par_stmt  = getEnclosingNode<SgOmpParallelStatement> (start_stmt, false);
    // Lexically nested within a parallel region
    if (enclosing_par_stmt)
    {
      // locally declared variables are private to enclosing_par_stmt
      SgScopeStatement* enclosing_construct_scope = enclosing_par_stmt->get_scope();
      ROSE_ASSERT(enclosing_construct_scope != NULL);
      if (isAncestor(enclosing_construct_scope, var_scope))
        return false;

      // Explicitly declared as a shared variable
      if (isInClauseVariableList(init_var, enclosing_par_stmt, V_SgOmpSharedClause))
        result = true;
      else
      {// shared by default
        VariantVector vv(V_SgOmpPrivateClause);
        vv.push_back(V_SgOmpFirstprivateClause);
        vv.push_back(V_SgOmpCopyinClause);
        vv.push_back(V_SgOmpReductionClause);
        if (isInClauseVariableList(init_var, enclosing_par_stmt,vv))
          result = false;
        else
          result = true;
      }
    }
    else 
     //the variable is in an orphaned construct
     // The variable could be
     // 1. a function parameter: it is private to its enclosing parallel region
     // 2. a global variable: either a threadprivate variable or shared by default
     // 3. is a variable declared within an orphaned function: it is private to its enclosing parallel region
     // ?? any other cases?? TODO
    {
      SgFunctionDefinition* func_def = getEnclosingFunctionDefinition(start_stmt);
      ROSE_ASSERT (func_def != NULL);
      if (isSgGlobal(var_scope))
      {
        set<SgInitializedName*> tp_vars = collectThreadprivateVariables();
        if (tp_vars.find(init_var)!= tp_vars.end())
          result = false; // is threadprivate
        else 
          result = true; // otherwise
      }
      else if (isSgFunctionParameterList(init_var->get_parent()))
      {
        // function parameters are private to its dynamically (non-lexically) nested parallel regions.
        result = false;
      }
      else if (isAncestor(func_def,var_scope))
      {
         // declared within an orphaned function, should be private
          result = false;
      } else
      {
#if 1
        cerr<<"Error: OmpSupport::isSharedInEnclosingConstructs() \n Unhandled variables within an orphaned construct:"<<endl;
        cerr<<"SgInitializedName name = "<<init_var->get_name().getString()<<endl;
        dumpInfo(init_var);
        init_var->get_file_info()->display("tttt");
        ROSE_ASSERT(false);
#endif        
      }
    }
    return result;
  } // end isSharedInEnclosingConstructs()

//! Patch up firstprivate variables for omp task. The reason is that the specification 3.0 defines rules for implicitly determined data-sharing attributes and this function will make the implicit firstprivate variable of omp task explicit.
/*
variables used in task block: 

2.9.1.1 Data-sharing Attribute Rules for Variables Referenced in a Construct
Ref. OMP 3.0 page 79 
A variable is firstprivate to the task (default) , if
** not explicitly specified by default(), shared(),private(), firstprivate() clauses
** not shared in enclosing constructs

It should also satisfy the restriction defined in specification 3.0 page 93  TODO
* cannot be a variable which is part of another variable (as an array or structure element)
* cannot be private, reduction
* must have an accessible, unambiguous copy constructor for the class type
* must not have a const-qualified type unless it is of class type with a mutable member
* must not have an incomplete C/C++ type or a reference type
*
I decided to exclude variables which are used by addresses when recognizing firstprivate variables 
      The reason is that in real code, it is often to have private variables first then use their
      address later.   Skipping the replacement will result in wrong semantics.
       e.g. from Allan Porterfield
          void    create_seq( double seed, double a )
      {
             double x, s;
             int    i, k;
     
      #pragma omp parallel private(x,s,i,k)
         { 
              // ..... 
             // here s is private 
             s = find_my_seed( myid, num_procs,
                               (long)4*NUM_KEYS, seed, a );
     
             for (i=k1; i<k2; i++)
             {
                 x = randlc(&s, &a); // here s is used by its address
     
             }
         }
      }   
If not, wrong code will be generated later on. The reason follows:
   * Considering nested omp tasks: 
         #pragma omp task untied
            {
              int j =100;
              // i is firstprivate, item is shared
              {
                for (i = 0; i < LARGE_NUMBER; i++)
                {
      #pragma omp task if(1) 
                  process (item[i],&j);
                }
              }
            }
   * the variable j will be firstprivate by default 
   * however, it is used by its address within a nested task (&j)
   * replacing it with its local copy will not get the right, original address.
   *
   * Even worse: the replacement will cause some later translation (outlining) to 
   * access the address of a parent task's local variable. 
   * It seems (not 100% certain!!!) that GOMP implements tasks as independent entities.
   * As a result a parent task's local stack will not be always accessible to its nested tasks.
   * A segmentation fault will occur when the lexically nested task tries to obtain the address of
   * its parent task's local variable. 
   * An example mistaken translation is shown below
       int main()
      {
        GOMP_parallel_start(OUT__3__1527__,0,0);
        OUT__3__1527__();
        GOMP_parallel_end();
        return 0;
      }
      
      void OUT__3__1527__()
      {
        if (GOMP_single_start()) {
          int i;
          printf(("Using %d threads.\n"),omp_get_num_threads());
          void *__out_argv2__1527__[1];
          __out_argv2__1527__[0] = ((void *)(&i));
          GOMP_task(OUT__2__1527__,&__out_argv2__1527__,0,4,4,1,1);
          //GOMP_task(OUT__2__1527__,&__out_argv2__1527__,0,4,4,1,0); //untied or not, no difference
        }
      }
      
      void OUT__2__1527__(void **__out_argv)
{
  int *i = (int *)(__out_argv[0]);
  //  int _p_i;
  //  _p_i =  *i;
  //  for (_p_i = 0; _p_i < 1000; _p_i++) {
  for (*i = 0; *i < 1000; (*i)++) {
    void *__out_argv1__1527__[1];
    // cannot access auto variable from the stack of another task instance!!
    //__out_argv1__1527__[0] = ((void *)(&_p_i));
    __out_argv1__1527__[0] = ((void *)(&(*i)));// this is the right translation
    GOMP_task(OUT__1__1527__,&__out_argv1__1527__,0,4,4,1,0);
  }
}
void OUT__1__1527__(void **__out_argv)
{
  int *i = (int *)(__out_argv[0]);
  int _p_i;
  _p_i =  *i;
  assert(_p_i>=0);
  assert(_p_i<10000);

  process((item[_p_i]));
}
*
  */
int patchUpFirstprivateVariables(SgFile*  file)
{
  int result = 0;
  ROSE_ASSERT(file != NULL);
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(file, V_SgOmpTaskStatement);
  Rose_STL_Container<SgNode*>::iterator iter = nodeList.begin();
  for (; iter != nodeList.end(); iter ++)
  {
    SgOmpTaskStatement * target = isSgOmpTaskStatement(*iter);
    SgScopeStatement* directive_scope = target->get_scope();
    SgStatement* body = target->get_body();
    ROSE_ASSERT(body != NULL);

    // Find all variable references from the task's body
    Rose_STL_Container<SgNode*> refList = NodeQuery::querySubTree(body, V_SgVarRefExp);
    Rose_STL_Container<SgNode*>::iterator var_iter = refList.begin();
    for (; var_iter != refList.end(); var_iter ++)
    {
      SgVarRefExp * var_ref = isSgVarRefExp(*var_iter);
      ROSE_ASSERT(var_ref->get_symbol() != NULL);
      SgInitializedName* init_var = var_ref->get_symbol()->get_declaration();
      ROSE_ASSERT(init_var != NULL);
      SgScopeStatement* var_scope = init_var->get_scope();
      ROSE_ASSERT(var_scope != NULL);

      // Variables with automatic storage duration that are declared in 
      // a scope inside the construct are private. Skip them
      if (isAncestor(directive_scope, var_scope))
        continue;

      if (SageInterface::isUseByAddressVariableRef(var_ref))
        continue;
      // Skip variables already with explicit data-sharing attributes
      VariantVector vv (V_SgOmpDefaultClause);
      vv.push_back(V_SgOmpPrivateClause);
      vv.push_back(V_SgOmpSharedClause);
      vv.push_back(V_SgOmpFirstprivateClause);
      if (isInClauseVariableList(init_var, target ,vv)) 
        continue;
      // Skip variables which are class/structure members: part of another variable
      if (isSgClassDefinition(init_var->get_scope()))
        continue;
      // Skip variables which are shared in enclosing constructs  
      if(isSharedInEnclosingConstructs(init_var, target))
        continue;
      // Now it should be a firstprivate variable   
      addClauseVariable(init_var, target, V_SgOmpFirstprivateClause);
      result ++;
    } // end for each variable reference
  } // end for each SgOmpTaskStatement
  return result;
} // end patchUpFirstprivateVariables()


/*
 * Winnie, Handle collapse clause before openmp and openmp accelerator
 * add new variables inserted by SageInterface::loopCollasping() into mapin clause
 *
 * This function passes target for loop of collpase clause and the collapse factor to the function SageInterface::loopCollapse.
 * After return from SageInterface::loopCollapse, this function will insert new variables(generated by loopCollapse()) into map to
 * or map tofrom clause, if the collapse clause comes with target directive.
 *
 */
void transOmpCollapse(SgOmpClauseBodyStatement * node)
{

  SgStatement * body =  node->get_body();
  ROSE_ASSERT(body != NULL);

  // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
  SgForStatement * for_loop = isSgForStatement(body);
  //SgStatement * loop = for_loop;

  if(for_loop == NULL)
    return;

  ROSE_ASSERT(getScope(for_loop)->get_parent()->get_parent() != NULL);

  Rose_STL_Container<SgOmpClause*> collapse_clauses = getClause(node, V_SgOmpCollapseClause);

  int collapse_factor = atoi(isSgOmpCollapseClause(collapse_clauses[0])->get_expression()->unparseToString().c_str());
  SgExprListExp * new_var_list = SageInterface::loopCollapsing(for_loop, collapse_factor);

  // remove the collapse clause
  removeClause(node,V_SgOmpCollapseClause);
  // we need to insert the loop index variable of the collapsed loop into the private() clause 
  patchUpPrivateVariables (node);

  /*
   *Winnie, we need to add the new variables into the map in list, if there is a SgOmpTargetStatement
   */
  /*For OmpTarget, we need to create SgOmpMapClause if there is no such clause in the original code.
   *   target_stmt, #pragma omp target
   *                or, #pragma omp parallel, when is not OmpTarget
   *   inside this if condition, ompacc=false means there is no map clause, we need to create one
   *   outside this if condition, ompacc=false means, no need to add new variables in the map in clause
   *   TODO: adding the variables into the map() clause is not sufficient.
   *         we have to move the corresponding variable declarations to be in front of the directive containing map(). 
   */
  SgStatement * target_stmt = isSgStatement(node->get_parent()->get_parent());
  if(isSgOmpTargetStatement(target_stmt))
  {
    Rose_STL_Container<SgOmpClause*> map_clauses;
    SgOmpMapClause * map_to;

    /*get the data clause of this target statement*/
    SgOmpClauseBodyStatement * target_clause_body = isSgOmpClauseBodyStatement(target_stmt); 

    map_clauses = target_clause_body->get_clauses();
    if(map_clauses.size() == 0 ) 
    {
      SgOmpTargetDataStatement * target_data_stmt = getEnclosingNode<SgOmpTargetDataStatement>(target_stmt);

      target_clause_body = isSgOmpClauseBodyStatement(target_data_stmt);
      map_clauses = target_clause_body->get_clauses();
    }

    assert(map_clauses.size() != 0);

    for(Rose_STL_Container<SgOmpClause*>::const_iterator iter = map_clauses.begin(); iter != map_clauses.end(); iter++)
    {
      SgOmpMapClause * temp_map_clause = isSgOmpMapClause(*iter);
      if(temp_map_clause != NULL) //Winnie, look for the map(to) or map(tofrom) clause
      {
        SgOmpClause::omp_map_operator_enum map_operator = temp_map_clause->get_operation();
        if(map_operator == SgOmpClause::e_omp_map_to || map_operator == SgOmpClause::e_omp_map_tofrom)
        {
          map_to = temp_map_clause;
          break;
        }
      }
    }

    if(map_to == NULL)
    {
      cerr <<"prepare to create a map in clause" << endl;
    }

    SgVarRefExpPtrList & mapto_var_list = map_to->get_variables();
    SgExpressionPtrList new_vars = new_var_list->get_expressions();
    for(size_t i = 0; i < new_vars.size(); i++)
    {
      mapto_var_list.push_back(deepCopy(isSgVarRefExp(new_vars[i])));
    }

    // TODO We also have to move the relevant variable declarations to sit in front of the map() clause
    // Liao 7/9/2014

  } // end if target
}//Winnie, end of loop collapse




//! Bottom-up processing AST tree to translate all OpenMP constructs
// the major interface of omp_lowering
// We now operation on scoped OpenMP regions and blocks
//    SgBasicBlock
//      /                   #
//     /                    #
// SgOmpParallelStatement   #
//          \               #
//           \              #
//           SgBasicBlock   #
//               \          #
//                \         #
//                SgOmpParallelStatement
void lower_omp(SgSourceFile* file)
{
  ROSE_ASSERT(file != NULL);

  patchUpPrivateVariables(file); // the order of these two functions matter! We want to patch up private variable first!
  patchUpFirstprivateVariables(file);
  // Liao 12/2/2010, Fortran does not require function prototypes
  if (!SageInterface::is_Fortran_language() )
    insertRTLHeaders(file);
  if (!enable_accelerator)
    insertRTLinitAndCleanCode(file);
  else
    insertAcceleratorInit(file);
  //    translationDriver driver;
  // SgOmpXXXStatment is compiler-generated and has no file info
  //driver.traverseWithinFile(file,postorder);
  //  driver.traverse(file,postorder);
  // AST manipulation with postorder traversal is not reliable,
  // We record nodes first then do changes to them

  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(file, V_SgStatement);
  Rose_STL_Container<SgNode*>::reverse_iterator nodeListIterator = nodeList.rbegin();
  for ( ;nodeListIterator !=nodeList.rend();  ++nodeListIterator)
  {
    SgStatement* node = isSgStatement(*nodeListIterator);
    ROSE_ASSERT(node != NULL);
    //debug the order of the statements
    //    cout<<"Debug lower_omp(). stmt:"<<node<<" "<<node->class_name() <<" "<< node->get_file_info()->get_line()<<endl;


    /*Winnie, handle Collapse clause.*/
    if(  isSgOmpClauseBodyStatement(node) != NULL && hasClause(isSgOmpClauseBodyStatement(node), V_SgOmpCollapseClause))
      transOmpCollapse(isSgOmpClauseBodyStatement(node));
#if 1 // debugging code after collapsing the loops     
    switch (node->variantT())
    {
      case V_SgOmpParallelStatement:
        {
          // check if this parallel region is under "omp target"
          SgNode* parent = node->get_parent();
          ROSE_ASSERT (parent != NULL);
          if (isSgBasicBlock(parent)) // skip the padding block in between.
            parent= parent->get_parent();
          if (isSgOmpTargetStatement(parent))
            transOmpTargetParallel(node);
          else  
            transOmpParallel(node);
          break;
        }
      case V_SgOmpSectionsStatement:
        {
          transOmpSections(node);
          break;
        }

      case V_SgOmpTaskStatement:
        {
          transOmpTask(node);
          break;
        }
      case V_SgOmpForStatement:
      case V_SgOmpDoStatement:
        {
          // check if the loop is part of the combined "omp parallel for" under the "omp target" directive
          // TODO: more robust handling of this logic, not just fixed AST form
          bool is_target_loop = false;
          SgNode* parent = node->get_parent();
          ROSE_ASSERT (parent != NULL);
          // skip a possible BB between omp parallel and omp for, especially when the omp parallel has multiple omp for loops 
          if (isSgBasicBlock(parent))
            parent = parent->get_parent();
          SgNode* grand_parent = parent->get_parent();
          ROSE_ASSERT (grand_parent != NULL);

          if (isSgOmpParallelStatement (parent) && isSgOmpTargetStatement(grand_parent) ) 
            is_target_loop = true;

          if (is_target_loop)
          {
            //            transOmpTargetLoop (node);
            // use round-robin scheduler for larger iteration space and better performance
            transOmpTargetLoop_RoundRobin(node);
          }
          else  
          { 
            transOmpLoop(node);
          }
          break;
        }
        //          {
        //            transOmpDo(node);
        //            break;
        //          }
      case V_SgOmpBarrierStatement:
        {
          transOmpBarrier(node);
          break;
        }
      case V_SgOmpFlushStatement:
        {
          transOmpFlush(node);
          break;
        }

      case V_SgOmpThreadprivateStatement:
        {
          transOmpThreadprivate(node);
          break;
        }
      case V_SgOmpTaskwaitStatement:
        {
          transOmpTaskwait(node);
          break;
        }
      case V_SgOmpSingleStatement:
        {
          transOmpSingle(node);
          break;
        }
      case V_SgOmpMasterStatement:
        {
          transOmpMaster(node);
          break;
        }
      case V_SgOmpAtomicStatement:
        {
          transOmpAtomic(node);
          break;
        }
      case V_SgOmpOrderedStatement:
        {
          transOmpOrdered(node);
          break;
        }
      case V_SgOmpCriticalStatement:
        {
          transOmpCritical(node);
          break;
        }
      case V_SgOmpTargetStatement:
        {
          transOmpTarget(node);
          break;
        }
      case V_SgOmpTargetDataStatement:
        {
          transOmpTargetData(node);
          break;
        }

      default:
        {
          // do nothing here    
        }
    }// switch

#endif

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

} // end namespace
