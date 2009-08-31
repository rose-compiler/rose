
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
   //TODO handle preprocessing information
   // Save some preprocessing information for later restoration. 
   //  AttachedPreprocessingInfoType ppi_before, ppi_after;
   //     ASTtools::cutPreprocInfo (s, PreprocessingInfo::before, ppi_before);
   //     ASTtools::cutPreprocInfo (s, PreprocessingInfo::after, ppi_after);

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
   //TODO there should be some semantics check for the regions to be outlined
   //for example, multiple entries or exists are not allowed for OpenMP
   //We should have a semantic check phase for this
   //This is however of low priority since most vendor compilers have this already
   SgBasicBlock* body_block = Outliner::preprocess(body);
   ASTtools::VarSymSet_t syms, pSyms, fpSyms,reductionSyms, pdSyms;
   std::set<SgInitializedName*> readOnlyVars;

   string func_name = Outliner::generateFuncName(target);
   SgGlobal* g_scope = SageInterface::getGlobalScope(body_block);
   ROSE_ASSERT(g_scope != NULL);

   Outliner::collectVars(body_block, syms, pSyms, fpSyms,reductionSyms);
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
   SageInterface::replaceStatement(target,func_call);

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
  // add GOMP_parallel_end ();
  SgExprStatement * s2 = buildFunctionCallStmt("GOMP_parallel_end", buildVoidType(), NULL, p_scope); 
  SageInterface::insertStatementAfter(func_call, s2); 

   // Postprocessing  to ensure the AST is legal 
   // Should not rely on this usually.
   //   SgSourceFile* originalSourceFile = TransformationSupport::getSourceFile(g_scope);
   //   AstPostProcessing(originalSourceFile);
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
    switch (node->variantT())
    {
      case V_SgOmpParallelStatement:
        {
          transParallelRegion(node);
          break;
        }
      default:
        {
          // do nothing here    
        }
    }// switch

  }//translationDriver::visit()

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

    insertRTLHeaders(file);

    translationDriver driver;
    driver.traverseWithinFile(file,postorder);
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
