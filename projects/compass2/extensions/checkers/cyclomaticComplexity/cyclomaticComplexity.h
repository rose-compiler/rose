// Cyclomatic Complexity
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"

#ifndef COMPASS_CYCLOMATIC_COMPLEXITY_H
#define COMPASS_CYCLOMATIC_COMPLEXITY_H

namespace CompassAnalyses
   { 
     namespace CyclomaticComplexity
        { 
        /*! \brief Cyclomatic Complexity: Add your description here 
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
	       static int cc;
	       static int complexity;
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };
		    static std::string getCC() { 
		      std::ostringstream myStream; //creates an ostringstream object
		      myStream << cc << std::flush;
		      return (myStream.str()); //returns the string form of the stringstream object
		    }
		    static std::string getComplexity() { 
		      std::ostringstream myStream; //creates an ostringstream object
		      myStream << complexity << std::flush;
		      return (myStream.str()); //returns the string form of the stringstream object
		    }
		    void checkNode(SgNode* node);
		    int checkDepth(std::vector<SgNode*> children);
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_CYCLOMATIC_COMPLEXITY_H
#endif 

