
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "sageBuilder.h"
#include "Outliner.hh"
#include "omp_lowering.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

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

  unsigned int nCounter = 0;
  //------------------------------------
  // Add include "xxxx.h" into source files, right before the first statement from users
  // Lazy approach: assume all files will contain OpenMP runtime library calls
  // TODO: (low priority) a better way is to only insert Headers when OpenMP is used.
  // 2/1/2008, try to use MiddleLevelRewrite to parse the content of the header, which
  //  should generate function symbols used for runtime function calls 
  //  But it is not stable!
  void insertRTLHeaders(SgSourceFile* file)
  {
    ROSE_ASSERT(file != NULL);    
    SgGlobal* globalscope = file->get_globalScope() ; //isSgGlobal(*i);
    ROSE_ASSERT (globalscope != NULL);
#ifdef ENABLE_XOMP
      SageInterface::insertHeader("libxomp.h",PreprocessingInfo::after,false,globalscope);
#else    
    if (rtl_type == e_omni)
      SageInterface::insertHeader("ompcLib.h",PreprocessingInfo::after,false,globalscope);
    else if (rtl_type == e_gomp)
      SageInterface::insertHeader("libgomp_g.h",PreprocessingInfo::after,false,globalscope);
    else
      ROSE_ASSERT(false);
#endif      
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
    SgStatement *l_stmt = findLastDeclarationStatement(currentscope);
    if (l_stmt != NULL)
    insertStatementAfter (l_stmt, varDecl1);
    else
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
      if_stmt->set_use_then_keyword(true); 
      if_stmt->set_has_end_statement(true); 
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
#if 0 // Liao 1/4/2010, use a call to XOMP_loop_default() instead of generating statements to calculate loop bounds      
      // ---------------------------------------------------------------
      // calculate chunksize for static scheduling , if the chunk size is not specified.
      if (!hasSpecifiedSize) // This portion is always on since hasSpecifiedSize will turn on transOmpLoop_others()
      {
        SgVariableDeclaration* chunk_decl=  buildVariableDeclaration("_p_chunk_size", buildIntType(), NULL,bb1);
        appendStatement(chunk_decl, bb1); 
        // for C-style for (i=lower; i< upper; i+=stride) 
        // int _p_iter_count = (stride + -1 + upper - lower )/stride; // if stride is positive (incremental iteration space)
        //or int _p_iter_count = (stride + 1 + upper - lower )/stride; // if stride is negative
        // we use inclusive upper bound (<= or >=) after loop normalization, so -1 or +1 is not needed.
        SgExpression * count_exp = buildDivideOp( buildAddOp(createAdjustedStride(orig_stride,isIncremental),
              buildSubtractOp(copyExpression(orig_upper), copyExpression(orig_lower) )) , 
            createAdjustedStride(orig_stride,isIncremental));
        SgVariableDeclaration* count_decl = buildVariableDeclaration("_p_iter_count",buildIntType(), buildAssignInitializer(count_exp), bb1);
        appendStatement(count_decl, bb1);
        // constant folding 
        constantFolding(count_decl->get_parent()); 
        //int _p_num_threads = omp_get_num_threads ();
        SgVariableDeclaration* num_threads_decl = buildVariableDeclaration("_p_num_threads", buildIntType(), 
            buildAssignInitializer(buildFunctionCallExp("omp_get_num_threads", buildVoidType(), NULL, bb1)),
            bb1);
        appendStatement(num_threads_decl, bb1);
        //  _p_chunk_size = _p_iter_count / _p_num_threads;
        SgExprStatement * assign1 = buildAssignStatement(buildVarRefExp(chunk_decl), 
            buildDivideOp(buildVarRefExp(count_decl), buildVarRefExp(num_threads_decl)) );  
        appendStatement(assign1, bb1);
        //  int _p_ck_temp = (_p_chunk_size * _p_num_threads) != _p_iter_count;
        SgVariableDeclaration* temp_decl = buildVariableDeclaration("_p_ck_temp", buildIntType(), 
            buildAssignInitializer( buildNotEqualOp(buildMultiplyOp(buildVarRefExp(chunk_decl), buildVarRefExp(num_threads_decl)) ,
                buildVarRefExp(count_decl)) ), 
            bb1);  
        appendStatement(temp_decl, bb1);
        // _p_chunk_size = _p_ck_temp + _p_chunk_size;
        SgExpression* temp_chunk = buildVarRefExp(chunk_decl);
        SgExprStatement * assign2 = buildAssignStatement(buildVarRefExp(chunk_decl), 
            buildAddOp(buildVarRefExp(temp_decl), temp_chunk)); 
        // buildAddOp(buildVarRefExp(temp_decl), buildVarRefExp(chunk_decl)));  // use temp_chunk once to avoid dangling expression
        appendStatement(assign2, bb1);
        my_chunk_size = temp_chunk; // now my_chunk_size has to be copied before reusing it!!
      }
      ROSE_ASSERT(my_chunk_size != NULL);
      ROSE_ASSERT(my_chunk_size->get_parent() != NULL);

      // adjust start and end iterations for the current thread for static scheduling
      // ---------------------------------------------------------------
      // int _p_thread_id = omp_get_thread_num ();
      SgVariableDeclaration* thread_id_decl = buildVariableDeclaration("_p_thread_id", buildIntType(), 
          buildAssignInitializer(buildFunctionCallExp("omp_get_thread_num", buildVoidType(), NULL, bb1)),
          bb1);
      appendStatement(thread_id_decl, bb1);
      // _p_lower = lower + _p_chunk_size * _p_thread_id * stride;
      SgExprStatement * assign_lower = buildAssignStatement(buildVarRefExp(lower_decl),
          buildAddOp(copyExpression(orig_lower), 
            buildMultiplyOp( buildMultiplyOp( copyExpression(my_chunk_size), buildVarRefExp(thread_id_decl)) , 
              createAdjustedStride(orig_stride,isIncremental) )) ); 
      appendStatement(assign_lower, bb1);
      // _p_upper = _p_lower + _p_chunk_size * stride;
      // _p_upper = _p_lower + (_p_chunk_size) * stride -1; // we normalized loop to have inclusive upper bound, so -1 is needed
      // _p_upper = _p_lower + (_p_chunk_size) * stride +1; // +1 if decremental
      int upperAdjust;  
      if (isIncremental) 
        upperAdjust = -1;
      else 
        upperAdjust = 1;
      SgExprStatement * assign_upper = buildAssignStatement(buildVarRefExp(upper_decl), buildAddOp(
            buildAddOp(buildVarRefExp(lower_decl), buildMultiplyOp(copyExpression(my_chunk_size), createAdjustedStride(orig_stride,isIncremental))), 
            buildIntVal(upperAdjust)));

      appendStatement(assign_upper, bb1);
      // _p_upper = _p_upper<upper? _p_upper: upper; 
      //or  _p_upper = _p_upper>upper? _p_upper: upper;  if decremental
      SgExpression* cond_exp = NULL;
      if (isIncremental)
        cond_exp = buildLessThanOp(buildVarRefExp(upper_decl), copyExpression(orig_upper));
      else
        cond_exp = buildGreaterThanOp(buildVarRefExp(upper_decl), copyExpression(orig_upper));
      assign_upper = buildAssignStatement(buildVarRefExp(upper_decl),
          buildConditionalExp(cond_exp, buildVarRefExp(upper_decl), copyExpression(orig_upper) ));
      appendStatement(assign_upper, bb1);

#else
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
#endif

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
// It calls the ROSE AST outliner internally. 
SgFunctionDeclaration* generateOutlinedTask(SgNode* node, std::string& wrapper_name, ASTtools::VarSymSet_t& syms, std::set<SgInitializedName*>& readOnlyVars, ASTtools::VarSymSet_t&pdSyms3)
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
  //This is however of low priority since most vendor compilers have this already
  SgBasicBlock* body_block = Outliner::preprocess(body);

  // Variable handling is done after Outliner::preprocess() to ensure a basic block for the body,
  // but before calling the actual outlining 
  // This simplifies the outlining since firstprivate, private variables are replaced 
  //with their local copies before outliner is used 
  transOmpVariables (target, body_block);

  ASTtools::VarSymSet_t pSyms, fpSyms,reductionSyms, pdSyms;

  string func_name = Outliner::generateFuncName(target);
  SgGlobal* g_scope = SageInterface::getGlobalScope(body_block);
  ROSE_ASSERT(g_scope != NULL);

  // This step is less useful for private, firstprivate, and reduction variables
  // since they are already handled by transOmpVariables(). 
  Outliner::collectVars(body_block, syms, pSyms);

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

  // Similarly , exclude private variable, also read only
 SgInitializedNamePtrList p_vars = collectClauseVariables (target, V_SgOmpPrivateClause);
 ASTtools::VarSymSet_t p_syms; //, pdSyms3;
 convertAndFilter (p_vars, p_syms);
  //TODO keep class typed variables!!!  even if they are firstprivate or private!! 
  set_difference (pdSyms2.begin(), pdSyms2.end(),
      p_syms.begin(), p_syms.end(),
      std::inserter(pdSyms3, pdSyms3.begin()));
 
  // lastprivate and reduction variables cannot be excluded  since write access to their shared copies

  // a data structure used to wrap parameters
  SgClassDeclaration* struct_decl = NULL; 
  if (SageInterface::is_Fortran_language())
    struct_decl = NULL;  // We cannot use structure for Fortran
  else  
     struct_decl = Outliner::generateParameterStructureDeclaration (body_block, func_name, syms, pdSyms3, g_scope);
  // ROSE_ASSERT (struct_decl != NULL); // can be NULL if no parameters to be passed

  //Generate the outlined function
  result = Outliner::generateFunction(body_block, func_name, syms, pdSyms3, pSyms, struct_decl, g_scope);
  Outliner::insert(result, g_scope, body_block);

  // A fix from Tristan Ravitch travitch@cs.wisc.edu to make outlined functions static to avoid name conflicts
  if (result->get_definingDeclaration() != NULL)
    SageInterface::setStatic(result->get_definingDeclaration());
  if (result->get_firstNondefiningDeclaration() != NULL)
    SageInterface::setStatic(result->get_firstNondefiningDeclaration());

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
     // generated an outlined function as the task
    std::string wrapper_name;
    ASTtools::VarSymSet_t syms; // store all variables in the outlined task ???
    ASTtools::VarSymSet_t pdSyms3; // store all variables which should be passed by references
    std::set<SgInitializedName*> readOnlyVars;
    SgFunctionDeclaration* outlined_func = generateOutlinedTask (node, wrapper_name, syms, readOnlyVars, pdSyms3);

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
    if (SageInterface::is_Fortran_language())
    { // The parameter list for Fortran is little bit different from C/C++'s XOMP interface 
      // since we are forced to pass variables one by one in the parameter list to support Fortran 77
       // void xomp_parallel_start (void (*func) (void *), unsigned* numThread, int * argcount, ...)
      //e.g. xomp_parallel_start(OUT__1__1527__,0,2,S,K)
      SgExpression * parameter2= buildIntVal(0); // TODO numThread not 0 
      SgExpression * parameter3 = buildIntVal (pdSyms3.size()); //TODO double check if pdSyms3 is the right set of variables to be passed
      parameters = buildExprListExp(buildFunctionRefExp(outlined_func), parameter2, parameter3);

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
      parameters = buildExprListExp(buildFunctionRefExp(outlined_func), parameter2, buildIntVal(0)); 
    }

    ROSE_ASSERT (parameters != NULL);

#ifdef ENABLE_XOMP
    SgExprStatement * s1 = buildFunctionCallStmt("XOMP_parallel_start", buildVoidType(), parameters, p_scope); 
    SageInterface::replaceStatement(target, s1 , true);
#else
    SgExprStatement * s1 = buildFunctionCallStmt("GOMP_parallel_start", buildVoidType(), parameters, p_scope); 
    SageInterface::insertStatementBefore(func_call, s1); 
#endif
    // Keep preprocessing information
    // I have to use cut-paste instead of direct move since 
    // the preprocessing information may be moved to a wrong place during outlining
    // while the destination node is unknown until the outlining is done.
   // SageInterface::moveUpPreprocessingInfo(s1, target, PreprocessingInfo::before); 
   pastePreprocessingInfo(s1, PreprocessingInfo::before, save_buf1); 
    // add GOMP_parallel_end ();
#ifdef ENABLE_XOMP
    SgExprStatement * s2 = buildFunctionCallStmt("XOMP_parallel_end", buildVoidType(), NULL, p_scope); 
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
    // Postprocessing  to ensure the AST is legal 
    // Should not rely on this usually.
    //   SgSourceFile* originalSourceFile = TransformationSupport::getSourceFile(g_scope);
    //   AstPostProcessing(originalSourceFile);
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
  TODO we now use a structure to wrap parameters, we have to go through each field of the structure to compute the size!
  */
  void transOmpTask(SgNode* node)
  {
    ROSE_ASSERT(node != NULL);
    SgOmpTaskStatement* target = isSgOmpTaskStatement(node);
    ROSE_ASSERT (target != NULL);

    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
    // Save preprocessing info as early as possible, avoiding mess up from the outliner
    AttachedPreprocessingInfoType save_buf1, save_buf2;
    cutPreprocessingInfo(target, PreprocessingInfo::before, save_buf1) ;
    cutPreprocessingInfo(target, PreprocessingInfo::after, save_buf2) ;
    // generated an outlined function as a task
    std::string wrapper_name;
    ASTtools::VarSymSet_t syms;
    ASTtools::VarSymSet_t pdSyms3; // store all variables which should be passed by reference
    std::set<SgInitializedName*> readOnlyVars;
    SgFunctionDeclaration* outlined_func = generateOutlinedTask (node, wrapper_name, syms, readOnlyVars, pdSyms3);

    SgScopeStatement * p_scope = target->get_scope();
    ROSE_ASSERT(p_scope != NULL);
    // Generate a call to it
   
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
    //  void GOMP_task (void (*fn) (void *), void *data, void (*cpyfn) (void *, void *), long arg_size, long arg_align, 
    //                  bool if_clause, unsigned flags)
    SgExpression * parameter_data = NULL;
    SgExpression * parameter_cpyfn = NULL;
    SgExpression * parameter_arg_size = NULL;
    SgExpression * parameter_arg_align = NULL;
    SgExpression * parameter_if_clause =  NULL;
    SgExpression * parameter_untied = NULL;
    size_t parameter_count = syms.size();
    if ( parameter_count == 0) // No parameters to be passed at all
    {
      parameter_data = buildIntVal(0);
      parameter_cpyfn=buildIntVal(0); // no copy function is needed
      parameter_arg_size = buildIntVal(0);
      parameter_arg_align = buildIntVal(0);
    }
    else
    {
      parameter_data =  buildAddressOfOp(buildVarRefExp(wrapper_name, p_scope));
      parameter_cpyfn=buildIntVal(0); // no special copy function for array of pointers
      // arg size of array of pointers = pointer_count * pointer_size
      // ROSE does not support cross compilation so sizeof(void*) can use as a workaround for now
      parameter_arg_size = buildIntVal( parameter_count* sizeof(void*));
      parameter_arg_align = buildIntVal(sizeof(void*));
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

    SgExprListExp* parameters = buildExprListExp(buildFunctionRefExp(outlined_func),
        parameter_data, parameter_cpyfn, parameter_arg_size, parameter_arg_align, parameter_if_clause, parameter_untied);

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
     SgOmpClause::omp_reduction_operator_enum result = SgOmpClause::e_omp_reduction_unkown;
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
          
       case SgOmpClause::e_omp_reduction_unkown:
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

static void insertOmpLastprivateCopyBackStmts(SgStatement* ompStmt, vector <SgStatement* >& end_stmt_list,  SgBasicBlock* bb1, SgInitializedName* orig_var, SgVariableDeclaration* local_decl, SgExpression* orig_loop_upper)
{
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
   * */
  // lastprivate can be used with loop constructs or sections.
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
    //TODO we need the original upper bound!!
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
  else
  {
    cerr<<"Unhandled SgOmpxx for lastprivate variable: \nOmpStatement is:"<< ompStmt->class_name()<<endl;
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
    case SgOmpClause::e_omp_reduction_unkown: 
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

  SgBasicBlock * getEnclosingRegionOrFuncDefinition(SgBasicBlock *orig_scope)
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
    void transOmpVariables(SgStatement* ompStmt, SgBasicBlock* bb1, SgExpression * orig_loop_upper/*= NULL*/)
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

     vector <SgStatement* > front_stmt_list, end_stmt_list, front_init_list;  
    
     for (size_t i=0; i< var_list.size(); i++)
     {
       SgInitializedName* orig_var = var_list[i];
       ROSE_ASSERT(orig_var != NULL);
       string orig_name = orig_var->get_name().getString();
       SgType* orig_type =  orig_var->get_type();
       SgVariableSymbol* orig_symbol = isSgVariableSymbol(orig_var->get_symbol_from_symbol_table());
       ROSE_ASSERT(orig_symbol!= NULL);

       VariantVector vvt (V_SgOmpPrivateClause);
       vvt.push_back(V_SgOmpFirstprivateClause);
       vvt.push_back(V_SgOmpLastprivateClause);
       vvt.push_back(V_SgOmpReductionClause);
   
      // a local private copy
      SgVariableDeclaration* local_decl = NULL;
      SgOmpClause::omp_reduction_operator_enum r_operator = SgOmpClause::e_omp_reduction_unkown  ;
      bool isReductionVar = isInClauseVariableList(orig_var, clause_stmt,V_SgOmpReductionClause);

      // step 1. Insert local declaration for private, firstprivate, lastprivate and reduction
      if (isInClauseVariableList(orig_var, clause_stmt, vvt))
      {
        SgInitializer * init = NULL;
        // use copy constructor for firstprivate on C++ class object variables
        // For simplicity, we handle C and C++ scalar variables the same way
        //
        // But here is one exception: an array type firstprivate variable should
        // be initialized element-by-element
        // Liao, 4/12/2010
        if (isInClauseVariableList(orig_var, clause_stmt,V_SgOmpFirstprivateClause) && !isSgArrayType(orig_type))
        {
          init = buildAssignInitializer(buildVarRefExp(orig_var, bb1));
        }
        string private_name = "_p_"+orig_name;

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
          //ROSE_ASSERT (getFirst);
          //   prependStatement(local_decl, bb1);
          front_stmt_list.push_back(local_decl);   
        }
        // record the map from old to new symbol
        var_map.insert( VariableSymbolMap_t::value_type( orig_symbol, getFirstVarSym(local_decl)) ); 
      }
      // step 2. Initialize the local copy for array-type firstprivate variables TODO copyin, copyprivate
#if 1
      if (isInClauseVariableList(orig_var, clause_stmt,V_SgOmpFirstprivateClause) && isSgArrayType(orig_type))
      {
        // SgExprStatement* init_stmt = buildAssignStatement(buildVarRefExp(local_decl), buildVarRefExp(orig_var, bb1));
        SgInitializedName* leftArray = getFirstInitializedName(local_decl); 
        SgBasicBlock* arrayAssign = generateArrayAssignmentStatements (leftArray, orig_var, bb1); 
       front_stmt_list.push_back(arrayAssign);   
      } 
#endif    
      if (isReductionVar)
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

      // step 3. Save the value back for lastprivate and reduction
      if (isInClauseVariableList(orig_var, clause_stmt,V_SgOmpLastprivateClause))
      {
        insertOmpLastprivateCopyBackStmts (ompStmt, end_stmt_list, bb1, orig_var, local_decl, orig_loop_upper);
      } else if (isReductionVar)
      { 
        insertOmpReductionCopyBackStmts(r_operator, end_stmt_list, bb1, orig_var, local_decl);
      }

     } // end for (each variable)

   // step 4. Variable replacement for all original bb1
   replaceVariableReferences(bb1, var_map); 

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
    SgIfStmt* if_stmt = buildIfStmt(func_call, body, NULL); 
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

#ifdef ENABLE_XOMP
    SgExpression* func_exp = buildFunctionCallExp("XOMP_single", buildBoolType(), NULL, scope);
#else
    SgExpression* func_exp = buildFunctionCallExp("GOMP_single_start", buildBoolType(), NULL, scope);
#endif
    SgIfStmt* if_stmt = buildIfStmt(func_exp, body, NULL); 
    replaceStatement(target, if_stmt,true);
    SgBasicBlock* true_body = ensureBasicBlockAsTrueBodyOfIf (if_stmt);
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
    
  //! Patch up private variables for omp for. The reason is that loop indices should be private by default and this function will make this explicit. This should happen before the actual translation is done.
  int patchUpPrivateVariables(SgFile* file)
  {
    int result = 0;
    ROSE_ASSERT(file != NULL);
    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(file, V_SgOmpForStatement);
    Rose_STL_Container<SgNode*>::iterator nodeListIterator = nodeList.begin();
    // For each omp for statement
    for ( ;nodeListIterator !=nodeList.end();  ++nodeListIterator)
    {
      SgOmpForStatement* node = isSgOmpForStatement(*nodeListIterator);
      SgScopeStatement* directive_scope = node->get_scope();
      ROSE_ASSERT(directive_scope != NULL);
      // Collected nested loops and their indices
      // skip the top level loop?
      Rose_STL_Container<SgNode*> loops = NodeQuery::querySubTree(node->get_body(), V_SgForStatement);
      Rose_STL_Container<SgNode*>::iterator loopIter = loops.begin();
      for (; loopIter!= loops.end(); loopIter++)
      {
        SgInitializedName* index_var = getLoopIndexVariable(*loopIter);
        ROSE_ASSERT (index_var != NULL);
        SgScopeStatement* var_scope = index_var->get_scope();
        // Only loop index variables declared in higher  or the same scopes matter
        if (isAncestor(var_scope, directive_scope) || var_scope==directive_scope)
        {
          // add it into the private variable list
          if (!isInClauseVariableList(index_var, node, V_SgOmpPrivateClause)) 
          {
            result ++;
            addClauseVariable(index_var,node, V_SgOmpPrivateClause);
          }
        }

      } // end for loops

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

    patchUpPrivateVariables(file);
    patchUpFirstprivateVariables(file);
    // Liao 12/2/2010, Fortran does not require function prototypes
    if (!SageInterface::is_Fortran_language() )
      insertRTLHeaders(file);
    insertRTLinitAndCleanCode(file);
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
      switch (node->variantT())
      {
        case V_SgOmpParallelStatement:
          {
            transOmpParallel(node);
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
            //transOmpFor(node);
            transOmpLoop(node);
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

        default:
          {
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

} // end namespace
