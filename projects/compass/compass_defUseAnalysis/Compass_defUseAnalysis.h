// DefUseAnalysis
// Author: Thomas Panas
// Date: 21March 2008

#include "compass.h"

#ifndef COMPASS_DEF_USE_H
#define COMPASS_DEF_USE_H

namespace CompassAnalyses
   { 
     namespace Compass_defUseAnalysis
        { 
        /*! \brief Loc Per Function: Add your description here 
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
	       static int loc;
	       static int loc_actual;
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);
                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };
		    static std::string getLOC() { 
		      std::ostringstream myStream; //creates an ostringstream object
		      myStream << loc << std::flush;
		      return (myStream.str()); //returns the string form of the stringstream object
		    }
		    static std::string getLOC_actual() { 
		      std::ostringstream myStream; //creates an ostringstream object
		      myStream << loc_actual << std::flush;
		      return (myStream.str()); //returns the string form of the stringstream object
		    }
                    void visit(SgNode* n);
             };
        }
   }

// Compass_DefUseAnalysis
#endif 

