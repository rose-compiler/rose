// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2:tw=80

// Set Pointers To Null Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 19-September-2007

#include "compass.h"
#include "setPointersToNull.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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
   
