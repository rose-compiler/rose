
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

using namespace std ;
using namespace SageBuilder ;

static const char *dumpFunc ;
static SgType *ETtype ;
static bool emitSeqSeg        = true ;
static bool countIters        = true ;
static bool instrumentWhile   = true ;
static bool instrumentDoWhile = true ;
static bool fullLoopStat      = true ;

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

   SgFunctionCallExp *registerCall = buildFunctionCallExp(
      SgName(registerFuncName),
      varType,
      buildExprListExp(
         buildStringVal(fileInfo->get_filenameString()),
         buildStringVal(funcDecl->get_name().str()),
         buildIntVal(fileInfo->get_line())
      ),
      funcBody
   ) ;
          
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
   SgExprListExp *args = buildExprListExp(expr) ;

   SgExprStatement *statement = buildFunctionCallStmt(
      SgName(stopSeqFuncName), buildVoidType(), args, funcBody) ;
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
      SgTypedefSymbol* typedefSymbol ;

      /* Note that typeDecl is not added to AST */
      typeDecl =
         buildTypedefDeclaration(name, surrogateType, globalScope) ;
      ROSE_ASSERT(typeDecl) ;
      typeDecl->get_file_info()->unsetOutputInCodeGeneration() ;
   }
   else
   {
     SgTypedefSymbol *ts = globalScope->lookup_typedef_symbol(SgName(name)) ;
     ROSE_ASSERT(ts) ;
     typeDecl = ts->get_declaration() ;
     ROSE_ASSERT(typeDecl) ;
   }
   type = isSgType(typeDecl->get_type()) ;
   ROSE_ASSERT(type) ;

   return type ;
}

/************************************************************/
/* Determine if a statement contains a call to an unknown   */
/* function, or a function known to have non-sequential     */
/* behavior.                                                */
/************************************************************/

static bool ContainsNonSimpleCall(SgStatement *stmt)
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
         printf("File SEGDB.txt is absent. Sequential segment results degraded.\n") ;
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

static int LoopDepth(SgNode *baseScope, SgNode *stmt )
{
   int level = 0 ;
   for (SgNode *scan = stmt; scan != baseScope; scan = scan->get_parent())
   {
      if (isSgForStatement(scan) ||
          (instrumentWhile && isSgWhileStmt(scan)) ||
          (instrumentDoWhile && isSgDoWhileStmt(scan)) )
      {
        if (!countIters ||
            SageInterface::isCanonicalForLoop(scan, NULL, NULL, NULL, NULL) )
        {
          ++level ;
        }
      }
   }

   return level ;
}

/************************************************************/
/*       Add a caliper around a sequential code segment.    */
/************************************************************/

static void CreateSegmentProbe(SgFunctionDeclaration *funcDecl, SgBasicBlock *funcBody,
                              SgStatement *begin, SgStatement *end, int *segmentCount,
                              bool finalSegmentInScope = false)
{
   if (emitSeqSeg == false)
      return ;

   SgVariableDeclaration *stopSegDecl =
      ET_BuildDecl(funcBody, "ET_stopSeg", *segmentCount, ETtype, true) ;
   SgVariableDeclaration *startSegDecl =
      ET_BuildDecl(funcBody, "ET_startSeg", *segmentCount, ETtype, true) ;

   /* Start segment code fragment */
   RegisterItem(funcDecl, funcBody, begin,
                "ET_RegisterStartSeg", ETtype, startSegDecl, true) ;
   HandleItem(funcBody, begin, "ET_StartSeg", buildVarRefExp(startSegDecl), true) ;

   /* Stop segment code fragment */
   if (finalSegmentInScope) {
      HandleItem(funcBody, end, "ET_StopSeg", buildVarRefExp(stopSegDecl), false) ;
      RegisterItem(funcDecl, funcBody, end,
                   "ET_RegisterStopSeg", ETtype, stopSegDecl, false) ;
   }
   else {
      RegisterItem(funcDecl, funcBody, end,
                   "ET_RegisterStopSeg", ETtype, stopSegDecl, true) ;
      HandleItem(funcBody, end, "ET_StopSeg", buildVarRefExp(stopSegDecl), true) ;
   }

   ++(*segmentCount) ;
}

/*************************************************/
/*       Add a caliper around a loop segment.    */
/*************************************************/

static void CreateLoopProbe(SgFunctionDeclaration *funcDecl, SgBasicBlock *funcBody,
                            SgStatement *stmt, int *loopCount)
{
   SgExpression *lb ;
   SgExpression *ub ;
   SgExpression *step ;
   bool isCanonicalFor = SageInterface::isCanonicalForLoop(stmt, NULL, &lb, &ub, &step) ;

   /* generate loop segment, and iteration information if requested */
   if (countIters && !isCanonicalFor) {
      Sg_File_Info *fileInfo = stmt->get_startOfConstruct() ;
      printf("Non-canonical loop in %s:::%s at line %d\n",
             fileInfo->get_filenameString().c_str(),
             funcDecl->get_name().str(), fileInfo->get_line()) ;
   }
   else
   {
      SgExpression *its = NULL ;
      if (countIters && isCanonicalFor) {
         bool isDecreasing = false ;
         bool adjustBound = false ;

         SgExpression *high ;
         SgExpression *max ;
         SgExpression *minuend ;
         SgExpression *subtrahend ;

         SgExpression *tmp ;
         SgForStatement* fs = isSgForStatement(stmt) ;
         ROSE_ASSERT(fs) ;
         SgBinaryOp* test = isSgBinaryOp(fs->get_test_expr());
         ROSE_ASSERT(test) ;
         switch (test->variantT())
         {
           case V_SgLessOrEqualOp:
             adjustBound = true ;
           case V_SgLessThanOp:
             break;
           case V_SgGreaterOrEqualOp:
             adjustBound = true ;
           case V_SgGreaterThanOp:
//           tmp = lb ;
//           lb = ub ;
//           ub = tmp ;
             isDecreasing = true ;
             break;
//         case V_SgNotEqualOp: // Do we really want to allow this != operator ?
//           break;
           default:
             /* do nothing */ ;
         }

         if (adjustBound)
         {
            high = buildAddOp(SageInterface::copyExpression(ub),
                              buildIntVal(isDecreasing ? -1 : 1)) ;
         }
         else
         {
            high = SageInterface::copyExpression(ub) ;
         }

         /* if step != const 1, we need to adjust high */
         if (!(isSgIntVal(step) && (isSgIntVal(step)->get_value() == 1))) {
            max = buildAddOp(high,
                             buildSubtractOp(SageInterface::copyExpression(step), buildIntVal(1))) ;
         }
         else {
            max = high ;
         }

         minuend    = max ;
         subtrahend = SageInterface::copyExpression(lb) ;
#if 0
         if (isDecreasing)
         {
            SgExpression *tmp ;
            tmp = minuend ;
            minuend = subtrahend ;
            subtrahend = tmp ;
         }
#endif

         /* The control logic is purely to clean up the output, since */
         /* the compiler can do a fine job of folding integer constansts */

         its = buildDivideOp(buildSubtractOp(minuend, subtrahend),
                             SageInterface::copyExpression(step)) ;
#if 0
         if (!(isSgIntVal(step) && (isSgIntVal(step)->get_value() == 1))) {
            its = buildDivideOp(buildSubtractOp(minuend, subtrahend),
                                SageInterface::copyExpression(step)) ;
         }
         else {
            if (!(isSgIntVal(subtrahend) &&
                  (isSgIntVal(subtrahend)->get_value() == 0))) {
               its = buildSubtractOp(minuend, subtrahend) ;
            }
            else {
               its = minuend ;
            }
         }
#endif
       }

       /* start loop code fragment */
       SgVariableDeclaration *loopDecl =
         ET_BuildDecl(funcBody, "ET_loop", *loopCount, ETtype, true) ;

       RegisterItem(funcDecl, funcBody, stmt,
                    "ET_RegisterLoop", ETtype, loopDecl, true) ;
       HandleItem(funcBody, stmt, "ET_PushLoopSeqId", buildVarRefExp(loopDecl), true) ;

       /* end loop code fragment */
       HandleItem(funcBody, stmt, "ET_PopLoopSeqId", its, false) ;

       ++(*loopCount) ;
   }
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

bool TransformFunction(SgFunctionDeclaration *funcDecl, SgBasicBlock *funcBody,
                       SgBasicBlock *scanScope, int *loopCount, int *segmentCount)
{
   /* Wherever TransformFunction is called recursively below, the return */
   /* value should most likely be propagated to the local simpleWork var */

   bool simpleWork = true ; /* entire block only contains sequential work */
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
         SgBasicBlock *loopBody = SageInterface::ensureBasicBlockAsBodyOfFor(forStatement) ;
         ROSE_ASSERT(loopBody) ;
         if (segBegin != NULL)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
           segBegin = NULL ;
         }
         CreateLoopProbe(funcDecl, funcBody, stmt, loopCount) ;
         simpleWork = false ;
         TransformFunction(funcDecl, funcBody, loopBody, loopCount, segmentCount) ; 
      }
      else if (isSgWhileStmt(stmt)) /* could be embedded inside a statement */
      {
         /* At minimum, identifiers in code should be specialized for while */
         SgWhileStmt *whileStatement = isSgWhileStmt(stmt) ;
         SgBasicBlock *loopBody = SageInterface::ensureBasicBlockAsBodyOfWhile(whileStatement) ;
         ROSE_ASSERT(loopBody) ;
         if (instrumentWhile) {
           if (segBegin != NULL)
           {
             CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
             segBegin = NULL ;
           }
           CreateLoopProbe(funcDecl, funcBody, stmt, loopCount) ;
           simpleWork = false ;
         }
         TransformFunction(funcDecl, funcBody, loopBody, loopCount, segmentCount) ; 
      }
      else if (isSgDoWhileStmt(stmt)) /* could be embedded inside a statement */
      {
         /* At minimum, identifiers in code should be specialized for while */
         SgDoWhileStmt *doWhileStatement = isSgDoWhileStmt(stmt) ;
         SgBasicBlock *loopBody = SageInterface::ensureBasicBlockAsBodyOfDoWhile(doWhileStatement) ;
         ROSE_ASSERT(loopBody) ;
         if (instrumentDoWhile) {
           if (segBegin != NULL)
           {
             CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
             segBegin = NULL ;
           }
           CreateLoopProbe(funcDecl, funcBody, stmt, loopCount) ;
           simpleWork = false ;
         }
         TransformFunction(funcDecl, funcBody, loopBody, loopCount, segmentCount) ; 
      }
      else if (isSgReturnStmt(stmt)) /* could be embedded inside a statement */
      {
         /* Note -- we should probably put the return statement in block scope */
         /* before adding the extra code */

         /* We do not currently count iterations for loops that are aborted */

         int level = LoopDepth(funcBody, stmt) ;
         if ((segBegin != NULL) && !simpleWork)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
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

         /* We do not currently count iterations for loops that are aborted */

         int gotoLevel = LoopDepth(funcBody, stmt) ;
         int labelLevel = LoopDepth(funcBody, isSgGotoStatement(stmt)->get_label()) ;
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
         /* we are currently not making an iteration adjustment for */
         /* break or continue statements.  See comments below */
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
      else if (isSgIfStmt(stmt))
      {
         bool simpleWorkTrueBody  = true ;
         bool simpleWorkFalseBody = true ;
         SgIfStmt *ifStatement = isSgIfStmt(stmt) ;

         if (ifStatement->get_true_body() != NULL) {
           SgBasicBlock *trueBody = SageInterface::ensureBasicBlockAsTrueBodyOfIf(ifStatement) ;
           ROSE_ASSERT(trueBody) ;
           simpleWorkTrueBody =
              TransformFunction(funcDecl, funcBody, trueBody, loopCount, segmentCount) ; 
         }
         if (ifStatement->get_false_body() != NULL) {
           SgBasicBlock *falseBody = SageInterface::ensureBasicBlockAsFalseBodyOfIf(ifStatement) ;
           ROSE_ASSERT(falseBody) ;
           simpleWorkFalseBody =
              TransformFunction(funcDecl, funcBody, falseBody, loopCount, segmentCount) ; 
         }
         if (!(simpleWorkTrueBody && simpleWorkFalseBody)) {
           if (segBegin != NULL)
           {
             CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
             segBegin = NULL ;
           }
           simpleWork = false ;
         }
         else {
           if (segBegin == NULL)
           {
#ifndef DISABLE_SEQUENTIAL_SEGMENTS
              segBegin = stmt ;
#endif
           }
         }
      }
      else if (isSgSwitchStatement(stmt))
      {
         bool simpleWorkSwitch ;
         SgSwitchStatement *switchStatement = isSgSwitchStatement(stmt) ;
         // SgBasicBlock *body = isSgBasicBlock(switchStatement->get_body()) ;
         SgBasicBlock *body = SageInterface::ensureBasicBlockAsBodyOfSwitch(switchStatement) ;
         ROSE_ASSERT(body) ;
         simpleWorkSwitch = TransformFunction(funcDecl, funcBody, body, loopCount, segmentCount) ;
         if (!simpleWorkSwitch) {
           if (segBegin != NULL)
           {
             CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
             segBegin = NULL ;
           }
           simpleWork = false ;
         }
         else {
           if (segBegin == NULL)
           {
#ifndef DISABLE_SEQUENTIAL_SEGMENTS
              segBegin = stmt ;
#endif
           }
         }
      }
      else if (isSgCaseOptionStmt(stmt))
      {
         SgCaseOptionStmt *caseStmt = isSgCaseOptionStmt(stmt) ;
         if (segBegin != NULL)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
           simpleWork = false ;
           segBegin = NULL ;
         }
         SgBasicBlock *body = isSgBasicBlock(caseStmt->get_body()) ;
         TransformFunction(funcDecl, funcBody, body, loopCount, segmentCount) ;
      }
      else if (isSgDefaultOptionStmt(stmt))
      {
         SgDefaultOptionStmt *defaultStmt = isSgDefaultOptionStmt(stmt) ;
         if (segBegin != NULL)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
           simpleWork = false ;
           segBegin = NULL ;
         }
         SgBasicBlock *body = isSgBasicBlock(defaultStmt->get_body()) ;
         TransformFunction(funcDecl, funcBody, body, loopCount, segmentCount) ;
      }
      else if (isSgBasicBlock(stmt)) {
         bool simpleWorkBlock ;
         SgBasicBlock *block = isSgBasicBlock(stmt) ;
         simpleWorkBlock = TransformFunction(funcDecl, funcBody, block, loopCount, segmentCount) ;
         if (!simpleWorkBlock) {
           if (segBegin != NULL)
           {
             CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
             segBegin = NULL ;
           }
           simpleWork = false ;
         }
         else {
           if (segBegin == NULL)
           {
#ifndef DISABLE_SEQUENTIAL_SEGMENTS
              segBegin = stmt ;
#endif
           }
         }
      }
      else if (ContainsNonSimpleCall(stmt))
      {
         if (segBegin != NULL)
         {
           CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount) ;
           segBegin = NULL ;
         }
         simpleWork = false ;
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
      CreateSegmentProbe(funcDecl, funcBody, segBegin, stmt, segmentCount, true) ;
   }

   return simpleWork ;
}


/******************************************************/
/*                Driver Routine                      */
/******************************************************/

int main (int argc, char *argv[])
{
   /* indicate whether include files need to be added */
   bool loopTransformApplied = false ;

   /* more bools at top of file... */
   bool withPAPI = false ;
   bool showStats = false ;
   bool enablePostProcessing = false ;

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
   if ( CommandlineProcessing::isOption(
           cmdLineArgs, "-et:", "(p|papi)", true) )
   {
     withPAPI = true ;
   }
   if ( CommandlineProcessing::isOption(
           cmdLineArgs, "-et:", "(l|loops)", true) )
   {
     emitSeqSeg = false ;
   }
   if ( CommandlineProcessing::isOption(
           cmdLineArgs, "-et:", "noiter", true) )
   {
     countIters = false ;
   }
   if ( CommandlineProcessing::isOption(
           cmdLineArgs, "-et:", "fast", true) )
   {
     fullLoopStat = false ;
     emitSeqSeg   = false ;
     countIters   = false ;
     withPAPI     = false ;
     enablePostProcessing = true ;
   }

   dumpFunc = (showStats ? "ET_LogStats" : "ET_Dump") ;

   /***********************************************/
   /*               Invoke ROSE                   */
   /***********************************************/

   /* build AST */
   SgProject* project = frontend(argc, argv);
   ROSE_ASSERT(project);

   if (project->get_fileList().empty() == false) {

     /* make sure AST is well formed */
     AstTests::runAllTests(project);

     /* set up some needed typedefs for runtime support */

     SgGlobal *globalScope = SageInterface::getFirstGlobalScope(project) ;
     ETtype = buildTypedefDeclaration("ET_Idx_t", buildShortType(), globalScope)->get_type() ;

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

     SgFunctionDeclaration *mainFunc = SageInterface::findMain(project) ;
     if (countIters == false && (loopTransformApplied || mainFunc != NULL)) {
        SageInterface::attachArbitraryText(globalScope,
           std::string("#define ET_NO_COUNT_ITERS 1\n")) ;
     }

     /* files containing at least one loop require run-time support */

     if (loopTransformApplied)
     {
        SageInterface::attachArbitraryText(globalScope,
           std::string("#include \"ETrt.h\"\n")) ;
     }

     /* fold run-time support code into file containing main() */

     if (mainFunc != NULL)
     {

        SgFunctionDefinition *mainFuncDef = mainFunc->get_definition() ;

        /* include ETrt.c just before main() in this file */
        if (!fullLoopStat) {
          SageInterface::attachArbitraryText(globalScope,
             std::string("#define ET_SIMPLE_LOOP_STATS 1\n") );
        }
        if (enablePostProcessing) {
          SageInterface::attachArbitraryText(globalScope,
             std::string("#define ET_POST_PROCESS_SEQ_TO_LOOP 1\n") );
        }
        if (withPAPI) {
          SageInterface::attachArbitraryText(globalScope,
             std::string("#define ET_PAPI 1\n\n") );
        }
        SageInterface::attachArbitraryText(globalScope,
           std::string("#include \"ETrt.c\"\n") );

        if (withPAPI) {
           /* Insert PAPI initialization code at top of main */
           SgBasicBlock *mainBody = mainFuncDef->get_body() ;

           Rose_STL_Container<SgNode*> blockStmts =
               NodeQuery::querySubTree(mainBody, V_SgStatement,
                                       AstQueryNamespace::ChildrenOnly) ;

           for (Rose_STL_Container<SgNode*>::iterator s_itr = blockStmts.begin();
                   s_itr != blockStmts.end(); ++s_itr)
           {
              SgStatement *stmt = isSgStatement(*s_itr) ;
              ROSE_ASSERT(stmt);

              /* skip variable declarations */
              if (isSgDeclarationStatement(stmt))
                 continue ;

              SgExprStatement *initCall = buildFunctionCallStmt(
                 SgName("ET_Init"), buildVoidType(), buildExprListExp(),
                 mainFuncDef->get_body()) ;
              stmt->get_scope()->insert_statement(stmt, initCall) ;

              break ;
           }
        }

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

              SgExprStatement *sanityCall = buildFunctionCallStmt(
                 SgName("ET_SanityCheck"), buildVoidType(), buildExprListExp(),
                 mainFuncDef->get_body()) ;
              ret->get_scope()->insert_statement(ret, sanityCall) ;

              SgExprStatement *logStatCall = buildFunctionCallStmt(
                 SgName(dumpFunc), buildVoidType(), buildExprListExp(),
                 mainFuncDef->get_body()) ;
              ret->get_scope()->insert_statement(ret, logStatCall) ;
           }
        }
        else
        {
           SgExprStatement *sanityCall = buildFunctionCallStmt(
              SgName("ET_SanityCheck"), buildVoidType(), buildExprListExp(),
              mainFuncDef->get_body()) ;
           mainFuncDef->get_body()->append_statement(sanityCall) ;

           SgExprStatement *logStatCall = buildFunctionCallStmt(
              SgName(dumpFunc), buildVoidType(), buildExprListExp(),
              mainFuncDef->get_body()) ;
           mainFuncDef->get_body()->append_statement(logStatCall) ;
        }
     }
   }

   /* make sure AST is well formed */
   AstTests::runAllTests(project);

   // generateDOT (*project);

   return backend(project);
}

