// $Id: IRInterface.h,v 1.2 2006/04/24 00:21:34 dquinlan Exp $
// -*-C++-*-
// * BeginRiceCopyright *****************************************************
// 
// Copyright ((c)) 2002, Rice University 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of Rice University (RICE) nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// This software is provided by RICE and contributors "as is" and any
// express or implied warranties, including, but not limited to, the
// implied warranties of merchantability and fitness for a particular
// purpose are disclaimed. In no event shall RICE or contributors be
// liable for any direct, indirect, incidental, special, exemplary, or
// consequential damages (including, but not limited to, procurement of
// substitute goods or services; loss of use, data, or profits; or
// business interruption) however caused and on any theory of liability,
// whether in contract, strict liability, or tort (including negligence
// or otherwise) arising in any way out of the use of this software, even
// if advised of the possibility of such damage. 
// 
// ******************************************************* EndRiceCopyright *

#ifndef IRInterface_h
#define IRInterface_h

//-----------------------------------------------------------------------------
// This file contains the abstract base classes for the IR interface.
//
// See the top level README for a description of the IRInterface and
// how to use it.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Below are generic types for handles to relate objects to a user's IR. 0 is
// reserved as a NULL value.  There are two sets of types: 
//  1) one whose type/size is relative to the current platform, and 
//  2) one whose type/size is maximum, which is useful for cross-platform tools
//
//   OA_IRHANDLETYPE_UL:  For handles on this platform
//   OA_IRHANDLETYPE_SZ64: Maximum size for cross-platform handles 
//-----------------------------------------------------------------------------

#include "inttypes.h"

#if defined(OA_IRHANDLETYPE_UL)

  typedef unsigned long openanal_base_type;

#elif defined(OA_IRHANDLETYPE_SZ64)

  typedef uint64_t openanal_base_type;

#else

# error OpenAnalysis handle type must be specified!

#endif

typedef openanal_base_type ProcHandle;
typedef openanal_base_type StmtHandle;
typedef openanal_base_type ExprHandle; 
typedef openanal_base_type LeafHandle; // An expr that happens to be a leaf.
typedef openanal_base_type StmtLabel;
typedef openanal_base_type SymHandle;


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

// Enumerate all the procedures in a certain IR
class IRProcIterator {
public:
  IRProcIterator() { }
  virtual ~IRProcIterator() { }
  
  virtual ProcHandle Current() = 0;   // Returns the current item.
  virtual bool IsValid () = 0;        // False when all items are exhausted. 
  virtual void operator++() = 0;
  void operator++(int) { ++*this; }

  virtual void Reset() = 0;
};

// Enumerate all the statements in a program region, e.g. all the statements
// in a procedure or a loop.
class IRStmtIterator {
public:
  IRStmtIterator() { }
  virtual ~IRStmtIterator() { };

  virtual StmtHandle Current() = 0;  // Returns the current item.
  virtual bool IsValid() = 0;        // False when all items are exhausted.
  virtual void operator++() = 0;
  void operator++(int) { operator++(); } ;

  virtual void Reset() = 0;
};

// Enumerate all the variable uses or variable definitions in a statement.
// This is useful for analyses that require information about variable
// references or definitions, such as SSA construction.
class IRUseDefIterator {
public:
  enum { Uses, Defs };

  IRUseDefIterator() { }
  virtual ~IRUseDefIterator() { }

  virtual LeafHandle Current() = 0;   // Returns the current item.
  virtual bool IsValid () = 0;        // False when all items are exhausted. 
  virtual void operator++() = 0;
  void operator++(int) { ++*this; }

  virtual void Reset() = 0;
};

// Enumerate all the procedure calls in a statement.
class IRCallsiteIterator {
public:
  IRCallsiteIterator() { }
  virtual ~IRCallsiteIterator() { }

  virtual ExprHandle Current() = 0;   // Returns the current item.
  virtual bool IsValid () = 0;        // False when all items are exhausted. 
  virtual void operator++() = 0;
  void operator++(int) { ++*this; }

  virtual void Reset() = 0;
};

// Enumerate all (actual) parameters within a callsite
class IRCallsiteParamIterator {
public:
  IRCallsiteParamIterator() { }
  virtual ~IRCallsiteParamIterator() { }

  virtual ExprHandle Current() = 0;   // Returns the current item.
  virtual bool IsValid () = 0;        // False when all items are exhausted. 
  virtual void operator++() = 0;
  void operator++(int) { ++*this; }

  virtual void Reset() = 0;
};



// Procedures are classified into one of the following types:
enum IRProcType 
{
  ProcType_PGM,
  ProcType_SUB,
  ProcType_FUNC,
  ProcType_BDATA,
  ProcType_ILLEGAL
};

// Statements are classified into one of the following types:
enum IRStmtType 
{
  SIMPLE,                       // Anything not covered below.
  COMPOUND,                     // A block of statements. 
  LOOP,                         // Any type of top-tested, structured loop.
  END_TESTED_LOOP,              // Any type of end-tested, structured loop.
  STRUCT_TWOWAY_CONDITIONAL,    // Structured if-then-else.
  STRUCT_MULTIWAY_CONDITIONAL,  // Structured switch statement.
  USTRUCT_TWOWAY_CONDITIONAL_T, // Unstructured branch (on true).
  USTRUCT_TWOWAY_CONDITIONAL_F, // Unstructured branch (on false).
  USTRUCT_MULTIWAY_CONDITIONAL, // Unstructured multiway branch
				//  (e.g., computed goto in Fortran or 
				//  jump tables in low-level/assembly
				//  languages).
  RETURN,                       // Return statement.
  BREAK,                        // Break statement.
  LOOP_CONTINUE,                // Loop continuation statement.
  ALTERNATE_PROC_ENTRY,         // Alternate entry point (e.g., Fortran)
  UNCONDITIONAL_JUMP,           // GOTO in HLL, or unconditional direct
				//   jump in low-level/assembly languages.
  UNCONDITIONAL_JUMP_I,         // Assigned GOTO in HLL, or unconditional
				//   indirect jump in low-level/assembly
				//   languages.
  NONE
};


class EdgeInfo;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

// The IRInterface abstract base class gives a set of methods for manipulating 
// a program.  This is the primary interface to the underlying intermediate
// representation.
class IRInterface {
 public:
  IRInterface() { }
  virtual ~IRInterface() { }

  //--------------------------------------------------------
  // Procedures and call sites
  //--------------------------------------------------------

  // Given a procedure, return its IRProcType.
  virtual IRProcType GetProcType(ProcHandle h) = 0;
  
  // Given a ProcHandle, return an IRStmtIterator* for the
  // procedure. The user must free the iterator's memory via delete.
  virtual IRStmtIterator *ProcBody(ProcHandle h) = 0;
  
  // Get IRCallsiteIterator* for a statement. The user must free the
  // iterator's memory via delete.
  virtual IRCallsiteIterator *GetCallsites(StmtHandle h) = 0; 

  // Get IRCallsiteIterator* for a statement. The user must free the
  // iterator's memory via delete.
  virtual IRCallsiteParamIterator *GetCallsiteParams(ExprHandle h) = 0; 

  virtual bool IsParamProcRef(ExprHandle h) = 0;

  // Given an expression representing a callsite, is this an
  // invocation through a procedure parameter.
  virtual bool IsCallThruProcParam(ExprHandle h) = 0;


  //--------------------------------------------------------
  // Statements: General
  //--------------------------------------------------------

  // Given a statement, return its IRStmtType.
  virtual IRStmtType GetStmtType(StmtHandle h) = 0;

  // Given a statement, return a label (or NULL if
  // there is no label associated with the statement).
  virtual StmtLabel GetLabel(StmtHandle h) = 0;

  // Given a compound statement, return an IRStmtIterator* for the
  // statements.  The user must free the iterator's memory via delete.
  // A compound is a list of statements.
  virtual IRStmtIterator *GetFirstInCompound(StmtHandle h) = 0; 
  
  //--------------------------------------------------------
  // Loops
  //--------------------------------------------------------

  // Given a loop statement, return an IRStmtIterator* for the loop body.
  // The user must free the iterator's memory via delete.
  virtual IRStmtIterator *LoopBody(StmtHandle h) = 0; 

  // Given a loop statement, return the loop header statement.  This 
  // would be the initialization statement in a C 'for' loop, for example.
  virtual StmtHandle LoopHeader(StmtHandle h) = 0;

  // Given a loop statement, return the increment statement. 
  virtual StmtHandle GetLoopIncrement(StmtHandle h) = 0;

  // Given a loop statement, return:
  // 
  // True: If the number of loop iterations is defined
  // at loop entry (i.e. Fortran semantics).  This causes the CFG builder 
  // to add the loop statement representative to the header node so that
  // definitions from inside the loop don't reach the condition and increment
  // specifications in the loop statement.
  //
  // False: If the number of iterations is not defined at
  // entry (i.e. C semantics), we add the loop statement to a node that
  // is inside the loop in the CFG so definitions inside the loop will 
  // reach uses in the conditional test. For C style semantics, the 
  // increment itself may be a separate statement. if so, it will appear
  // explicitly at the bottom of the loop. 
  virtual bool LoopIterationsDefinedAtEntry(StmtHandle h) = 0;

  //--------------------------------------------------------
  // Invariant: a two-way conditional or a multi-way conditional MUST provide
  // provide either a target, or a target label
  //--------------------------------------------------------

  //--------------------------------------------------------
  // Structured two-way conditionals
  //
  // Note: An important pre-condition for structured conditionals is
  // that chains of else-ifs must be represented as nested elses.  For
  // example, this Matlab statement:
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
  //--------------------------------------------------------

  // Given a structured two-way conditional statement, return an
  // IRStmtIterator* for the "true" part (i.e., the statements under
  // the "if" clause). The user must free the iterator's memory via
  // delete.
  virtual IRStmtIterator *TrueBody(StmtHandle h) = 0;

  // Given a structured two-way conditional statement, return an
  // IRStmtIterator* for the "else" part (i.e., the statements under
  // the "else" clause). The user must free the iterator's memory via
  // delete.
  virtual IRStmtIterator *ElseBody(StmtHandle h) = 0;

  //--------------------------------------------------------
  // Structured multiway conditionals
  //--------------------------------------------------------

  // Given a structured multi-way branch, return the number of cases.
  // The count does not include the default/catchall case.
  virtual int NumMultiCases(StmtHandle h) = 0;

  // Given a structured multi-way branch, return an IRStmtIterator* for
  // the body corresponding to target 'bodyIndex'. The n targets are 
  // indexed [0..n-1].  The user must free the iterator's memory via delete.
  virtual IRStmtIterator *MultiBody(StmtHandle h, int bodyIndex) = 0;

  // Given a structured multi-way branch, return true if the cases have
  // implied break semantics.  For example, this method would return false
  // for C since one case will fall-through to the next if there is no
  // explicit break statement.  Matlab, on the other hand, implicitly exits
  // the switch statement once a particular case has executed, so this
  // method would return true.
  virtual bool IsBreakImplied(StmtHandle multicond) = 0;

  // Given a structured multi-way branch, return an IRStmtIterator*
  // for the body corresponding to default/catchall case.  The user
  // must free the iterator's memory via delete.
  virtual IRStmtIterator * GetMultiCatchall (StmtHandle h) = 0;

  //--------------------------------------------------------
  // Unstructured two-way conditionals: 
  //--------------------------------------------------------

  // Given an unstructured two-way branch, return the label of the
  // target statement.  The second parameter is currently unused.
  virtual StmtLabel  GetTargetLabel(StmtHandle h, int n) = 0;

  //--------------------------------------------------------
  // Unstructured multi-way conditionals
  // FIXME: Review all of the multi-way stuff.
  //--------------------------------------------------------

  // Given an unstructured multi-way branch, return the number of targets.
  // The count does not include the optional default/catchall case.
  virtual int NumUMultiTargets(StmtHandle h) = 0;

  // Given an unstructured multi-way branch, return the label of the target
  // statement at 'targetIndex'. The n targets are indexed [0..n-1]. 
  virtual StmtLabel GetUMultiTargetLabel(StmtHandle h, int targetIndex) = 0;

  // Given an unstructured multi-way branch, return label of the target
  // corresponding to the optional default/catchall case.  Return 0
  // if there is no default target.
  virtual StmtLabel GetUMultiCatchallLabel(StmtHandle h) = 0;

  //--------------------------------------------------------
  // Special, for assembly-language level instructions only.
  // These are necessary because there are some intricacies involved
  // in building a CFG for an instruction set which has delayed branches,
  // and in particular, allows branches within branch delay slots. 
  //--------------------------------------------------------

  // Given a statement, return true if it issues in parallel with its
  // successor.  This would be used, for example, when the underlying IR
  // is a low-level/assembly-level language for a VLIW or superscalar
  // instruction set. The default implementation (which is appropriate
  // for most IR's) is to return false.
  virtual bool ParallelWithSuccessor(StmtHandle h) { return false; }

  // Given an unstructured branch/jump statement, return the number
  // of delay slots. Again, this would be used when the underlying IR
  // is a low-level/assembly-level language for a VLIW or superscalar
  // instruction set. The default implementation (which is appropriate
  // for most IR's) is to return 0.
  virtual int NumberOfDelaySlots(StmtHandle h) { return 0; }

  
  //--------------------------------------------------------
  // Obtain uses and defs
  //--------------------------------------------------------

  // Given a statement, return an IRUseDefIterator* (which is used by the
  // client to enumerate all the variables referenced by the statement).
  // The user must free the iterator's memory via delete.
  virtual IRUseDefIterator *GetUses(StmtHandle h) = 0; 

  // Given a statement, return an IRUseDefIterator* (which is used by the
  // client to enumerate all the variables defined by the statement).
  // The user must free the iterator's memory via delete.
  virtual IRUseDefIterator *GetDefs(StmtHandle h) = 0; 

  //--------------------------------------------------------
  // Symbol Handles
  //--------------------------------------------------------

  // FIXME
  virtual SymHandle GetProcSymHandle(ProcHandle h) = 0;

  // Given an ExprHandle containing a use or def (including a
  // callsite), return the SymHandle.  The SymHandle is mapped to a
  // symbol table entry for the variable or callsite.
  //virtual SymHandle GetSymHandle(LeafHandle vh) = 0; FIXME
  virtual SymHandle GetSymHandle(ExprHandle h) = 0; // GetExprSymHandle

  // Given a SymHandle, return the textual name.
  virtual const char *GetSymNameFromSymHandle(SymHandle sh) = 0;
  
  //--------------------------------------------------------
  // Debugging
  //--------------------------------------------------------  

  // Given a LeafHandle, pretty-print it to the output stream os.
  // The default implementation does nothing.
  virtual void PrintLeaf(LeafHandle vh, std::ostream & os) { }

  // Given a statement, pretty-print it to the output stream os.
  // The default implementation does nothing.
  virtual void Dump(StmtHandle stmt, std::ostream& os) { }
  
  //--------------------------------------------------------
  // FIXME: the procedures below are of undetermined utility at the moment.
  //--------------------------------------------------------

  // Given a loop statement, return the loop condition expression.
  // This is the expression that tests for loop termination.
  virtual ExprHandle GetLoopCondition(StmtHandle h) = 0;

  // DQ (11/27/2005): I think this is not used.  Most locations (all but SgDoWhileStmt) 
  // represent the condition as a SgStatement (especially true now with the current 
  // modifications (required by C and C++ grammar specification)).
  // Given an unstructured two-way branch, return the branch condition expression.
  // virtual ExprHandle GetCondition(StmtHandle h) = 0;
  // virtual StmtHandle GetCondition(StmtHandle h) = 0;

  // Given an unstructured multi-way branch, return the condition
  // expression corresponding to target 'targetIndex'. The n targets
  // are indexed [0..n-1].
  virtual ExprHandle GetUMultiCondition(StmtHandle h, int targetIndex) = 0; // multiway target condition 

  // Given a structured multi-way branch, return the condition
  // expression corresponding to target 'bodyIndex'. The n targets are
  // indexed [0..n-1].
  virtual ExprHandle GetSMultiCondition(StmtHandle h, int bodyIndex) = 0; //condition for multi body 

  // Given a structured multi-way branch, return the switch/selector
  // expression.
  virtual ExprHandle GetMultiExpr(StmtHandle h) = 0; //multi-way beginning expression
};

// FIXME: Used from CFG::ltnode
//const char *IRGetSymNameFromLeafHandle(LeafHandle vh); 

#endif // IRInterface_h
