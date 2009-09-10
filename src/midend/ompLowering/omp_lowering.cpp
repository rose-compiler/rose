
#include "rose.h"
#include "Outliner.hh"
#include "omp_lowering.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;

namespace OmpSupport
{ 
  omp_rtl_enum rtl_type = e_gcc; /* default to  generate code targetting gcc's gomp */

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
    if (rtl_type == e_omni)
      SageInterface::insertHeader("ompcLib.h",PreprocessingInfo::after,false,globalscope);
    else if (rtl_type == e_gcc)
      SageInterface::insertHeader("libgomp_g.h",PreprocessingInfo::after,false,globalscope);
    else
      ROSE_ASSERT(false);
  }


#if 0
  //----------------------------
  //tasks:
  // * find the main entry for the application
  // * add (int argc, char *argv[]) if not exist(?)
  // * add runtime system init code at the begin 
  // * find all return points and append cleanup code
  // * add global declarations for threadprivate variables
  // * add global declarations for lock variables

  int insertRTLinitAndCleanCode(SgProject* project)
    //int insertRTLinitAndCleanCode(SgProject* project, OmpFrontend *ompfrontend)
  {
    bool hasMain= false;
    //find the main entry
    SgFunctionDefinition* mainDef=NULL;

    // DQ (1/6/2007): The correct qualified name for "main" is "::main", at least in C++.
    // however for C is should be "main".  Our name qualification is not language specific,
    // however, for C is makes no sense to as for the qualified name, so the name we
    // want to search for could be language specific.  The test code test2007_07.C 
    // demonstrates that the function "main" can exist in both classes (as member functions)
    // and in namespaces (as more meaningfully qualified names).  Because of this C++
    // would have to qualify the global main function as "::main", I think.  Fixing 
    // this details correctly turns on "hasMain" in the code below and fixes the linking
    // problem we were having!
    // string mainName = "main";
    string mainName = "::main";

    const SgFilePtrList& fileptrlist = project->get_fileList();

    for (std::vector<SgFile*>::const_iterator i = fileptrlist.begin();i!=fileptrlist.end();i++)
    {
      SgSourceFile* sgfile = isSgSourceFile(*i);
      ROSE_ASSERT(sgfile != NULL);

      SgFunctionDeclaration * mainDecl=findMain(sgfile);
      if (mainDecl!= NULL)
      {
        // printf ("Found main function setting hasMain == true \n");
        mainDef = mainDecl->get_definition();
        hasMain = true;
      }

      // declare pointers for threadprivate variables and global lock
      addGlobalOmpDeclarations(ompfrontend, sgfile->get_globalScope(), hasMain );

      if (hasMain){  // only insert into main function
        // add parameter  int argc , char* argv[] if not exist
        SgInitializedNamePtrList args = mainDef->get_declaration()->get_args();
        SgType * intType=  SgTypeInt::createType();
        SgType *charType=  SgTypeChar::createType();
#if 1
        if (args.size()==0){
          SgFunctionParameterList *parameterList = mainDef->get_declaration()->get_parameterList();
          ROSE_ASSERT(parameterList);

          // int argc
          SgName name1("argc");
          SgInitializedName *arg1 = buildInitializedName(name1,intType);

          //char* argv[]
          SgName name2("argv");
          SgPointerType *pType1= new SgPointerType(charType);
          SgPointerType *pType2= new SgPointerType(pType1);
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

        SgVarRefExp *var1 = buildVarRefExp(isSgInitializedName(*i), mainDef->get_body());
        SgVarRefExp *var2 = buildVarRefExp(isSgInitializedName(*++i), mainDef->get_body());

        SgExprListExp * exp_list_exp = buildExprListExp();
        appendExpression(exp_list_exp,var1);
        appendExpression(exp_list_exp,var2);

        SgExprStatement * expStmt=  buildFunctionCallStmt (SgName("_ompc_init"),
            buildVoidType(), exp_list_exp,currentscope);

        //  cout<<"debug:"<<expStmt->unparseToString()<<endl;
        //prepend to main body
        prependStatement(expStmt,currentscope);
        prependStatement(varDecl1,currentscope);

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
        SgExprStatement * expStmt2= buildFunctionCallStmt (SgName("_ompc_terminate"),
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
        return 0;
      } // end if hasMain

    } //end for sgfile
    return 0;
  }
#endif 

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
  int replaceVariableReferences(SgNode* root, VariableSymbolMap_t varRemap)
  {
    int result =0;
    typedef Rose_STL_Container<SgNode *> NodeList_t;
    NodeList_t refs = NodeQuery::querySubTree (root, V_SgVarRefExp);
    for (NodeList_t::iterator i = refs.begin (); i != refs.end (); ++i)
    {
      SgVarRefExp* ref_orig = isSgVarRefExp (*i);
      ROSE_ASSERT (ref_orig);
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
      return buildMultiplyOp(buildIntVal(-1),copyExpression(orig_stride));
  }

  //! check if an omp for loop use static schedule or not
  // Static schedule include: default schedule, or schedule(static[,chunk_size]) 
  bool useStaticSchedule(SgOmpForStatement* omp_for)
  {
    bool result= false; 
    ROSE_ASSERT(omp_for);
    Rose_STL_Container<SgOmpClause*> clauses = getClause(omp_for, V_SgOmpScheduleClause);
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

  //! Generate GOMP loop schedule start function's name
  string generateGOMPLoopStartFuncName (bool isOrdered, SgOmpClause::omp_schedule_kind_enum s_kind)
  {
    // GOMP_loop_static_start () 
    // GOMP_loop_ordered_static_start ()
    // GOMP_loop_dynamic_start () 
    // GOMP_loop_ordered_dynamic_start ()
    // .....
    string result;
    result = "GOMP_loop_";
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

    result = "GOMP_loop_";
    if (isOrdered)
      result +="ordered_";
    result += toString(s_kind);  
    result += "_next"; 
    return result;
  }
  //! Translate an omp for loop with non-static scheduling clause or with ordered clause ()
  // bb1 is the basic block to insert the translated loop
  // bb1 already has compiler-generated variable declarations for new loop control variables
  /*
   * start, end, incremental, chunk_size, own_start, own_end            
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
  */
  static void transOmpFor_nonStaticOrOrderedSchedule(SgOmpForStatement * target,  
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

    SgInitializedName* orig_index; 
    SgExpression* orig_lower, * orig_upper, * orig_stride; 
    bool isIncremental = true; // if the loop iteration space is incremental
    // grab the original loop 's controlling information
    bool is_canonical = isCanonicalForLoop (for_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental);
    ROSE_ASSERT(is_canonical == true);

    Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpScheduleClause);

    // the case of with the ordered schedule, but without any schedule policy specified
    // treat it as (static, 0) based on GCC's translation
    SgOmpClause::omp_schedule_kind_enum s_kind = SgOmpClause::e_omp_schedule_static;
    SgExpression* orig_chunk_size = buildIntVal(0);
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
    int upper_adjust = 1;  // we use inclusive bounds, adjust them accordingly 
    if (!isIncremental) 
      upper_adjust = -1;
    SgExprListExp* para_list = buildExprListExp(copyExpression(orig_lower), buildAddOp(copyExpression(orig_upper), buildIntVal(upper_adjust)),
        createAdjustedStride(orig_stride, isIncremental)); 
    if (s_kind != SgOmpClause::e_omp_schedule_auto && s_kind != SgOmpClause::e_omp_schedule_runtime)
    {
      appendExpression(para_list, copyExpression(orig_chunk_size));
    }
    appendExpression(para_list, buildAddressOfOp(buildVarRefExp(lower_decl)));
    appendExpression(para_list, buildAddressOfOp(buildVarRefExp(upper_decl)));
    SgFunctionCallExp* func_start_exp = buildFunctionCallExp(func_start_name, buildBoolType(), para_list, bb1);
    SgBasicBlock * true_body = buildBasicBlock();
    SgIfStmt* if_stmt = buildIfStmt(func_start_exp, true_body, NULL);
    appendStatement(if_stmt, bb1);
    // do {} while (GOMP_loop_static_next (&_p_lower, &_p_upper))
    SgExpression* func_next_exp = buildFunctionCallExp(generateGOMPLoopNextFuncName(hasOrder, s_kind), buildBoolType(),
        buildExprListExp(buildAddressOfOp(buildVarRefExp(lower_decl)), buildAddressOfOp(buildVarRefExp(upper_decl))), bb1);
    SgBasicBlock * do_body = buildBasicBlock();
    SgDoWhileStmt * do_while_stmt = buildDoWhileStmt(do_body, func_next_exp);
    appendStatement(do_while_stmt, true_body);
    // insert the loop into do-while
    appendStatement(for_loop, do_body);
    // Rewrite loop control variables
    replaceVariableReferences(for_loop,isSgVariableSymbol(orig_index->get_symbol_from_symbol_table ()), 
                      getFirstVarSym(index_decl));
    int upperAdjust;
    if (isIncremental)  // adjust the bounds again, inclusive bound so -1 for incremental loop
      upperAdjust = -1;
    else 
      upperAdjust = 1;
    SageInterface::setLoopLowerBound(for_loop, buildVarRefExp(lower_decl));
    SageInterface::setLoopUpperBound(for_loop, buildAddOp(buildVarRefExp(upper_decl),buildIntVal(upperAdjust)));

    transOmpVariables(target, bb1); // This should happen before the barrier is inserted.
    // GOMP_loop_end ();  or GOMP_loop_end_nowait (); 
    string func_loop_end_name = "GOMP_loop_end"; 
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
  // We translate static schedule here and non-static ones in transOmpFor_nonStaticOrOrderedSchedule()
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
  void transOmpFor(SgNode* node)
  {
    ROSE_ASSERT(node != NULL);
    SgOmpForStatement* target = isSgOmpForStatement(node);
    ROSE_ASSERT (target != NULL);
    SgScopeStatement* p_scope = target->get_scope();
    ROSE_ASSERT (p_scope != NULL);

    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);
    // The OpenMP syntax requires that the omp for pragma is immediately followed by the for loop.
    SgForStatement * for_loop = isSgForStatement(body);
    // Step 1. Loop normalization
    // we reuse the normalization from SageInterface, though it is different from what gomp expects.
    // the point is to have a consistent loop form. We can adjust the difference later on.
    SageInterface::forLoopNormalization(for_loop);
    SgInitializedName * orig_index = NULL;
    SgExpression* orig_lower = NULL;
    SgExpression* orig_upper= NULL;
    SgExpression* orig_stride= NULL;
    bool isIncremental = true; // if the loop iteration space is incremental
    // grab the original loop 's controlling information
    bool is_canonical = isCanonicalForLoop (for_loop, &orig_index, & orig_lower, &orig_upper, &orig_stride, NULL, &isIncremental);
    ROSE_ASSERT(is_canonical == true);

    // step 2. Insert a basic block to replace SgOmpForStatement
    SgBasicBlock * bb1 = SageBuilder::buildBasicBlock(); 
    replaceStatement(target, bb1, true);
    //TODO handle preprocessing information
    // Save some preprocessing information for later restoration. 
    //  AttachedPreprocessingInfoType ppi_before, ppi_after;
    //  ASTtools::cutPreprocInfo (s, PreprocessingInfo::before, ppi_before);
    //  ASTtools::cutPreprocInfo (s, PreprocessingInfo::after, ppi_after);

    // Declare local loop control variables: _p_loop_index _p_loop_lower _p_loop_upper , no change to the original stride
    SgVariableDeclaration* index_decl =  buildVariableDeclaration("_p_index",  buildIntType(), NULL,bb1); 
    SgVariableDeclaration* lower_decl =  buildVariableDeclaration("_p_lower",  buildIntType(), NULL,bb1); 
    SgVariableDeclaration* upper_decl =  buildVariableDeclaration("_p_upper",  buildIntType(), NULL,bb1); 

    appendStatement(index_decl, bb1);
    appendStatement(lower_decl, bb1);
    appendStatement(upper_decl, bb1);

    bool hasOrder = false;
    if (hasClause(target, V_SgOmpOrderedClause))
      hasOrder = true;

    //  step 3. Translation for omp for 
    //if (hasClause(target, V_SgOmpScheduleClause)) 
    if (!useStaticSchedule(target) || hasOrder) 
    {
      transOmpFor_nonStaticOrOrderedSchedule( target,   index_decl, lower_decl,   upper_decl, bb1);
    }
    else 
    {
      // Grab or calculate chunk_size
      SgExpression* my_chunk_size = NULL; 
      bool hasSpecifiedSize = false;
      Rose_STL_Container<SgOmpClause*> clauses = getClause(target, V_SgOmpScheduleClause);
      if (clauses.size() !=0)
      {
        SgOmpScheduleClause* s_clause = isSgOmpScheduleClause(clauses[0]);
        ROSE_ASSERT(s_clause);
        SgOmpClause::omp_schedule_kind_enum s_kind = s_clause->get_kind();
        ROSE_ASSERT(s_kind == SgOmpClause::e_omp_schedule_static);
        SgExpression* orig_chunk_size = s_clause->get_chunk_size();  
        if (orig_chunk_size)
        {
          hasSpecifiedSize = true;
          my_chunk_size = orig_chunk_size;
        }
      }
      // ---------------------------------------------------------------
      // calculate chunksize for static scheduling , if the chunk size is not specified.
      if (!hasSpecifiedSize)
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
            buildAddOp(buildVarRefExp(temp_decl), buildVarRefExp(chunk_decl))); 
        // buildAddOp(buildVarRefExp(temp_decl), buildVarRefExp(chunk_decl)));  // use temp_chunk once to avoid dangling expression
        appendStatement(assign2, bb1);
        my_chunk_size = temp_chunk; // now my_chunk_size has to be copied before reusing it!!
      }
      ROSE_ASSERT(my_chunk_size != NULL);

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

      // add loop here
      appendStatement(for_loop, bb1); 
      // replace loop index with the new one
      replaceVariableReferences(for_loop,
         isSgVariableSymbol(orig_index->get_symbol_from_symbol_table()), getFirstVarSym(index_decl))    ; 
      // rewrite the lower and upper bounds
      SageInterface::setLoopLowerBound(for_loop, buildVarRefExp(lower_decl)); 
      SageInterface::setLoopUpperBound(for_loop, buildVarRefExp(upper_decl)); 

       transOmpVariables(target, bb1); // This should happen before the barrier is inserted.
      // insert barrier if there is no nowait clause
      if (!hasClause(target, V_SgOmpNowaitClause)) 
      {
        //insertStatementAfter(for_loop, buildFunctionCallStmt("GOMP_barrier", buildVoidType(), NULL, bb1));
        appendStatement(buildFunctionCallStmt("GOMP_barrier", buildVoidType(), NULL, bb1), bb1);
      }
    }

    // handle variables 
    // transOmpVariables(target, bb1); // This should happen before the barrier is inserted.
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

  /* GCC's libomp uses the following translation method: 
   * (we use array of pointers instead of structure of variables to pass data)
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
  void transParallelRegion (SgNode* node)
  {
    // Replace the region with a call to it
#if 0   // 
    SgFunctionDeclaration *outFuncDecl= generateOutlinedFunction(decl);
    ROSE_ASSERT(outFuncDecl !=NULL);
    insertOutlinedFunction(decl, outFuncDecl);

    SgBasicBlock *rtlCall= generateParallelRTLcall(decl, outFuncDecl,ompattribute);
    ROSE_ASSERT(rtlCall !=NULL );
    replacePragmaBlock(decl, rtlCall);
#endif 

    ROSE_ASSERT(node != NULL);
    SgOmpParallelStatement* target = isSgOmpParallelStatement(node);
    ROSE_ASSERT (target != NULL);

    // printf("translating a parallel region..\n");
    SgStatement * body =  target->get_body();
    ROSE_ASSERT(body != NULL);

    //Initialize outliner 

    // always wrap parameters for outlining used during OpenMP translation
    Outliner::useParameterWrapper = true; 

#if 0 // This is not necessary since Outliner::preprocess() will call it. 
    if (!Outliner::isOutlineable(body))
    {
      cout<<"OmpSupport::transParallelRegion() found a region body which can not be outlilned!\n"
        <<body->unparseToString()<<endl;
      ROSE_ASSERT(false);
    }
#endif   
    // Save preprocessing info as early as possible, avoiding mess up from the outliner
    AttachedPreprocessingInfoType save_buf1, save_buf2;
    cutPreprocessingInfo(target, PreprocessingInfo::before, save_buf1) ;
    cutPreprocessingInfo(target, PreprocessingInfo::after, save_buf2) ;

    //TODO there should be some semantics check for the regions to be outlined
    //for example, multiple entries or exists are not allowed for OpenMP
    //We should have a semantic check phase for this
    //This is however of low priority since most vendor compilers have this already
    SgBasicBlock* body_block = Outliner::preprocess(body);
    transOmpVariables (target, body_block);

    ASTtools::VarSymSet_t syms, pSyms, fpSyms,reductionSyms, pdSyms;
    std::set<SgInitializedName*> readOnlyVars;

    string func_name = Outliner::generateFuncName(target);
    SgGlobal* g_scope = SageInterface::getGlobalScope(body_block);
    ROSE_ASSERT(g_scope != NULL);

    Outliner::collectVars(body_block, syms, pSyms, fpSyms, reductionSyms);
    //TODO Do we want to use side effect analysis to improve the quality of outlining here
    // SageInterface::collectReadOnlyVariables(s,readOnlyVars);
    // ASTtools::collectPointerDereferencingVarSyms(s,pdSyms);

    //Generate the outlined function
    SgFunctionDeclaration* outlined_func = Outliner::generateFunction(body_block, func_name,
        syms, pdSyms, pSyms, fpSyms, reductionSyms, g_scope);
    Outliner::insert(outlined_func, g_scope, body_block);

    // Generate a call to the outlined function
    // Generate packing statements
    std::string wrapper_name;
    // must pass target , not body_block to get the right scope in which the declarations are inserted
    wrapper_name= Outliner::generatePackingStatements(target,syms);
    SgScopeStatement * p_scope = target->get_scope();
    ROSE_ASSERT(p_scope != NULL);

    SgStatement* func_call = Outliner::generateCall (outlined_func, syms, readOnlyVars, wrapper_name,p_scope);
    ROSE_ASSERT(func_call != NULL);  

    // Replace the parallel region with the function call statement
    // TODO should we introduce another level of scope here?
    SageInterface::replaceStatement(target,func_call, true);

    //add GOMP_parallel_start (OUT_func_xxx, &__out_argv1__5876__, 0);
    // or GOMP_parallel_start (OUT_func_xxx, 0, 0); // if no variables need to be passed
    SgExpression * parameter2 = NULL;
    if (syms.size()==0)
      parameter2 = buildIntVal(0);
    else
      parameter2 =  buildAddressOfOp(buildVarRefExp(wrapper_name, p_scope));
    SgExprListExp* parameters = buildExprListExp(buildFunctionRefExp(outlined_func), 
        parameter2, buildIntVal(0)); 
    SgExprStatement * s1 = buildFunctionCallStmt("GOMP_parallel_start", buildVoidType(), parameters, p_scope); 
    SageInterface::insertStatementBefore(func_call, s1); 
    // Keep preprocessing information
    // I have to use cut-paste instead of direct move since 
    // the preprocessing information may be moved to a wrong place during outlining
    // while the destination node is unknown until the outlining is done.
   // SageInterface::moveUpPreprocessingInfo(s1, target, PreprocessingInfo::before); 
   pastePreprocessingInfo(s1, PreprocessingInfo::before, save_buf1); 
    // add GOMP_parallel_end ();
    SgExprStatement * s2 = buildFunctionCallStmt("GOMP_parallel_end", buildVoidType(), NULL, p_scope); 
    SageInterface::insertStatementAfter(func_call, s2); 
   // SageInterface::moveUpPreprocessingInfo(s2, target, PreprocessingInfo::after); 
   pastePreprocessingInfo(s1, PreprocessingInfo::after, save_buf2); 

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
    SgExprStatement* func_call_stmt1 = buildFunctionCallStmt("GOMP_atomic_start", buildVoidType(), NULL, scope);
    SgExprStatement* func_call_stmt2 = buildFunctionCallStmt("GOMP_atomic_end", buildVoidType(), NULL, scope);
    insertStatementBefore(body, func_call_stmt1);
    insertStatementAfter(body, func_call_stmt2);
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
    SgExprStatement* func_call_stmt1 = buildFunctionCallStmt("GOMP_ordered_start", buildVoidType(), NULL, scope);
    SgExprStatement* func_call_stmt2 = buildFunctionCallStmt("GOMP_ordered_end", buildVoidType(), NULL, scope);
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
      SgVariableDeclaration* vardecl = buildVariableDeclaration(g_lock_name, buildIntType(), NULL, global);
      //SgVariableDeclaration* vardecl = buildVariableDeclaration(g_lock_name, buildOpaqueType("gomp_mutex_t",global), NULL, global);
      setStatic(vardecl);
      prependStatement(vardecl,global);
      SgExprListExp * param1= buildExprListExp(buildAddressOfOp(buildVarRefExp(vardecl)));
      SgExprListExp * param2= buildExprListExp(buildAddressOfOp(buildVarRefExp(vardecl)));
      func_call_stmt1 = buildFunctionCallStmt("GOMP_critical_name_start", buildVoidType(), param1, scope);
      func_call_stmt2 = buildFunctionCallStmt("GOMP_critical_name_end", buildVoidType(), param2, scope);
    }
    insertStatementBefore(body, func_call_stmt1);
    insertStatementAfter(body, func_call_stmt2);
  }


  //! Simply replace the pragma with a function call to void GOMP_barrier (void); 
  void transOmpBarrier(SgNode * node)
  {
    ROSE_ASSERT(node != NULL );
    SgOmpBarrierStatement* target = isSgOmpBarrierStatement(node);
    ROSE_ASSERT(target != NULL );
    SgScopeStatement * scope = target->get_scope();
    ROSE_ASSERT(scope != NULL );

    SgExprStatement* func_call_stmt = buildFunctionCallStmt("GOMP_barrier", buildVoidType(), NULL, scope);
    replaceStatement(target, func_call_stmt, true);
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
      result2 = isSgOmpVariablesClause(p_clause[i])->get_variables();  
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
       SgInitializedNamePtrList var_list = isSgOmpVariablesClause(r_clause)->get_variables();
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
  //! Translate clauses with variable lists, such as private, firstprivate, lastprivate, reduction, etc.
  //bb1 is the affected code block by the clause.
  //Command steps are: insert local declarations for the variables:(all)
  //                   initialize the local declaration:(firstprivate, reduction)
  //                   variable substitution for the variables:(all)
  //                   save local copy back to its global one:(reduction, lastprivate)
  // Note that a variable could be both firstprivate and lastprivate                  
  void transOmpVariables(SgStatement* ompStmt, SgBasicBlock* bb1)
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

   vector <SgStatement* > front_stmt_list, end_stmt_list;  
  
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
      local_decl = buildVariableDeclaration("_p_"+orig_name, orig_type, NULL, bb1);
     //   prependStatement(local_decl, bb1);
     front_stmt_list.push_back(local_decl);   
      // record the map from old to new symbol
     var_map.insert( VariableSymbolMap_t::value_type( orig_symbol, getFirstVarSym(local_decl)) ); 
    }

    // step 2. Initialize the local copy for firstprivate, reduction TODO copyin, copyprivate?
    if (isInClauseVariableList(orig_var, clause_stmt,V_SgOmpFirstprivateClause))
    {
      SgExprStatement* init_stmt = buildAssignStatement(buildVarRefExp(local_decl), buildVarRefExp(orig_var, bb1));
     front_stmt_list.push_back(init_stmt);   
    } else if (isReductionVar)
    {
      r_operator = getReductionOperationType(orig_var, clause_stmt);
      SgExprStatement* init_stmt = buildAssignStatement(buildVarRefExp(local_decl), createInitialValueExp(r_operator));
      front_stmt_list.push_back(init_stmt);   
    }

    // step 3. Save the value back for lastprivate and reduction
     if (isInClauseVariableList(orig_var, clause_stmt,V_SgOmpLastprivateClause))
    {
      SgExprStatement* save_stmt = buildAssignStatement(buildVarRefExp(orig_var, bb1), buildVarRefExp(local_decl));
     // appendStatement(save_stmt, bb1);
     end_stmt_list.push_back(save_stmt);   
    } else if (isReductionVar)
    { // Two ways to do the reduction operation: 
      //1. builtin function TODO
      //    __sync_fetch_and_add_4(&shared, (unsigned int)local);
      //2. using atomic runtime call: 
      //    GOMP_atomic_start ();
      //    shared = shared op local;
      //    GOMP_atomic_end ();
      // We use the 2nd method only for now for simplicity and portability
      SgExprStatement* atomic_start_stmt = buildFunctionCallStmt("GOMP_atomic_start", buildVoidType(), NULL, bb1); 
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
      SgExprStatement* atomic_end_stmt = buildFunctionCallStmt("GOMP_atomic_end", buildVoidType(), NULL, bb1);  
      end_stmt_list.push_back(atomic_end_stmt);   
    }
    
   } // end for (each variable)

   // step 4. Variable replacement for all original bb1
   replaceVariableReferences(bb1, var_map); 

   // We delay the insertion of declaration, initialization , and save-back statements until variable replacement is done
   // in order to avoid replacing variables of these newly generated statements.
   prependStatementList(front_stmt_list, bb1); 
   appendStatementList(end_stmt_list, bb1); 
    
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

    SgStatement* body = target->get_body();
    ROSE_ASSERT(body!= NULL );

    SgExpression* func_exp = buildFunctionCallExp("omp_get_thread_num", buildIntType(), NULL, scope);

    SgIfStmt* if_stmt = buildIfStmt(buildEqualityOp(func_exp,buildIntVal(0)), body, NULL); 
    replaceStatement(target, if_stmt,true);
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

    SgExpression* func_exp = buildFunctionCallExp("GOMP_single_start", buildBoolType(), NULL, scope);
    SgIfStmt* if_stmt = buildIfStmt(func_exp, body, NULL); 
    replaceStatement(target, if_stmt,true);
    SgBasicBlock* true_body = ensureBasicBlockAsTrueBodyOfIf (if_stmt);
    transOmpVariables(target, true_body);
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
          transParallelRegion(node);
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
        target_clause->get_variables().push_back(var);
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


    insertRTLHeaders(file);
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
            transParallelRegion(node);
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
