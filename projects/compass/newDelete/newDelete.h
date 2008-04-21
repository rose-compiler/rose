// New Delete
// Author: Thomas Panas
// Date: 24-August-2007


#include "compass.h"

#ifndef COMPASS_NEW_DELETE_H
#define COMPASS_NEW_DELETE_H

#include "BoostGraphInterface.hxx"

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
	: public AstSimpleProcessing, public Compass::TraversalBase,  BOOSTGraphInterface::tps_graph_interface
	{
	  // Checker specific parameters should be allocated here.


	  std::string getFileName(SgFile* src);
	  void checkNewDelForFunction(SgFunctionDeclaration* funcDecl, SgDeleteExp* delExpr, std::string name);
	  void switchForAssignment(std::vector<BOOSTGraphInterface::tps_node> &vec, 
				   BOOSTGraphInterface::tps_node n, 
				   BOOSTGraphInterface::tps_node oldN, SgExpression* rhs) const;
	  std::vector <BOOSTGraphInterface::tps_node> tps_out_edges(BOOSTGraphInterface::tps_node node) const;

	  static std::string printTrace(const std::vector<BOOSTGraphInterface::tps_node>& trace);
	  static std::string print_tps_error(std::string error, BOOSTGraphInterface::tps_node n, BOOSTGraphInterface::tps_node oldN) ;

	  static std::string trimIfNeeded(const std::string& s);
	  static SgStatement* getStatement(SgNode* n);

	  

	  template<typename T>    
	    static std::string ToString(T t){
	    std::ostringstream myStream; //creates an ostringstream object               
	    myStream << t << std::flush;       
	    return myStream.str(); //returns the string form of the stringstream object
	  }                        

	public:
	  Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

	  // The implementation of the run function has to match the traversal being called.
	  void run(SgNode* n){ 

	  if (Compass::verboseSetting >= 0)
	    std::cout << " TURN ON BOOST FOR THIS ANALYSIS. " << std::endl;
	    this->traverse(n, preorder); };

	  void visit(SgNode* n);
	};
    }
}

// COMPASS_NEW_DELETE_H
#endif
