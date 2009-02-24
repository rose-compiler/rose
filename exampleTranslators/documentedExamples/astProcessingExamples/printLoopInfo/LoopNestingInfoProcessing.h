// Author: Markus Schordan, Vienna University of Technology, 2004.
// $Id: LoopNestingInfoProcessing.h,v 1.4 2008/01/08 02:55:50 dquinlan Exp $

#ifndef PRINTVARS_H
#define PRINTVARS_H

#include <string>
#include <iostream>


// using namespace std;

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

#endif
