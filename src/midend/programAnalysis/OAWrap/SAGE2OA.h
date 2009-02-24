

#ifndef _SAGE2OA_H
#define _SAGE2OA_H



#include <OAWrap/OAConfig.h>

namespace OpenAnalysis {

#include <OpenAnalysis/Interface/IRInterface.h>
class SageIRStmtIterator: public IRStmtIterator {
public:
  SageIRStmtIterator (SgStatementPtrList & lst);
  SageIRStmtIterator() { st_iter = st_list.end(); }
  ~SageIRStmtIterator () { }

  StmtHandle Current (); 
  bool IsValid (); 
  void operator ++();

  void Reset();
private:
  SgStatementPtrList st_list;
  SgStatementPtrList::iterator st_iter;
};

class SageIRUseDefIterator: public IRUseDefIterator {
public:
  SageIRUseDefIterator (SgStatementPtrList *n, int uses_or_defs);
  SageIRUseDefIterator () { assert (0); }
  ~SageIRUseDefIterator () { }

  LeafHandle Current () { return 0; }
  bool IsValid () { return false; }
  void operator++ () { };
        
private:
};

class SageIRCallsiteParamIterator: public IRCallsiteParamIterator {
public:
  SageIRCallsiteParamIterator(SgExpressionPtrList & lst);
  SageIRCallsiteParamIterator() { assert(0); }
  ~SageIRCallsiteParamIterator() { }

  ExprHandle Current();   // Returns the current item.
  bool IsValid ();        // False when all items are exhausted. 
  void operator++();

  void Reset();
private:
  SgExpressionPtrList::iterator arg_iter;
  SgExpressionPtrList::iterator begin;
  SgExpressionPtrList::iterator end;
};

class SageIRInterface : public IRInterface {
public:
  SageIRInterface () { /*FIXME*/ }
  ~SageIRInterface (void) ;

  static SgStatement* StmtHandle2SgNode( StmtHandle h)
     { return (SgStatement*)h; }

  //########################################################
  // Procedures and call sites
  //########################################################

  // Given a procedure, return its IRProcType.
  IRProcType GetProcType(ProcHandle h) ;
  
  // Given a ProcHandle, return an IRStmtIterator* for the
  // procedure. The user must free the iterator's memory via delete.
  IRStmtIterator * ProcBody(ProcHandle h) ;
  
  // Get IRCallsiteIterator* for a statement. The user must free the
  // iterator's memory via delete.
  IRCallsiteIterator * GetCallsites(StmtHandle h) ; 

  // Get IRCallsiteIterator* for a statement. The user must free the
  // iterator's memory via delete.
  IRCallsiteParamIterator * GetCallsiteParams(ExprHandle h) ; 

  bool IsParamProcRef(ExprHandle h) ;

  // Given an expression representing a callsite, is this an
  // invocation through a procedure parameter.
  bool IsCallThruProcParam(ExprHandle h) ;

  //########################################################
  // Statements: General
  //########################################################

  IRStmtType GetStmtType (StmtHandle h);
  StmtLabel GetLabel (StmtHandle h);

  //------------------------------
  // for compound statement. 
  //------------------------------
  IRStmtIterator *GetFirstInCompound (StmtHandle h);

  //------------------------------
  // for procedure, loop
  //------------------------------
  IRStmtIterator *Body (StmtHandle h);

  //------------------------------
  // loops
  //------------------------------
  IRStmtIterator *LoopBody (StmtHandle h);
  StmtHandle LoopHeader (StmtHandle h);
  bool LoopIterationsDefinedAtEntry (StmtHandle h);

// DQ (11/25/2005): Modified to reflect condition being a SgStatment now
// This is required for C and C++ (see test2005_114.C)
// ExprHandle GetLoopCondition (StmtHandle h); 
  StmtHandle GetLoopCondition (StmtHandle h);

  StmtHandle GetLoopIncrement (StmtHandle h);

  //------------------------------
  // invariant: a two-way conditional or a multi-way conditional MUST provide
  // provided either a target, or a target label
  //------------------------------

  //------------------------------
  // unstructured two-way conditionals: 
  //------------------------------
  // two-way branch, loop continue
  StmtLabel  GetTargetLabel (StmtHandle h, int n);

  ExprHandle GetCondition (StmtHandle h);

  //------------------------------
  // unstructured multi-way conditionals
  //------------------------------
  int NumUMultiTargets (StmtHandle h);
  StmtLabel GetUMultiTargetLabel (StmtHandle h, int targetIndex);
  StmtLabel GetUMultiCatchallLabel (StmtHandle h);
  ExprHandle GetUMultiCondition (StmtHandle h, int targetIndex);

  //------------------------------
  // structured multiway conditionals
  //------------------------------
  int NumMultiCases (StmtHandle h);
  // condition for multi body 
  ExprHandle GetSMultiCondition (StmtHandle h, int bodyIndex);
  // multi-way beginning expression
  ExprHandle GetMultiExpr (StmtHandle h);
  IRStmtIterator *MultiBody (StmtHandle h, int bodyIndex);
  bool IsBreakImplied (StmtHandle multicond);
  IRStmtIterator *GetMultiCatchall (StmtHandle h);

  //------------------------------
  // structured conditionals
  //------------------------------
  IRStmtIterator *TrueBody (StmtHandle h);
  IRStmtIterator *ElseBody (StmtHandle h);

  //------------------------------
  // obtain uses and defs
  //------------------------------
  IRUseDefIterator *GetUses (StmtHandle h);
  IRUseDefIterator *GetDefs (StmtHandle h);

  SymHandle GetSymHandle (LeafHandle vh) {
   //WN *wn = (WN *) vh;
    //assert (OPERATOR_has_sym (WN_operator (wn)));
    //return (SymHandle) WN_st (wn); 
    return 0; // MS: SymHandle is not a pointer, does not compile with 2.96; NULL;
  }

  //for now
  SymHandle GetProcSymHandle(ProcHandle h) {return 0;};

  // Given a SymHandle, return the textual name.
  // DQ (1/13/2006): Andreas has pointed out that the return type should be "const char*" instead of "char*"
  const char *GetSymNameFromSymHandle (SymHandle sh) {
    //ST *st = (ST *) sh;
    //return (char *) ST_name (st); 
    return 0; // MS: SymHandle is not a pointer, does not compile with 2.96;NULL;
  }

  //void PrintLeaf (LeafHandle vh, ostream & os);

  //void Dump (StmtHandle stmt, ostream& os);

private:
  // Dump Whirl subtree in a compact, friendly format.
  //void dump_wn_subtree (WN *, ostream &);
};
};

#endif
