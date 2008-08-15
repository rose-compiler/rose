// Name Consistency
// Author: Andreas Saebjoernsen
// Date: 23-July-2007

#include "compass.h"


#include "checkNameImpl.h"

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
       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {

            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
                    std::string ruleFile;

                    NameEnforcer nm;

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

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Name Consistency Analysis
// Author: Andreas Saebjoernsen
// Date: 23-July-2007




#include "compass.h"
// #include "nameConsistency.h"


namespace CompassAnalyses
   { 
     namespace NameConsistency
        { 
          const std::string checkerName      = "NameConsistency";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Enforces naming conventions.";
          const std::string longDescription  = "Naming conventions are used to enforce how variables, functions and "
                                               "classes are named. This can make programs more maintainable.";
        } //End of namespace NameConsistency.
   } //End of namespace CompassAnalyses.



#include "checkNameImpl.C"


CompassAnalyses::NameConsistency::
CheckerOutput::CheckerOutput(SgNode* n, std::string rname, std::string r)
        : IRnode(n), regex_name(rname), regex(r),
          OutputViolationBase(n,checkerName,shortDescription)

      {
         ROSE_ASSERT(n!=NULL);
         file_info= n->get_file_info();
         ROSE_ASSERT(file_info!=NULL);
      }

CompassAnalyses::NameConsistency::
CheckerOutput::CheckerOutput(PreprocessingInfo* p, std::string rname, std::string r)
        : preproc(p), regex_name(rname), regex(r),
          OutputViolationBase(new SgLocatedNode(p->get_file_info()),checkerName,shortDescription)

      {
         ROSE_ASSERT(p!=NULL);
         file_info=p->get_file_info();

      }

std::string 
CompassAnalyses::NameConsistency::
CheckerOutput::getString() const {
        ROSE_ASSERT(file_info != NULL);
        std::string loc = Compass::formatStandardSourcePosition(file_info);

        std::string returnString =  loc + ": violates the "+ regex_name + " rule \"" + regex + "\"";
        if(preproc != NULL){
//           returnString = returnString + " : " + preproc->getString();
        }else if (IRnode != NULL){
           returnString = returnString + " : " + IRnode->unparseToString();
        }else
           exit(1); 

        return returnString;
   }




CompassAnalyses::NameConsistency::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
       // Initalize checker specific parameters here, for example: 
       ruleFile = Compass::parseString(inputParameters["NameConsistency.RulesFile"]);
       nm.readFile(ruleFile);
   }

void
CompassAnalyses::NameConsistency::Traversal::
visit(SgNode* node)
   { 
          //All violations to SgNode type rules
          std::list< std::pair<name_types,SgNode*> > violations;
          //All violations to rules for macros
          std::list< std::pair<name_types,PreprocessingInfo*> > macroViolations;
          if( (isSgLocatedNode(node) != NULL) &&
              (node->get_file_info() != NULL) &&
              (node->get_file_info()->isCompilerGenerated() == false) &&
              (node->get_file_info()->get_filenameString() != "compilerGenerated")
            ){

          //Call the function to enforce all rules found in the current rule file

          nm.enforceRules(node,violations, macroViolations);


          for( std::list< std::pair<name_types,SgNode*> >::iterator iItr = violations.begin();
               iItr != violations.end(); ++iItr  ){
                output->addOutput(new CheckerOutput(iItr->second, nm.get_enumName(iItr->first),nm.get_reg(iItr->first) ));
          }

          for( std::list< std::pair<name_types,PreprocessingInfo*> >::iterator iItr = macroViolations.end();
               iItr != macroViolations.end(); ++iItr ){
                output->addOutput(new CheckerOutput(iItr->second, nm.get_enumName(iItr->first),nm.get_reg(iItr->first) ));
          }
          }
   } //End of the visit function.



static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NameConsistency::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NameConsistency::Traversal(params, output);
}

extern const Compass::Checker* const nameConsistencyChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NameConsistency::checkerName,
        CompassAnalyses::NameConsistency::shortDescription,
        CompassAnalyses::NameConsistency::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
