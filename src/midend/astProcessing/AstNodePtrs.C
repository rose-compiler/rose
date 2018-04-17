//#include "rose.h"
// Author: Markus Schordan
// $Id: AstNodePtrs.C,v 1.3 2006/04/24 00:21:32 dquinlan Exp $


// DQ (3/6/2003): added from AstProcessing.h to avoid referencing
// the traversal classes in AstFixes.h before they are defined.
#include "sage3basic.h"
#include "roseInternal.h"

#include <sstream>
#include "AstNodePtrs.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

//  NodeStatistics Constructors/Destructors
AstNodePtrs::AstNodePtrs() {
}
AstNodePtrs::~AstNodePtrs() { 
}

AstNodePtrSynAttr
AstNodePtrs::evaluateSynthesizedAttribute(SgNode* node,SynthesizedAttributesList l) { 
  ROSE_ASSERT(node);
  ROSE_ASSERT(node->variantT()<V_SgNumVariants);

  string s=string(node->sage_class_name())+"(";
  
// DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
// bool nullValueExists=false;
  for(SynthesizedAttributesList::iterator i=l.begin(); i!=l.end();i++) {
    if((*i).node==NULL) {
   // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
   // nullValueExists=true;
      s+=", NULL";
    } else {
      s+=string(", ")+(*i).node->sage_class_name();
    }
  }
  s+=")";
  
  //if(nullValueExists) {
  //  cout << s << endl;
  //  if(SgDotExp* dotNode=dynamic_cast<SgDotExp*>(node)) {
  //    cout << "AST TEST: SgDotExp(" << dotNode->get_lhs_operand_i() << ", " << dotNode->get_rhs_operand_i() << ") found." << endl;
  //  }
  //}

  // provide a list of node pointers for some simple operations on the AST
  AstNodePointersList pl;
  for(SynthesizedAttributesList::iterator i=l.begin(); i!=l.end();i++) {
    SgNode* np=(*i).node;
    pl.push_back(np);
  }
  visitWithAstNodePointersList(node,pl);

  AstNodePtrSynAttr syn;
  syn.node=node;
  return syn;
}
