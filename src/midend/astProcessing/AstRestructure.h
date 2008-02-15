// Author: Markus Schordan
// $Id: AstRestructure.h,v 1.2 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTRESTRUCTURE_H
#define ASTRESTRUCTURE_H

#include "roseInternal.h"
#include "sage3.h"
#include "rewrite.h"
#include <set>
#include <utility>

class AstUnparseAttribute : public AstAttribute {
 public:
  AstUnparseAttribute(std::string s):unparseReplacement(s) {}
  virtual std::string toString() { return unparseReplacement; }
  std::string unparseReplacement;
};

/*
class RestructureInhType : public AST_Rewrite::InheritedAttribute {
public:
  RestructureInhType(SgNode* n)
    : AST_Rewrite::InheritedAttribute(n) {}
  RestructureInhType(const RestructureInhType & X )
    : AST_Rewrite::InheritedAttribute(X) {}
  RestructureInhType ( const RestructureInhType& X, SgNode* astNode )
    : AST_Rewrite::InheritedAttribute (X,astNode) {};
  RestructureInhType &
    RestructureInhType::operator= ( const RestructureInhType & X ) {
    // Call the base class operator=
    AST_Rewrite::InheritedAttribute::operator=(X);
    return *this;
  }
};

class RestructureSynType : public AST_Rewrite::SynthesizedAttribute {
public:
  RestructureSynType() {}
  RestructureSynType(SgNode* n):AST_Rewrite::SynthesizedAttribute (n) {}
};
*/

class AstRestructure 
{
 public:
  AstRestructure(SgProject* project) {}
  AstRestructure(SgProject* project,std::string incheader) {}


  // replaces the AST with astNode as root node with the AST representing string s
  void immediateReplace(SgStatement* astNode,std::string s);

  // attaches a string s to the AST such that when 'unparse' or
  // unparseToString or unparseToCompleteString is called, the string
  // 's' is unparsed instead of the subtree with node 'astNode' as
  // root.  This function allows to replace parts of expressions!
  static void unparserReplace(SgExpression* astNode, std::string s); 

  // this replace is delayed until unlock of node astNode is called.
  void delayedReplace(SgNode* astNode,std::string s);
  void lock(SgNode* astNode); // used by abstract parser 
  void unlock(SgNode* astNode);  // used by abstract parser

 private:
  SgProject* sageProject;
  SgNode* targetNode;
  std::string sourceFragment;
  std::string includeHeaders; // will become obsolete with new rewrite system

  typedef std::set<SgNode*> Lock;
  Lock lockedNodes;
  typedef std::map<SgNode*,std::string> Schedule;
  typedef Schedule::value_type SchedulePair;
  Schedule scheduledReplacements;
  
  // Functions required by the AST Rewrite Tree Traversal mechanism
  /*
  RestructureInhType
    evaluateRewriteInheritedAttribute (SgNode* astNode,
				       RestructureInhType inheritedValue );
  RestructureSynType
    evaluateRewriteSynthesizedAttribute (SgNode* astNode,
					 RestructureInhType inheritedValue,
					 SubTreeSynthesizedAttributes attributeList );
  */
};

#endif
