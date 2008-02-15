// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Boolean Is Has Analysis
// Author: pants,,,
// Date: 24-July-2007

#include "compass.h"
#include "booleanIsHas.h"

namespace CompassAnalyses
   { 
     namespace BooleanIsHas
        { 
          const std::string checkerName      = "BooleanIsHas";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding boolean variables or boolean returning functions that do not begin with 'is_' or 'has_'";
          const std::string longDescription  = "Finds all boolean variables or boolean returning functions whose name does not begin with 'is_' or 'has_'";
        } //End of namespace BooleanIsHas.
   } //End of namespace CompassAnalyses.

CompassAnalyses::BooleanIsHas::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::BooleanIsHas::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["BooleanIsHas.YourParameter"]);


   }

void
CompassAnalyses::BooleanIsHas::Traversal::
visit(SgNode* node)
   {
        SgFunctionDeclaration* fn = isSgFunctionDeclaration(node);
        SgVariableDeclaration* var = isSgVariableDeclaration(node);
        if (!fn and !var) return;
        SgType *t;
        std::string thingName;
        if (fn)
          {
            thingName = fn->get_name().str();
            SgFunctionType* s = fn->get_type();
            t = s->get_return_type();
          }
        else
          { 
            SgInitializedNamePtrList vars =  var->get_variables();
            if (!vars.empty())
              {
                //I cut out the iterator loop because it seemed to
                //never be used.  Yell at me if this is wrong... why
                //else would you return a list?
                SgInitializedNamePtrList::iterator j = vars.begin();
                SgInitializedName* initName = isSgInitializedName (*j);
                if(!initName) {return;}
                t = initName->get_type();
                thingName  = initName->get_qualified_name().str();
              }
          }
        if (isSgTypeBool(t) and not (thingName.rfind("is_", 0) != std::string::npos or thingName.rfind("has_", 0) != std::string::npos))
          {
          output->addOutput(new CheckerOutput(node));
          }
   }
   
