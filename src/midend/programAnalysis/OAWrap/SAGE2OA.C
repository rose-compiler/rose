// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 17,4 MB to 7,1MB
#include "sage3basic.h"
#include "SAGE2OA.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

namespace OpenAnalysis {
//########################################################
// Iterators
//########################################################

void SageIRStmtIterator::Reset()
{
  st_iter = st_list.begin();
}

SageIRStmtIterator::  SageIRStmtIterator (SgStatementPtrList & lst) 
   : st_list(lst)
 {
   st_iter = st_list.begin();
 }

StmtHandle SageIRStmtIterator::Current () 
 { 
   cerr << "this = " << this << endl;
   SgStatement* cur = *st_iter;
   cerr << "cur stmt: " << cur->unparseToString() << endl;
   StmtHandle h = (StmtHandle)cur; 
   cerr << "handle : " << h << endl;
   return h; 
 }
bool SageIRStmtIterator::IsValid () 
 { 
   cerr << "IsValid() " << "this = " << this << endl;
    return (st_iter!=st_list.end()); 
 }

void SageIRStmtIterator::operator ++ ()
 {
  cerr << "advance() " << "this = " << this << endl;
  st_iter ++;
 }



//-----------------------------------------------------------------------------
// Callsite actual parameter iterator
//-----------------------------------------------------------------------------
SageIRCallsiteParamIterator::SageIRCallsiteParamIterator(SgExpressionPtrList& lst) 
{
  begin = lst.begin();
  arg_iter = lst.begin();
  end = lst.end();
}

// Returns the current item.
ExprHandle SageIRCallsiteParamIterator::Current()
{
  return (ExprHandle)(*arg_iter);
}

// False when all items are exhausted. 
bool SageIRCallsiteParamIterator::IsValid ()  
{
  return (arg_iter != end);;
}
void SageIRCallsiteParamIterator::operator++()
{
  arg_iter++;
}

void SageIRCallsiteParamIterator::Reset()
{
  arg_iter = begin;
}


//########################################################
// Procedures and call sites
//########################################################

// Given a procedure, return its IRProcType.
IRProcType SageIRInterface::GetProcType(ProcHandle h) 
{
  // TODO : fix this -- I really don't know what some of the types (in OA) mean
  if (SgFunctionCallExp *fncall = isSgFunctionCallExp((SgNode*)h)) {

 // DQ(1/16/2006): Removed redundent p_expression_type data member (type now computed, most often from operands)
 // SgType *returnType = fncall->get_expression_type();
    SgType *returnType = fncall->get_type();
    switch (returnType->variantT())
      {
      case V_SgTypeVoid:
        return ProcType_SUB;
      default:
        return ProcType_FUNC;
      }
  } else {
    printf("error in SageIRInterface::GetProcType\n");
    return ProcType_ILLEGAL;
  }
}

// Given a ProcHandle, return an IRStmtIterator* for the
// procedure. The user must free the iterator's memory via delete.
IRStmtIterator * SageIRInterface::ProcBody(ProcHandle h) 
{
        SgStatementPtrList stptr;
        SgStatement * sptr=(SgStatement*)h;
        if(SgFunctionDefinition * scope=isSgFunctionDefinition(sptr)) {
          stptr=scope->getStatementList();
        } else {
          printf("error in SageIRInterface::ProcBody\n");
          return NULL;
        }
        return new SageIRStmtIterator(stptr);
}

// Get IRCallsiteIterator* for a statement. The user must free the
// iterator's memory via delete.
IRCallsiteIterator * SageIRInterface::GetCallsites(StmtHandle h) 
{
  //for now
  return NULL;
}

// Get IRCallsiteIterator* for a statement. The user must free the
// iterator's memory via delete.
IRCallsiteParamIterator * SageIRInterface::GetCallsiteParams(ExprHandle h) 
{
  //for now
  return NULL;
}

bool SageIRInterface::IsParamProcRef(ExprHandle h) 
{
  //for now
  return false;
}

// Given an expression representing a callsite, is this an
// invocation through a procedure parameter.
bool SageIRInterface::IsCallThruProcParam(ExprHandle h) {
  //for now
  return false;
}


//########################################################
// Statements: General
//########################################################


SageIRInterface:: ~SageIRInterface () {}
        
// Translate a Sage statement type into a IRStmtType.
IRStmtType SageIRInterface::GetStmtType(StmtHandle h)
{
        IRStmtType ty;
        switch(((SgStatement*)h)->variantT())
        {
                case V_SgBasicBlock:
                        ty=COMPOUND;
                        break;
                case V_SgDoWhileStmt:
                        ty=END_TESTED_LOOP;
                        break;
                case V_SgForStatement:
                        ty=LOOP;
                        break;
                case V_SgIfStmt:
                        ty=STRUCT_TWOWAY_CONDITIONAL;
                        break;
                case V_SgSwitchStatement:
                        ty=STRUCT_MULTIWAY_CONDITIONAL;
                        break;
                case V_SgWhileStmt:
                        ty=LOOP;
                        break;
                case V_SgContinueStmt:
                        ty=LOOP_CONTINUE;
                        break;
                case V_SgBreakStmt:
                        ty=BREAK;
                        break;
                case V_SgReturnStmt:
                        ty=RETURN;
                        break;
                case V_SgGotoStatement:
                        ty=UNCONDITIONAL_JUMP; //BW or UNCONDITIONAL_JUMP_I not sure which...
                        break;
                default:
                        ty=SIMPLE;
        }
        return ty;
}

// Given a statement, return the label associated with it (or NULL if none).
StmtLabel SageIRInterface::GetLabel(StmtHandle h)
{
        //for now
        return 0;
}

// Given a compound statement, return an IRStmtIterator* for the statements.
IRStmtIterator* SageIRInterface::GetFirstInCompound(StmtHandle h)
{
        SgStatementPtrList stptr;
        SgStatement * sptr=(SgStatement*)h;
        if(SgScopeStatement * scope=isSgScopeStatement(sptr))
        {
                stptr=scope->getStatementList();
        }
        else
        {
                printf("error in SageIRInterface::GetFirstInCompound\n");
        }
        return new SageIRStmtIterator(stptr);
}

IRStmtIterator* SageIRInterface::Body(StmtHandle h)  //for now same as GetFirstInCompound
{
        SgStatement * sptr=(SgStatement*)h;
        if(SgScopeStatement * scope=isSgScopeStatement(sptr))
        {
                SgStatementPtrList  & stptr=scope->getStatementList();
                IRStmtIterator* retit=new SageIRStmtIterator(stptr);
                return retit;
        }
        else
        {
                printf("error in SageIRInterface::GetFirstInCompound\n");
                return NULL;
        }
}

IRStmtIterator* SageIRInterface::LoopBody(StmtHandle h)  //for now same as GetFirstInCompound
{
        SgStatement * sptr=(SgStatement*)h;
        if(SgScopeStatement * scope=isSgScopeStatement(sptr))
        {
                SgStatementPtrList  & stptr=scope->getStatementList();
                IRStmtIterator* retit=new SageIRStmtIterator(stptr);
                return retit;
        }
        else
        {
                printf("error in SageIRInterface::LoopBody\n");
                return NULL;
        }
}

StmtHandle SageIRInterface::LoopHeader(StmtHandle h)
{
        SgStatement * st=NULL;
        SgForStatement * forst=isSgForStatement((SgStatement *)h);
        if(forst)
        {
                st=forst->get_init_stmt().front();
        }
        
        return (StmtHandle)st;
}

StmtHandle SageIRInterface::GetLoopIncrement(StmtHandle h)
{
        SgStatement * st=NULL;
        SgForStatement * forst=isSgForStatement((SgStatement *)h);
        if(forst)
        {
                Sg_File_Info * finfo=new Sg_File_Info("foobar", 1,1);
                SgExpression * exp=forst->get_increment();
                st=new SgExprStatement(finfo, exp);
        }
        
        return (StmtHandle)st;
}

//bool
//Pro64IRInterface::LoopIterationsDefinedAtEntry (StmtHandle h)
//{
//  WN *wn = (WN *) h;

  // In Whirl, only an OPR_DO_LOOP is specified to have Fortran semantics,
  // which means the increment is a loop-invariant expression.  However,
  // Pro64 already enforces the restrictions, so we don't have to do
  // anything special here (i.e., always return false).
//  return false;
//}

bool SageIRInterface::LoopIterationsDefinedAtEntry(StmtHandle h)
{
        //this needs to be false for C (read comments in CFG.C)
        return false;
}

StmtLabel SageIRInterface::GetTargetLabel(StmtHandle h, int) 
{
        //for now
        return 0;
}

//-----------------------------------------------------------------------------
// Unstructured multiway conditionals
//-----------------------------------------------------------------------------

// Given an unstructured multi-way branch, return the number of targets.
// The count does not include the optional default/catchall target.
int SageIRInterface::NumUMultiTargets(StmtHandle h) 
{
        //for now
        return 0;
}

// Given an unstructured multi-way branch, return the label of the target
// statement at 'targetIndex'. The n targets are indexed [0..n-1]. 
StmtLabel SageIRInterface::GetUMultiTargetLabel(StmtHandle h, int)
{
        //for now
        return 0;
}

// Given an unstructured multi-way branch, return the label of the optional
// default/catchall target. Return 0 if no default target.
StmtLabel SageIRInterface::GetUMultiCatchallLabel(StmtHandle h)
{
        //for now
        return 0;
}


//for switch stmt?
int SageIRInterface::NumMultiCases(StmtHandle h)
{
        //for now
        return 0;
}

// I'm assuming bodyIndex is 0..n-1. 
IRStmtIterator* SageIRInterface::MultiBody(StmtHandle h, int bodyIndex)
{
        //for now
        return NULL;
}

bool SageIRInterface::IsBreakImplied(StmtHandle h)
{
        return false; //break is never implied in C/C++ ?!?!
}

//default?
IRStmtIterator* SageIRInterface::GetMultiCatchall(StmtHandle h)
{
        return new SageIRStmtIterator();
}

  //------------------------------
  // Structured two-way conditionals
  //
  // Note: An important pre-condition for structured conditionals is that chains
  // of else-ifs must be represented as nested elses.  For example, this Matlab
  // statement:
  //   if (c1)
  //     s1;
  //   elseif (c2)
  //     s2;
  //   else
  //     s3;
  //   end;
  //
  // would need be represented by the underlying IR as:
  //   if (c1)
  //     s1;
  //   else
  //     if (c2)
  //       s2;
  //     else
  //       s3;
  //     end;
  //   end; 
  //------------------------------
  
  //BW Sage already converts else ifs into nested elses...

  // Given a structured two-way conditional statement, return an IRStmtIterator*
  // for the "true" part (i.e., the statements under the "if" clause).
  

IRStmtIterator* SageIRInterface::TrueBody(StmtHandle h)
   {
  // return iterator of the true branch if  h if an if stmt
  // for now
  // return NULL;
     printf("in TrueBody\n");
     if (SgIfStmt * ifs=isSgIfStmt((SgStatement*) h))
        {
          SgStatementPtrList stptr;
          SgStatement * sptr=ifs->get_true_body();
          SgScopeStatement * scope=isSgScopeStatement(sptr);
          fflush(stdout);
          if (scope)
               return new SageIRStmtIterator(scope->getStatementList());       
            else
               return new SageIRStmtIterator(stptr);
        }
    // DQ (11/27/2005): I assume that the function should return something if the conditional is false and that it is an error!
       else
        {
          printf ("Note: return not handled by author for else case, assumed error! \n");
          ROSE_ABORT();
        }
   }

IRStmtIterator* SageIRInterface::ElseBody(StmtHandle h)
   {
     printf("in ElseBody\n");
     if(SgIfStmt * ifs=isSgIfStmt((SgStatement*) h))
        {
          SgStatementPtrList stptr;
          SgStatement * sptr=ifs->get_false_body();
          SgScopeStatement * scope=isSgScopeStatement(sptr);
          fflush(stdout);
          if (scope)
               return new SageIRStmtIterator(scope->getStatementList());       
            else 
               return new SageIRStmtIterator(stptr);
        }
    // DQ (11/27/2005): I assume that the function should return something if the conditional is false and that it is an error!
       else
        {
          printf ("Note: return not handled by author for else case, assumed error! \n");
          ROSE_ABORT();
        }
   }

IRUseDefIterator* SageIRInterface::GetUses(StmtHandle h)
{
        //for now
        return NULL;
}

IRUseDefIterator* SageIRInterface::GetDefs(StmtHandle h)
{
        //for now
        return NULL;
}

// DQ (11/25/2005): Modified to reflect condition being a SgStatment now
// This is required for C and C++ (see test2005_114.C)
// ExprHandle SageIRInterface::GetLoopCondition(StmtHandle h)
StmtHandle SageIRInterface::GetLoopCondition(StmtHandle h)
   {
     SgForStatement * forst=isSgForStatement((SgStatement *) h);
     SgStatement * stmt=NULL;
     if (forst)
        {
       // stmt = forst->get_test_expr();
          stmt = forst->get_test();
        }
     return (StmtHandle)stmt;
   }

#if 0
// DQ (11/25/2005): Modified to reflect condition being a SgStatment now
// This function now has to be a bit more complex since some loops can 
// have SgExpressions as loop tests and others now have to have SgStatements
// as loop tests.  I have removed this function until the subject can
// be discussed.  It is not clear if it is a limitation of OpenAnalysis
// or if it is just a limitation of the interface implementtion.

//-----------------------------------------------------------------------------
// Structured conditionals
//
// FIXME: Is GetCondition for unstructured conditionals also?  It is currently
// implemented that way.
//-----------------------------------------------------------------------------
ExprHandle SageIRInterface::GetCondition(StmtHandle h)
   {
  // DQ (11/27/2005): What about the condition in the switch (it is also a SgStatement)
  // This function is fundamentally flawed in that it assumes the condition is an expression,
  // this is true only for the SgDoWhileStmt.

#if 0
     SgExpression   *exp   = NULL;
     SgForStatement *forst = NULL;
     SgWhileStmt    *whst  = NULL;
     SgDoWhileStmt  *dost  = NULL;
     SgIfStmt       *ifst  = NULL;

     if(forst=isSgForStatement((SgStatement *) h))
        {
          exp=forst->get_test_expr();
        }
       else
          if(whst=isSgWhileStmt((SgStatement *) h))
             {
               SgStatement * condst=whst->get_condition();
               SgExprStatement * expst=isSgExprStatement(condst);
               if(condst)
                    exp=expst->get_the_expr();
             }
            else 
               if(dost=isSgDoWhileStmt((SgStatement *) h))
                  {
                    SgStatement * condst=dost->get_condition();
                    SgExprStatement * expst=isSgExprStatement(condst);
                    if(condst)
                        exp=expst->get_the_expr();
                  }
                 else
                    if(ifst=isSgIfStmt((SgStatement *) h))
                       {
                         SgStatement * condst=ifst->get_conditional();
                         SgExprStatement * expst=isSgExprStatement(condst);
                         if(condst)
                              exp=expst->get_the_expr();
                       }

     return (ExprHandle)exp;
#else

     *** Incomplete code implementation ***

     SgStatement* input = (SgStatement *) h;
     SgExpression *returnExpression = NULL;
     switch(input->variantT())
        {
          case V_SgForStatement:
             {
               SgForStatement* forStatement = isSgForStatement(input);
               returnExpression = forStatement->get_test_expr();
             }
          case V_SgWhileStmt:
             {
             }
          case V_SgDoWhileStmt:
             {
             }
          case V_SgIfStmt:
             {
             }
          default:
             {
               printf ("In SageIRInterface::GetCondition(SgStatement): default reached input = %p = %s \n",input,input->class_name().c_str());
             }
        }

     return returnExpression;
#endif
   }
#endif

// Given an unstructured multi-way branch, return the condition expression
// corresponding to target 'targetIndex'. The n targets are indexed [0..n-1].
ExprHandle SageIRInterface::GetUMultiCondition(StmtHandle h, int targetIndex)
{
  // FIXME: It isn't yet decided whether or not this function is needed in
  // the IR interface.
  ROSE_ABORT ();
}

ExprHandle SageIRInterface::GetSMultiCondition(StmtHandle h, int bodyIndex)
{
        //for now
        return 0;
        //in whirl switch stmts are unstructured, and in sage???
}

ExprHandle SageIRInterface::GetMultiExpr(StmtHandle h)
{
  // Whirl does not appear to have a structured switch statement. And what about Sage??
  ROSE_ABORT ();
}
};
