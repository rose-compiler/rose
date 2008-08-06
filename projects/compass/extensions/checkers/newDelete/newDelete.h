// New Delete
// Author: Thomas Panas
// Date: 24-August-2007


#include "compass.h"

#ifndef COMPASS_NEW_DELETE_H
#define COMPASS_NEW_DELETE_H

//#include "BoostGraphInterface.hxx"

namespace CompassAnalyses
{ 
  namespace NewDelete
    { 
      /*! \brief New Delete: Add your description here 
       */
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
	  BoolWithTrace expressionIsNewExpr(SgExpression* expr);
	  void checkNewDelForFunction(SgDeleteExp* delExpr, std::string name);

	  template<typename T>    
	    static std::string ToString(T t){
	    std::ostringstream myStream; //creates an ostringstream object               
	    myStream << t << std::flush;       
	    return myStream.str(); //returns the string form of the stringstream object
	  }                        

	public:
	  Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);


	  // The implementation of the run function has to match the traversal being called.
	  void run(SgNode* n){  this->traverse(n, preorder); };

	  void visit(SgNode* n);
	};
    }
}

// COMPASS_NEW_DELETE_H
#endif
