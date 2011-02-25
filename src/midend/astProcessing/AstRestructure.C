

// Author: Markus Schordan
// $Id: AstRestructure.C,v 1.3 2006/04/24 00:21:32 dquinlan Exp $

#ifndef ASTRESTRUCTURE_C
#define ASTRESTRUCTURE_C

#include "sage3basic.h"
#include "AstRestructure.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

void
AstRestructure::lock(SgNode* astNode) {
  Lock::iterator f=lockedNodes.find(astNode);
    if(f==lockedNodes.end()) {
      lockedNodes.insert(astNode);
      cout << "Node: " << astNode->sage_class_name() << " locked." << endl;
    } else {
      cerr << "ERROR: attempted to re-lock node " << astNode->sage_class_name() << " @" << astNode << endl;
      ROSE_ASSERT(false);
    }
}

void
AstRestructure::unlock(SgNode* astNode) {
  Lock::iterator f=lockedNodes.find(astNode);
    if(f!=lockedNodes.end()) {
      lockedNodes.erase(astNode);
      cout << "Node: " << astNode->sage_class_name() << " unlocked." << endl;
      // test is necessary otherwise the access would insert a default element
      Schedule::iterator sch=scheduledReplacements.find(astNode);
      if(sch!=scheduledReplacements.end()) {
        SgStatement* astNode2=dynamic_cast<SgStatement*>(astNode);
        ROSE_ASSERT(astNode2);
        cout << "Replacing Node: " << astNode2->sage_class_name() << endl;
        immediateReplace(astNode2,scheduledReplacements[astNode2]);
      }
    } else {
      cerr << "ERROR: attempted to unlock non-locked node " << astNode->sage_class_name() << " @" << astNode << endl;
      ROSE_ASSERT(false);
    }
}

void 
AstRestructure::delayedReplace(SgNode* astNode,string s) {
  scheduledReplacements.insert(make_pair(astNode,s));
}

void 
AstRestructure::unparserReplace(SgExpression* astNode,string s)
   {
     cout <<"AstRestructure::unsafeReplace:" << s << endl;

  // DQ (7/19/2008): Modified interface to AstUnparseAttribute
  // AstUnparseAttribute* newa = new AstUnparseAttribute(s);
     AstUnparseAttribute* newa = new AstUnparseAttribute(s,AstUnparseAttribute::e_replace);

     printf ("AstRestructure::unparserReplace(): using new attribute interface \n");
     astNode->addNewAttribute("_UnparserSourceReplacement",newa);

  // DQ (4/8/2004): This variable declaration does not appear to serve any 
  //                purpose and generates an warning. Is it required?
  // AstAttribute* a=astNode->attribute["_UnparserSourceReplacement"];
   }

void 
AstRestructure::immediateReplace(SgStatement* astNode,string s) {
  cout << "IMMEDIATE REPLACE: OLD AST-SOURCE: " << astNode->unparseToString() << endl;
  cout << "IMMEDIATE REPLACE: WITH: " << endl;
  cout << s << endl;
  MiddleLevelRewrite::insert(astNode,s,MidLevelCollectionTypedefs::SurroundingScope,MidLevelCollectionTypedefs::ReplaceCurrentPosition);
    //MiddleLevelRewrite::replace(statement,newSourceString);
  cout << "IMMEDIATE REPLACE RESULT: PARENT's NEW AST-SOURCE: " << endl;
  cout << astNode->get_parent()->unparseToString() << endl;
    /*
  ROSE_ASSERT(astNode);
  targetNode=astNode;
  sourceFragment=s;
  ROSE_ASSERT(sageProject);
  RestructureInhType inh(sageProject);
  traverseInputFiles(this->sageProject,inh);
  cout << "IMMEDIATE REPLACE RESULT: PARENT's NEW AST-SOURCE: " << astNode->get_parent()->unparseToString() << endl;
    */
};

/*
RestructureInhType
AstRestructure::evaluateRewriteInheritedAttribute (SgNode* astNode,
                                                   RestructureInhType inheritedValue ) {
  RestructureInhType inh(inheritedValue,astNode);
  return inh;
}

RestructureSynType
AstRestructure::evaluateRewriteSynthesizedAttribute (SgNode* astNode,
                                                      RestructureInhType inheritedValue,
                                                      SubTreeSynthesizedAttributes synList ) {
  RestructureSynType sa(astNode);
  for (SubTreeSynthesizedAttributes::iterator i = synList.begin(); i != synList.end(); i++) {
    sa+=*i;
  }

  
  //  if(dynamic_cast<SgExpression*>(astNode)) {
  //  cout << astNode->unparseToString() << endl;
  //  }
  
  if(astNode==targetNode) {
    cout << "TARGET NODE FOUND." << endl;
    AST_Rewrite::AST_FragmentString sourceFragmentSpecifier1(includeHeaders,inheritedValue,
                                                            AST_Rewrite::Preamble,
                                                            AST_Rewrite::PreamblePositionInScope
                                                             );
    sa.addSourceCodeString(sourceFragmentSpecifier1);      
    MidL
    AST_Rewrite::AST_FragmentString sourceFragmentSpecifier2(sourceFragment,inheritedValue,
                                                            AST_Rewrite::LocalScope,
                                                            AST_Rewrite::ReplaceCurrentPosition,
                                                            AST_Rewrite::TransformationString,
                                                            false);
    //MiddleLevelRewrite::insert(statement,newSourceString,MidLevelCollectionTypedefs::ReplaceCurrentPosition);
    MiddleLevelRewrite::replace(statement,newSourceString);

    sa.addSourceCodeString(sourceFragmentSpecifier2);      
    //cout << "\nREPLACED:" << sourceFragment << endl << endl;
    }
  return sa;
}
*/

#endif
