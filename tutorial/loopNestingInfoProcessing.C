// Author: Markus Schordan, Vienna University of Technology, 2004.
// $Id: loopNestingInfoProcessing.C,v 1.1 2006/04/24 00:22:00 dquinlan Exp $

// #include <string>
// #include <iostream>

#include "rose.h"

using namespace std;

typedef unsigned int NestingLevel;
typedef unsigned int NestingDepth;
typedef NestingLevel InhNestingLevel;
typedef NestingDepth SynNestingDepth;

/*! This class is used to attach information to AST nodes.
    Method 'toString' is overridden and
    called when a pdf file is generated. This allows to display
    the value of an AST node attribute (annotation) in a pdf file.
*/
class NestingLevelAnnotation : public AstAttribute {
public:
  NestingLevelAnnotation(NestingLevel n,NestingDepth d)
    : _nestingLevel(n),_nestingDepth(d) {}
  NestingLevel getNestingLevel() { return _nestingLevel; }
  NestingDepth getNestingDepth() { return _nestingDepth; }
  string toString() { 
    ostringstream ss; ss<<_nestingLevel<<","<<_nestingDepth; 
    return ss.str(); 
  }
private:
  NestingLevel _nestingLevel;
  NestingDepth _nestingDepth;
};

/* !
The loop nesting level and nesting depth for each while/dowhile/for
loop nest is computed. It is attached to the AST as annotation and
can be accessed as node->attribute["loopNestingInfo"]
after the processing has been performed.
The maximum nesting level of the whole AST is computed as
"accumulated" value in a member variable and can be accessed with
getMaxNestingLevel().
*/
class LoopLevelProcessing : public AstTopDownBottomUpProcessing<InhNestingLevel,SynNestingDepth> {
public:
  LoopLevelProcessing():_maxNestingLevel(0) {}

  /*! Performs a traversal of the AST and computes loop-nesting information by using
      inherited and synthesized attributes. The results are attached to the AST as
      annotation.
  */
  void attachLoopNestingAnnotaton(SgProject* node) { traverseInputFiles(node,0); }

  /*! Returns the maximum nesting level of the entire AST (of the input file). 
      Requires attachLoopNestingAnnotation (to be called before)
  */
  NestingLevel getMaxNestingLevel() { return _maxNestingLevel; }

protected:
  //! computes the nesting level
  InhNestingLevel evaluateInheritedAttribute(SgNode*,InhNestingLevel);
  //! computes the nesting depth
  SynNestingDepth evaluateSynthesizedAttribute(SgNode*,InhNestingLevel,SynthesizedAttributesList);
  //! provides the default value 0 for the nesting depth
  SynNestingDepth defaultSynthesizedAttribute(InhNestingLevel inh);
private:
  NestingLevel _maxNestingLevel;
};


NestingLevel
LoopLevelProcessing::evaluateInheritedAttribute(SgNode* node, NestingLevel loopNestingLevel) {

  //! compute maximum nesting level of entire program in accumulator (member variable)
  if(loopNestingLevel>_maxNestingLevel)
    _maxNestingLevel=loopNestingLevel;

  switch(node->variantT()) {
  case V_SgGotoStatement:
    cout << "WARNING: Goto statement found. We do not consider goto loops.\n";
 // DQ (11/17/2005): Added return statment to avoid g++ warning: control reaches end of non-void function
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
LoopLevelProcessing::evaluateSynthesizedAttribute(SgNode* node,InhNestingLevel nestingLevel,SynthesizedAttributesList l)
   {
     if (nestingLevel>_maxNestingLevel)
          _maxNestingLevel=nestingLevel;

  // compute maximum nesting depth of synthesized attributes 
     SynNestingDepth nestingDepth=0;
     for(SynthesizedAttributesList::iterator i=l.begin();i!=l.end();i++)
        {
          if(*i>nestingDepth) nestingDepth=*i;
        }

     switch(node->variantT())
        {
          case V_SgDoWhileStmt:
          case V_SgForStatement:
          case V_SgWhileStmt:
             {
               nestingDepth++;
               cout << "Nesting level:" << nestingLevel << ", nesting depth:" << nestingDepth << endl;
               break;
             }

          default:
             {
            // DQ (11/17/2005): Nothing to do here, but explicit default in switch avoids lots of warnings.
             }
        }

  // add loop nesting level as annotation to AST 
     NestingLevelAnnotation* nla = new NestingLevelAnnotation(nestingLevel,nestingDepth);
     ROSE_ASSERT(nla != NULL);

  // DQ (1/2/2006): Added support for new attribute interface.
  // printf ("LoopLevelProcessing::evaluateSynthesizedAttribute(): using new attribute interface \n");
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

int main ( int argc, char** argv) {
   // Initialize and check compatibility. See Rose::initialize
   ROSE_INITIALIZE;

   // command line parameters are passed to EDG
   // non-EDG parameters are passed (through) to ROSE (and the vendor compiler)
   SgProject* root=frontend(argc,argv); 
   LoopLevelProcessing t;

   // traverse only C++ files specified on command line
   // do not traverse included header files
   cout << endl << "Output:" << endl;
   t.traverseInputFiles(root,0);
   cout << "Max loop nesting level: " << t.getMaxNestingLevel() << endl;
   cout << endl;

   // Generate a dot file showing the AST
   AstDOTGeneration astdotgen;
   astdotgen.generateInputFiles(root,AstDOTGeneration::PREORDER);

   // Generate a pdf file showing the AST
#if ROSE_WITH_LIBHARU
   AstPDFGeneration astpdfgen;
   astpdfgen.generateInputFiles(root);
#else
   cout << "Warning: libharu is not enabled" << endl;
#endif
   
   return 0;
}   
