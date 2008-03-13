// Name Consistency
// Author: Andreas Saebjoernsen
// Date: 23-July-2007

#include "compass.h"


#if USE_ROSE_BOOST_WAVE_SUPPORT 
#include "checkNameImpl.h"
#endif

#ifndef COMPASS_NAME_CONSISTENCY_H
#define COMPASS_NAME_CONSISTENCY_H

namespace CompassAnalyses
   { 
     namespace NameConsistency
        { 
        /*! \brief Name Consistency: Add your description here 
         */
          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

#if USE_ROSE_BOOST_WAVE_SUPPORT 

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
                    Sg_File_Info* file_info;
                    SgNode* IRnode;
                    PreprocessingInfo* preproc;

                    std::string regex;
                    std::string regex_name;


               public:
                    CheckerOutput(SgNode* n, std::string rname, std::string r);
                    CheckerOutput(PreprocessingInfo* p, std::string rname, std::string r);

                    virtual std::string getString() const;

             };
#endif
       // Specification of Checker Traversal Implementation

          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {

#if USE_ROSE_BOOST_WAVE_SUPPORT
            // Checker specific parameters should be allocated here.
                    std::string ruleFile;

                    NameEnforcer nm;
#endif

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }



// COMPASS_NAME_CONSISTENCY_H
#endif 

