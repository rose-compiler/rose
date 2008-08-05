// Null Deref
// Author: Thomas Panas
// Date: 24-August-2007

#include "compass.h"

#ifndef COMPASS_NULL_DEREF_H
#define COMPASS_NULL_DEREF_H

#include "BoostGraphInterface.hxx"

namespace CompassAnalyses
{ 
  namespace NullDeref
    { 
      extern const std::string checkerName;
      extern const std::string shortDescription;
      extern const std::string longDescription;

      // Specification of Checker Output Implementation
      class CheckerOutput: public Compass::OutputViolationBase
	{ 
	public:
	  CheckerOutput(std::string problem, SgNode* node);
	};

      // Specification of Checker Traversal Implementation
      class Traversal
	: public AstSimpleProcessing, public Compass::TraversalBase
	{
	  // Checker specific parameters should be allocated here.
	  typedef std::pair<bool, std::vector<SgExpression*> > BoolWithTrace;
	  std::map<SgExpression*, BoolWithTrace> traces;

	  BoolWithTrace expressionIsNull(SgExpression* expr);

	  //	  bool isLegitimateNullPointerCheck(SgExpression* expr, SgInitializedName* pointerVar, bool invertCheck) const;
	  void checkNullDeref(std::string analysisname, SgExpression* theExp, std::string name);

	  template<typename T>    
	    static std::string ToString(T t){
	    std::ostringstream myStream; //creates an ostringstream object               
	    myStream << t << std::flush;       
	    return myStream.str(); //returns the string form of the stringstream object
	  }                                 

	  int counter;
	  int max;
	  static bool debug;

	public:
	  Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

	  // The implementation of the run function has to match the traversal being called.
	  void run(SgNode* n){ 
	    //SgProject* pr = isSgProject(n);
	    //ROSE_ASSERT(pr);
	    //	    Compass::runDefUseAnalysis(pr);
	    counter=0;
	    debug=true;
	    std::vector<SgNode*> exprList = NodeQuery:: querySubTree (n, V_SgFunctionDeclaration);
	    max = exprList.size();
	    this->traverse(n, preorder); 
	  };

	  void visit(SgNode* n);
	};
    }
}

// COMPASS_NULL_DEREF_H
#endif 


