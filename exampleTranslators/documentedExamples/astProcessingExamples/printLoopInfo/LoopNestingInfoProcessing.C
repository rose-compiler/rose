// Author: Markus Schordan, Vienna University of Technology, 2004.
// $Id: LoopNestingInfoProcessing.C,v 1.3 2006/04/24 00:21:26 dquinlan Exp 
#include "rose.h"
#include "LoopNestingInfoProcessing.h"

NestingLevel
LoopLevelProcessing::evaluateInheritedAttribute(SgNode* node, NestingLevel loopNestingLevel) {

  //! compute maximum nesting level of entire program in accumulator (member variable)
  if(loopNestingLevel>_maxNestingLevel)
    _maxNestingLevel=loopNestingLevel;

  switch(node->variantT()) {
  case V_SgGotoStatement:
    cout << "WARNING: Goto statement found. We do not consider goto loops.\n";
 // DQ (1/1/06): Added a return statement for case of V_SgGotoStatement
    return loopNestingLevel;
    break;
  case V_SgDoWhileStmt:
  case V_SgForStatement:
  case V_SgWhileStmt:
    return loopNestingLevel+1;
  default:
    return loopNestingLevel;
  }

}

SynNestingDepth 
LoopLevelProcessing::defaultSynthesizedAttribute(InhNestingLevel inh) {
  /*! we do not need the inherited attribute here
      as default value for synthesized attribute we set 0, representing nesting depth 0.
  */
  return 0;
}

SynNestingDepth
LoopLevelProcessing::evaluateSynthesizedAttribute(SgNode* node,InhNestingLevel nestingLevel,SynthesizedAttributesList l) {

  if(nestingLevel>_maxNestingLevel)
    _maxNestingLevel=nestingLevel;

  //! compute maximum nesting depth of synthesized attributes 
  SynNestingDepth nestingDepth=0;
  for(SynthesizedAttributesList::iterator i=l.begin();i!=l.end();i++) {
    if(*i>nestingDepth) nestingDepth=*i;
  }

  switch(node->variantT()) {
     case V_SgDoWhileStmt:
     case V_SgForStatement:
     case V_SgWhileStmt: {
        nestingDepth++;
        cout << "Nesting level:" << nestingLevel << ", nesting depth:" << nestingDepth << endl;
     }

  // DQ (1/1/06): Added a default case to avoid compiler warnings 
     default: {}     
  }

  //! add loop nesting level as annotation to AST 
  NestingLevelAnnotation* nla = new NestingLevelAnnotation(nestingLevel,nestingDepth);

  // DQ (1/2/2006): Added support for new attribute interface.
     printf ("LoopLevelProcessing::evaluateSynthesizedAttribute(): using new attribute interface \n");
#if 0
     if (node->get_attribute() == NULL)
        {
          AstAttributeMechanism* attributePtr = new AstAttributeMechanism();
          ROSE_ASSERT(attributePtr != NULL);
          node->set_attribute(attributePtr);
        }
#endif
  // node->attribute.add("loopNestingInfo",nla);
  // node->attribute().add("loopNestingInfo",nla);
     node->addNewAttribute("loopNestingInfo",nla);

  //! return the maximum nesting depth as synthesized attribute
  return nestingDepth;
}
