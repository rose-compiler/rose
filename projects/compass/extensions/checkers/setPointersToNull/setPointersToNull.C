// Set Pointers To Null
// Author: Mark Lewandowski, -422-3849
// Date: 19-September-2007

#include "compass.h"

#ifndef COMPASS_SET_POINTERS_TO_NULL_H
#define COMPASS_SET_POINTERS_TO_NULL_H

namespace CompassAnalyses
   { 
     namespace SetPointersToNull
        { 
        /*! \brief Set Pointers To Null: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public AstSimpleProcessing
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, postorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_SET_POINTERS_TO_NULL_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2:tw=80

// Set Pointers To Null Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 19-September-2007

#include "compass.h"
// #include "setPointersToNull.h"

namespace CompassAnalyses
   { 
     namespace SetPointersToNull
        { 
          const std::string checkerName      = "SetPointersToNull";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Set all dynamically allocated pointers to NULL after calling free().";
          const std::string longDescription  = "A simple yet effective way to avoid double-free and access-freed-memory vulnerabilities is to set pointers to NULL after they have been freed. Calling free() on a NULL pointer results in no action being taken by free(). Thus, it is recommended that freed pointers be set to NULL to help eliminate memory related vulnerabilities.";
        } //End of namespace SetPointersToNull.
   } //End of namespace CompassAnalyses.

CompassAnalyses::SetPointersToNull::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::SetPointersToNull::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["SetPointersToNull.YourParameter"]);


   }

void
CompassAnalyses::SetPointersToNull::Traversal::
visit(SgNode* node)
   { 
     SgFunctionCallExp* func = isSgFunctionCallExp(node);
     SgAssignOp* aop = isSgAssignOp(node);
     SgBasicBlock* bb = isSgBasicBlock(node);

     /* If node is not one of the above types, return right away. */
     if (!func && !aop && !bb) return;
     
     static std::list<std::pair<SgInitializedName*, SgNode*> > var_node_pairs;
     std::list<std::pair<SgInitializedName*, SgNode*> >::iterator it;

     /* Find all calls to free, and record the variable that it is called on and
      * the node that this occurs at. */
     if (func) {
       if( !isSgFunctionRefExp(func->get_function()) ) return;
       std::string func_name_str =
         isSgFunctionRefExp(func->get_function())->get_symbol()->get_name().getString();

       if(func_name_str.compare("free") != 0) return;

       SgCastExp* cast = isSgCastExp(func->get_args()->get_expressions().front());
       if(!cast) return;

       SgVarRefExp* var = isSgVarRefExp(cast->get_operand());
       if (!var) return;

       //Sg_File_Info* fi = var->get_file_info();
       //std::cout << "filename: " << ROSE::stripPathFromFileName(fi->get_filename()) << std::endl;

       var_node_pairs.push_back(std::pair<SgInitializedName*, SgNode*> (var->get_symbol()->get_declaration(), node));

       return;
     }

     /* Check assignment operations to see if a variable that has been freed has
      * been set to NULL. */
     if (aop) {
       /* Check to see if the right hand operand points to NULL.  If not we can
        * return imeadiatly. */
       SgExpression* rhop = aop->get_rhs_operand();
       if (isSgNullExpression(rhop)) {
         return;
       }
       
       SgVarRefExp* vr = isSgVarRefExp(aop->get_lhs_operand());
       if (!vr) return;
       
       SgInitializedName* lhop = vr->get_symbol()->get_declaration();
       
       for (it = var_node_pairs.begin(); it != var_node_pairs.end(); it++) {
         if (lhop == (*it).first) {
           it = var_node_pairs.erase(it);
           return;
         }
       }

       return;       
     }

     /* If the scope changes we should check to see if any variables have been
      * freed and not set to NULL.  If this has happened we add a CheckerOutput
      * object. */
     static SgScopeStatement* scope;
     if (bb) {
       if ((bb->get_scope() != scope) && (var_node_pairs.size())) {
         for (it = var_node_pairs.begin(); it != var_node_pairs.end(); it++) {
           output->addOutput(new CheckerOutput((*it).second));
         }
         var_node_pairs.clear();
       }
       
       scope = bb->get_scope();

       return;
     }

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::SetPointersToNull::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static AstSimpleProcessing* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::SetPointersToNull::Traversal(params, output);
}

extern const Compass::Checker* const setPointersToNullChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::SetPointersToNull::checkerName,
        CompassAnalyses::SetPointersToNull::shortDescription,
        CompassAnalyses::SetPointersToNull::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
