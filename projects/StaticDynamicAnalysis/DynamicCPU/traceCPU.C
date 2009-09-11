
/*
                              COPYRIGHT

Copyright 1993-2007, Lawrence Livermore National Security, LLC. All
rights reserved.  This work was produced at the Lawrence Livermore
National Laboratory (LLNL) under contract no. DE-AC52-07NA27344
(Contract 44) between the U.S. Department of Energy (DOE) and Lawrence
Livermore National Security, LLC (LLNS) for the operation of
LLNL. Copyright is reserved to Lawrence Livermore National Security,
LLC for purposes of controlled dissemination, commercialization
through formal licensing, or other disposition under terms of Contract
44; DOE policies, regulations and orders; and U.S. statutes. The
rights of the Federal Government are reserved under Contract 44.

                              DISCLAIMER

This work was prepared as an account of work sponsored by an agency of
the United States Government. Neither the United States Government nor
Lawrence Livermore National Security, LLC nor any of their employees,
makes any warranty, express or implied, or assumes any liability or
responsibility for the accuracy, completeness, or usefulness of any
information, apparatus, product, or process disclosed, or represents
that its use would not infringe privately-owned rights. Reference
herein to any specific commercial products, process, or service by
trade name, trademark, manufacturer or otherwise does not necessarily
constitute or imply its endorsement, recommendation, or favoring by
the United States Government or Lawrence Livermore National Security,
LLC. The views and opinions of authors expressed herein do not
necessarily state or reflect those of the United States Government or
Lawrence Livermore National Security, LLC, and shall not be used for
advertising or product endorsement purposes.

                        LICENSING REQUIREMENTS

Any use, reproduction, modification, or distribution of this software
or documentation for commercial purposes requires a license from
Lawrence Livermore National Security, LLC. Contact: Lawrence Livermore
National Laboratory, Industrial Partnerships Office, P.O. Box 808,
L-795, Livermore, CA 94551.

The Government is granted for itself and others acting on its behalf a
paid-up, nonexclusive, irrevocable worldwide license in this data to
reproduce, prepare derivative works, and perform publicly and display
publicly.

*/

/*
   Trace Analysis Tool
   -------------------
   This tool is designed to provide high level program execution
   information to help guide the design of hardware and software
   features relevant to the application being traced.

   The high level information breaks down an application into
   a series of loops and sequential code segments.  For each
   loop and segment, information about dependencies, critical
   paths, and maximum parallelism are recorded.   The tool
   is split over several components including a dynamic CPU
   profiler, dynamic memory profiler, and static dependence
   analyzer.

   This file contains the dynamic CPU profiler.  This profiler
   provides a low overhead way to break the program into loops
   and sequential segments, and measure typical hardware counter
   information with respect to each loop and segment.

   Proposed by Ravi Nair, IBM T.J. Watson Research Center
   Initial Implementation by Jeff Keasler, LLNL
*/

#include "rose.h"
#include "sageBuilder.h"
using namespace SageBuilder ;
using namespace std ;

static const char *dumpFunc ;
static SgType *ETtype ;

#define ANCHOR Sg_File_Info::generateDefaultFileInfoForTransformationNode()

/*****************************************************************************/
/* if (<var> == 0) <var> = <registerFuncName>(__FILE__, __FUNC__, __LINE__)  */
/*****************************************************************************/

static void RegisterItem(SgFunctionDeclaration *funcDecl, SgBasicBlock *funcBody,
                         SgStatement *stmt, const char *registerFuncName, 
                         SgType *varType, SgVariableDeclaration *var,
                         bool beforeStmt)
{
   Sg_File_Info *fileInfo ;

   if (!isSgForStatement(stmt))
   {
     fileInfo = stmt->get_file_info() ;
   }
   else
   {
     fileInfo = (beforeStmt ?
                 stmt->get_startOfConstruct() : stmt->get_endOfConstruct()) ;
   }

   SgExprListExp *args = new SgExprListExp(ANCHOR) ;
   args->append_expression(buildStringVal(fileInfo->get_filenameString())) ;
   args->append_expression(buildStringVal(funcDecl->get_name().str())) ;
   args->append_expression(buildIntVal(fileInfo->get_line())) ;
   SgFunctionCallExp *registerCall = buildFunctionCallExp(
      SgName(registerFuncName), varType, args, funcBody) ;
          
   SgExprStatement *varAssign =
      buildExprStatement(buildAssignOp(buildVarRefExp(var), registerCall)) ;

   SgExpression *varEqualZero =
      buildEqualityOp(buildVarRefExp(var), buildIntVal(0)) ;
         
   SgIfStmt *statement = buildIfStmt( varEqualZero, varAssign, NULL ) ;
   stmt->get_scope()->insert_statement(stmt, statement, beforeStmt) ;

   return ;
}

/*********************************/
/*   <stopSeqFuncName>(<idVar>)  */
/*********************************/

static void HandleItem(SgBasicBlock *funcBody, SgStatement *stmt,
                       const char *stopSeqFuncName, SgExpression *expr,
                       bool beforeStmt)
{
   SgExprListExp *args = new SgExprListExp(ANCHOR) ;
   if (expr != 0)
   {
      args->append_expression(expr) ;
   }
   SgExprStatement *statement = buildFunctionCallStmt(
      SgName(stopSeqFuncName), new SgTypeVoid(), args, funcBody) ;
   stmt->get_scope()->insert_statement(stmt, statement, beforeStmt) ;

   return ;
}

/****************************************/
/* [static] <varType> <basename><index> */
/****************************************/

static SgVariableDeclaration *ET_BuildDecl(SgBasicBlock *funcBody,
                                           const char *baseName, int index,
                                           SgType *varType,
                                           bool isStatic = false)
{
   char tmpName[64] ;
   sprintf(tmpName, "%s%d", baseName, index) ;
   SgVariableDeclaration *varDecl = 
      buildVariableDeclaration(SgName(tmpName), varType, NULL, funcBody)  ;
   if (isStatic)
   {
      SageInterface::setStatic(varDecl) ;
   }
   funcBody->prepend_statement(varDecl) ;

   return varDecl ;
}

/************************************************************/
/* If a typedef of the given name doesn't exist, create it. */
/* Use the surrogate type as a decoy since we don't know    */
/* the real type.                                           */
/************************************************************/

static SgType *GetTypedef(SgGlobal *globalScope, const char *name,
                          SgType *surrogateType)
{
   SgTypedefDeclaration *typeDecl ;
   SgType *type ;

   if (!globalScope->symbol_exists(SgName(name)))
   {
      /* Note that typeDecl is not added to AST */
      typeDecl =
         new SgTypedefDeclaration(ANCHOR, SgName(name), surrogateType) ;
      ROSE_ASSERT(typeDecl) ;
      typeDecl->get_file_info()->unsetOutputInCodeGeneration() ;
      typeDecl->set_parent(globalScope) ;
   }
   else
   {
     SgTypedefSymbol *ts = globalScope->lookup_typedef_symbol(SgName(name)) ;
     ROSE_ASSERT(ts) ;
     typeDecl = ts->get_declaration() ;
     ROSE_ASSERT(typeDecl) ;
   }
   type = isSgType(new SgTypedefType(typeDecl)) ;
   ROSE_ASSERT(type) ;

   return type ;
}

/************************************************************/
/* Determine if a statement contains a call to an unknown   */
/* function, or a function known to have non-sequential     */
/* behavior.                                                */
/************************************************************/

static bool ContainsCall(SgStatement *stmt)
{
   static std::set< std::string > segDB ;
   bool containsUnknownCall = false ;

   if (segDB.empty())
   {
      char funcName[128] ;
      FILE *fp ;
      if ((fp = fopen("SegDB.txt", "r")) != NULL)
      {
         while(fgets(funcName, 128, fp))
         {
            funcName[strlen(funcName)-1] = 0 ;
            segDB.insert(funcName) ;
         }
         fclose(fp) ;
      } 
      else
      {
         printf("File SEGDB.txt is absent. Segment results degraded.\n") ;
         segDB.insert("_____") ;
      }
   }
   /* check to see if this statement contains any function calls */
   Rose_STL_Container<SgNode*> calls =
       NodeQuery::querySubTree(stmt, V_SgFunctionCallExp) ;

   for (Rose_STL_Container<SgNode*>::iterator c_itr = calls.begin();
           c_itr != calls.end(); ++c_itr)
   {
      SgFunctionCallExp *stmt = isSgFunctionCallExp(*c_itr) ;
      ROSE_ASSERT(stmt);

      SgFunctionRefExp *func = isSgFunctionRefExp(stmt->get_function()) ;
      if (func != NULL)
      {
         SgFunctionSymbol *funcName = func->get_symbol() ;

         if (segDB.find(funcName->get_name().getString()) == segDB.end())
         {
            containsUnknownCall = true ;
            break ;
         }
      }
      else
      {
         /* Since I can't handle this case, assume the worst -- for now */
         containsUnknownCall = true ;
         break ;
      }
   }

   return containsUnknownCall ;
}

/************************************************************/
/* Determine the loop-nest level of a statement within      */
/* a function.                                              */
/************************************************************/

static int ForLoopDepth(SgNode *baseScope, SgNode *stmt )
{
   int level = 0 ;
   for (SgNode *scan = stmt; scan != baseScope; scan = scan->get_parent())
   {
      if (isSgForStatement(scan))
      {
        ++level ;
      }
   }

   return level ;
}

/************************************************************/
/*       Add a caliper around a sequential code segment.    */
/************************************************************/

static void CreateSegmentProbe(SgFunctionDeclaration *funcDecl, SgBasicBlock *funcBody,
                              SgStatement *begin, SgStatement *end, int *segmentCount)
{
   SgVariableDeclaration *stopSegDecl =
      ET_BuildDecl(funcBody, "ET_stopSeg", *segmentCount, ETtype, true) ;
   SgVariableDeclaration *startSegDecl =
      ET_BuildDecl(funcBody, "ET_startSeg", *segmentCount, ETtype, true) ;

   /* Start segment code fragment */
   RegisterItem(funcDecl, funcBody, begin,
                "ET_RegisterStartSeg", ETtype, startSegDecl, true) ;
   HandleItem(funcBody, begin, "ET_StartSeg", buildVarRefExp(startSegDecl), true) ;

   /* Stop segment code fragment */
   RegisterItem(funcDecl, funcBody, end,
                "ET_RegisterStopSeg", ETtype, stopSegDecl, true) ;
   HandleItem(funcBody, end, "ET_StopSeg", buildVarRefExp(stopSegDecl), true) ;

   ++(*segmentCount) ;
}

/*************************************************/
/*       Add a caliper around a loop segment.    */
/*************************************************/

static void CreateLoopProbe(SgFunctionDeclaration *funcDecl, SgBasicBlock *funcBody,
                            SgStatement *stmt, int *loopCount)
{
   SgVariableDeclaration *loopDecl =
      ET_BuildDecl(funcBody, "ET_loop", *loopCount, ETtype, true) ;

   /* start loop code fragment */
   RegisterItem(funcDecl, funcBody, stmt,
                "ET_RegisterLoop", ETtype, loopDecl, true) ;
   HandleItem(funcBody, stmt, "ET_PushLoopSeqId", buildVarRefExp(loopDecl), true) ;

   /* end loop code fragment */
   HandleItem(funcBody, stmt, "ET_PopLoopSeqId", 0, false) ;

   ++(*loopCount) ;
}

/*****************************************************/
/*       Create loop and sequential code segment.    */
/*****************************************************/

/*
    This assumes (1) a single segment should not cross a function boundary,
    and (2) variable declarations contain no work.

    (1) can be addressed by guaranteeing that a start-segment is active
        before the function call, a stop-segment exists after the call
        to the function, a stop-segment is placed directly before the
        first loop in every function (if a loop exists) and a start-segment
        is placed directly after the last loop in every function (if a
        loop exists).   This will create alot of overhead unless a
        database is created to squeeze out unneccessary segments (see
        INFO file).
    (2) can be addressed by creating an artifical scope to enclose the
        scanScope, and by placing a start-segment at the top of the
        artifical scope before the scanScope.  Of course, if you are
        parsing C++ instead of C, there is no reason to create an
        artificial scope.
*/

void TransformFunction(SgFunctionDeclaration *funcDecl, SgBasicBlock *funcBody,
                     SgBasicBlock *scanScope, int *loopCount, int *segmentCount)
{
   bool simpleWork = true ;
   SgStatement *segBegin = NULL ;
   SgStatement *stmt ;

   Rose_STL_Container<SgNode*> blockStmts =
       NodeQuery::querySubTree(scanScope, V_SgStatement,
                               AstQueryNamespace::ChildrenOnly) ;

   for (Rose_STL_Container<SgNode*>::iterator s_itr = blockStmts.begin();
           s_itr != blockStmts.end(); ++s_itr)
   {
      stmt = isSgStatement(*s_itr) ;
      ROSE_ASSERT(stmt);

      // printf("%s\n", stmt->sage_class_name()) ;

      /* assume no work in variable declarations -- for now */
      if (isSgDeclarationStatement(stmt))
         continue ;

      if (isSgForStatement(stmt)) /* could be embedded inside a statement */
      {
         SgForStatement *forStatement = isSgForStatement(stmt) ;
         SgBasicBlock *loopBody = isSgBasicBlock(forStatement->get_loop_body()) ;
         ROSE_ASSERT(loopBody) ;
         if (segBegin != NULL)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
           simpleWork = false ;
           segBegin = NULL ;
         }
         CreateLoopProbe(funcDecl, funcBody, stmt, loopCount) ;
         TransformFunction(funcDecl, funcBody, loopBody, loopCount, segmentCount) ; 
      }
      else if (isSgReturnStmt(stmt)) /* could be embedded inside a statement */
      {
         /* Note -- we should probably put the return statement in block scope */
         /* before adding the extra code */
         int level = ForLoopDepth(funcBody, stmt) ;
         if (segBegin != NULL)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
           simpleWork = false ;
           segBegin = NULL ;
         }
         if (level != 0)
         {
            HandleItem(funcBody, stmt, "ET_PopLoopSeqIds", buildIntVal(level), true) ;
         }
      }
      else if (isSgGotoStatement(stmt)) /* could be embedded inside a stmt */
      {
         /* ASSUMPTION:  We will always be jumping out of loops, not into them */
         /* Note -- we should probably put the goto statement in block scope */
         /* before adding the extra code */
         int gotoLevel = ForLoopDepth(funcBody, stmt) ;
         int labelLevel = ForLoopDepth(funcBody, isSgGotoStatement(stmt)->get_label()) ;
         int levelDiff = gotoLevel - labelLevel ;
         if (segBegin != NULL)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
           simpleWork = false ;
           segBegin = NULL ;
         }
         if (levelDiff > 0)
         {
            HandleItem(funcBody, stmt, "ET_PopLoopSeqIds", buildIntVal(levelDiff), true) ;
         }
         else if (levelDiff < 0)
         {
            printf("goto jump into a loop context unexpected.  Terminating.\n") ;
            exit(-1) ;
         }
      }
      else if (isSgLabelStatement(stmt) ||
               isSgBreakStmt(stmt) ||
               isSgContinueStmt(stmt)) /* could be embedded inside a stmt */
      {
         if (segBegin != NULL)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
           simpleWork = false ;
           segBegin = NULL ;
         }
      }
      /*
      // These two clauses can be used to count iterations on a per-loop basis
      // rather than on a per-iteration basis. Note that goto and return also
      // need to be modified to do the bulk counting.
      else if (isSgBreakStmt(stmt))
      {
      }
      else if (isSgContinueStmt(stmt))
      {
      }
      */
      else if (ContainsCall(stmt))
      {
         if (segBegin != NULL)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
           simpleWork = false ;
           segBegin = NULL ;
         }
      }
      else
      {
         if (segBegin == NULL)
         {
#ifndef DISABLE_SEQUENTIAL_SEGMENTS
            segBegin = stmt ;
#endif
         }
      }
   }

   if ((segBegin != NULL) && !simpleWork)
   {
      CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
   }
}


/******************************************************/
/*                Driver Routine                      */
/******************************************************/

int main (int argc, char *argv[])
{
   /* indicate whether include files need to be added */
   bool showStats = false ;
   bool loopTransformApplied = false ;

   /***********************************************/
   /* Process command line options                */
   /***********************************************/

   Rose_STL_Container<string> cmdLineArgs =
     CommandlineProcessing::generateArgListFromArgcArgv(argc,argv) ;

   if ( CommandlineProcessing::isOption(
           cmdLineArgs, "-et:", "(s|stats)", true) )
   {
     showStats = true ;
   }

   dumpFunc = (showStats ? "ET_LogStats" : "ET_Dump") ;

   /***********************************************/
   /*               Invoke ROSE                   */
   /***********************************************/

   /* build AST */
   SgProject* project = frontend(argc, argv);
   ROSE_ASSERT(project);

   /* make sure AST is well formed */
   AstTests::runAllTests(project);

   // generateDOT (*project);

   /* set up some needed typedefs for runtime support */

   SgGlobal *globalScope = SageInterface::getFirstGlobalScope(project) ;
   pushScopeStack(isSgScopeStatement(globalScope)) ;
   ETtype = GetTypedef(globalScope, "ET_Idx_t", new SgTypeShort() ) ;
   popScopeStack();   /* pop globalScope */

   /* insert probes into each function in this file */

   Rose_STL_Container<SgNode*> funcDefs =
       NodeQuery::querySubTree(project, V_SgFunctionDefinition) ;

   for (Rose_STL_Container<SgNode*>::iterator f_itr = funcDefs.begin();
              f_itr != funcDefs.end(); ++f_itr)
   {
      SgFunctionDefinition *funcDef = isSgFunctionDefinition(*f_itr) ;
      ROSE_ASSERT(funcDef);

#ifdef ET_DEBUG 
      printf("--- %s ---\n", funcDef->get_qualified_name().str()) ;
#endif

      SgBasicBlock *funcBody = funcDef->get_body() ;
      if (funcBody == NULL)
         continue ;  /* should be impossible to get here... */

      SgFunctionDeclaration *funcDecl = funcDef->get_declaration() ;
      ROSE_ASSERT(funcDecl);

      /* don't transform header file code  */
      if (strstr(funcDecl->get_name().str(), "operator"))
         continue ;

#ifdef ET_DEBUG
      printf("--- %s ---\n", funcDecl->get_name().str()) ;
#endif

      int loopCount = 0 ;  /* used to create local variable names */
      int segCount = 0 ;

      TransformFunction(funcDecl, funcBody, funcBody, &loopCount, &segCount) ;

      if (loopCount != 0)
      {
         loopTransformApplied = true ;
      }
   }

   /* files containing at least one loop require run-time support */

   if (loopTransformApplied)
   {
      SageInterface::attachArbitraryText(globalScope,
         std::string("#include \"ETrt.h\"\n#include \"cycle.h\"\n")) ;
   }

   /* fold run-time support code into file containing main() */

   SgFunctionDeclaration *mainFunc = SageInterface::findMain(project) ;
   if (mainFunc != NULL)
   {

      SgFunctionDefinition *mainFuncDef = mainFunc->get_definition() ;

      /* include ETrt.c just before main() in this file */
      SageInterface::attachArbitraryText(globalScope,
         std::string("#include \"ETrt.c\"\n")) ;

      /* insert finalization code at end of main() */
      Rose_STL_Container<SgNode*> retStmts =
          NodeQuery::querySubTree(mainFunc, V_SgReturnStmt) ;

      if (retStmts.size() > 0)
      {
         for (Rose_STL_Container<SgNode*>::iterator r_itr = retStmts.begin();
                 r_itr != retStmts.end(); ++r_itr)
         {
            SgReturnStmt *ret = isSgReturnStmt(*r_itr) ;
            ROSE_ASSERT(ret);

            SgExprListExp *sanityArgs = new SgExprListExp(ANCHOR) ;
            SgExprStatement *sanityCall = buildFunctionCallStmt(
               SgName("ET_SanityCheck"), new SgTypeVoid(), sanityArgs,
               mainFuncDef->get_body()) ;
            ret->get_scope()->insert_statement(ret, sanityCall) ;

            SgExprListExp *logStatArgs = new SgExprListExp(ANCHOR) ;
            SgExprStatement *logStatCall = buildFunctionCallStmt(
               SgName(dumpFunc), new SgTypeVoid(), logStatArgs,
               mainFuncDef->get_body()) ;
            ret->get_scope()->insert_statement(ret, logStatCall) ;
         }
      }
      else
      {
         SgExprListExp *sanityArgs = new SgExprListExp(ANCHOR) ;
         SgExprStatement *sanityCall = buildFunctionCallStmt(
            SgName("ET_SanityCheck"), new SgTypeVoid(), sanityArgs,
            mainFuncDef->get_body()) ;
         mainFuncDef->get_body()->append_statement(sanityCall) ;

         SgExprListExp *logStatArgs = new SgExprListExp(ANCHOR) ;
         SgExprStatement *logStatCall = buildFunctionCallStmt(
            SgName(dumpFunc), new SgTypeVoid(), logStatArgs,
            mainFuncDef->get_body()) ;
         mainFuncDef->get_body()->append_statement(logStatCall) ;
      }
   }

   /* make sure AST is well formed */
   AstTests::runAllTests(project);

   return backend(project);
}

