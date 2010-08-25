// Author: Markus Schordan
// $Id: AstRestructure.h,v 1.2 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTRESTRUCTURE_H
#define ASTRESTRUCTURE_H

#include "roseInternal.h"
//#include "sage3.h"
#include "rewrite.h"
#include <set>
#include <utility>

#if 0
// This has been moved to the unparser...
class AstUnparseAttribute : public AstAttribute
   {
     public:

       // DQ (7/19/2008): I think we should not reuse the PreprocessingInfo::RelativePositionType
       // since it does not make sense to "replace" an IR node with a comment or CPP directive, I think.
       // typedef PreprocessingInfo::RelativePositionType RelativeLocation;
          enum RelativePositionType
             {
               defaultValue = 0, // let the zero value be an error value
               undef        = 1, // Position of the directive is only going to be defined
                                 // when the preprocessing object is copied into the AST,
                                 // it remains undefined before that
               before       = 2, // Directive goes before the correponding code segment
               after        = 3, // Directive goes after the correponding code segment
               inside       = 4, // Directive goes inside the correponding code segment (as in between "{" and "}" of an empty basic block)

            // DQ (7/19/2008): Added additional fields so that we could use this enum type in the AstUnparseAttribute
               replace       = 5, // Support for replacing the IR node in the unparsing of any associated subtree
               before_syntax = 6, // We still have to specify the syntax
               after_syntax  = 7  // We still have to specify the syntax
             };

          RelativePositionType location;
          std::string unparseReplacement;
          std::vector< std::pair<std::string,RelativePositionType> > stringList;

          AstUnparseAttribute(std::string s, RelativePositionType inputlocation )
             : location(inputlocation), unparseReplacement(s)
             {
            // Add the string location pair to the list.
               stringList.push_back(std::pair<std::string,RelativePositionType>(s,inputlocation));
             }
          virtual std::string toString() { return unparseReplacement; }
   };
#endif

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
