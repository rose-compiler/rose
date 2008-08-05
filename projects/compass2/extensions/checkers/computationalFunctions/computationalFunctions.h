// Computational Functions
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"

#ifndef COMPASS_COMPUTATIONAL_FUNCTIONS_H
#define COMPASS_COMPUTATIONAL_FUNCTIONS_H

namespace CompassAnalyses
{ 
  namespace ComputationalFunctions
    { 
      /*! \brief Computational Functions: Add your description here 
       */

      extern const std::string checkerName;
      extern std::string shortDescription;
      extern std::string longDescription;

      // Specification of Checker Output Implementation
      class CheckerOutput: public Compass::OutputViolationBase
	{ 
	public:
	  CheckerOutput(SgNode* node);
	};

      // Specification of Checker Traversal Implementation

      class Traversal
	: public AstSimpleProcessing, public Compass::TraversalBase
	{
	  // Checker specific parameters should be allocated here.
	  static int intOps;
	  static int floatOps;
	  static int intOps_actual;
	  static int floatOps_actual;

	public:
	  Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);
	  static std::string intToString(int n) {
	    std::ostringstream myStream; //creates an ostringstream object
	    myStream << n << std::flush;
	    return (myStream.str()); //returns the string form of the stringstream object
	  }
	  // The implementation of the run function has to match the traversal being called.
	  void run(SgNode* n){ this->traverse(n, preorder); };
	  static std::string getIntOps() { return intToString(intOps);} 
	  static std::string getIntOps_actual() { return intToString(intOps_actual);} 
	  static std::string getFloatOps() { return intToString(floatOps);} 
	  static std::string getFloatOps_actual() { return intToString(floatOps_actual);} 

	  void checkNode(SgNode* node);
	  int checkDepth(std::vector<SgNode*> children);
	  void visit(SgNode* n);
	};
    }
}

// COMPASS_COMPUTATIONAL_FUNCTIONS_H
#endif 

