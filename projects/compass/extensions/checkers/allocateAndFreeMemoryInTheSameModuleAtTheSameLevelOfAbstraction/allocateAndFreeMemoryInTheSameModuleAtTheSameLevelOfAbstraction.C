// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Allocate And Free Memory In The Same Module At The Same Level Of Abstraction Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 16-November-2007

#include "compass.h"
#include "allocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction.h"

namespace CompassAnalyses
   { 
     namespace AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction
        { 
          const std::string checkerName      = "AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Allocating and freeing memory in different modules and levels of abstraction burdens the programmer with tracking the lifetime of that block of memory. This may cause confusion regarding when and if a block of memory has been allocated or freed, leading to programming defects such as double-free vulnerabilities, accessing freed memory, or writing to unallocated memory.";
          const std::string longDescription  = "Allocating and freeing memory in different modules and levels of abstraction burdens the programmer with tracking the lifetime of that block of memory. This may cause confusion regarding when and if a block of memory has been allocated or freed, leading to programming defects such as double-free vulnerabilities, accessing freed memory, or writing to unallocated memory.To avoid these situations, it is recommended that memory be allocated and freed at the same level of abstraction, and ideally in the same code module.The affects of not following this recommendation are best demonstrated by an actual vulnerability. Freeing memory in different modules resulted in a vulnerability in MIT Kerberos 5 MITKRB5-SA-2004-002 . The problem was that the MIT Kerberos 5 code contained error-handling logic, which freed memory allocated by the ASN.1 decoders if pointers to the allocated memory were non-NULL. However, if a detectable error occured, the ASN.1 decoders freed the memory that they had allocated. When some library functions received errors from the ASN.1 decoders, they also attempted to free, causing a double-free vulnerability.";
        } //End of namespace AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction.
   } //End of namespace CompassAnalyses.

CompassAnalyses::AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initialize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction.YourParameter"]);
   }

void
CompassAnalyses::AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction::Traversal::
visit(SgNode* node)
   { 
     SgFunctionCallExp* func  = isSgFunctionCallExp(node);
     SgAssignOp* aop          = isSgAssignOp(node);
     SgAssignInitializer* ain = isSgAssignInitializer(node);
     SgBasicBlock* bb         = isSgBasicBlock(node);

     /* If node is not one of the above types, return right away. */
     if (!func && !aop && !ain && !bb) return;

     static std::list< std::pair<SgInitializedName*, SgNode*> > var_node_pairs;
     std::list< std::pair<SgInitializedName*, SgNode*> >::iterator it;

     /* Find all calls to free, and record the variable that it is called on and
      * the node that this occurs at. */
     if (func) {
       //std::cout << "gets here 0\n";
       /*std::string func_name_str =
          isSgFunctionRefExp(func->get_function())->get_symbol()->get_name().getString();*/

       SgFunctionRefExp *fref = isSgFunctionRefExp(func->get_function());
       SgMemberFunctionRefExp *fmem = isSgMemberFunctionRefExp(func->get_function());

       //std::cout << "gets here 0a\n";
       std::string func_name_str("");
       if( fref ) func_name_str = fref->get_symbol()->get_name().getString();
       //std::cout << "gets here 0b\n";
       if( fmem ) func_name_str = fmem->get_symbol()->get_name().getString();
       //std::cout << "gets here 0c\n";
      
       if(func_name_str == "" || func_name_str.compare("free") != 0) return;
       
       //std::cout << "gets here 1\n";
       SgCastExp* cast = isSgCastExp(func->get_args()->get_expressions().front());
       if (!cast) return;

       //std::cout << "gets here 2\n";
       SgVarRefExp* var = isSgVarRefExp(cast->get_operand());
       if (!var) return;

       //var_node_pairs.push_back(std::pair<SgExpression*, SgNode*> (var, node));
       for (it = var_node_pairs.begin(); it != var_node_pairs.end(); it++) {
         /*if (var->unparseToString().compare((*it).first->unparseToString()) == 0) {*/
         if (var->get_symbol()->get_declaration() == (*it).first) {
           it = var_node_pairs.erase(it);
           return;
         }
       }

       /* If we reach this section of the code we know that we did not find a
        * corresponding call to malloc, and add a CheckerOutput object. */
       output->addOutput(new CheckerOutput(node));

       return;

     }


     /* Find all calls to malloc, and check to see if it has been freed in the
      * same scope.  If it has not, we add a CheckerOutput object.  If it has
      * we remove the reference to it from var_node_pairs. */
     if (aop) {
       /* Check to see if the right hand operand is a call to malloc.  If not we
        * can return immediatly. */
       SgCastExp* rhop_cast = isSgCastExp(aop->get_rhs_operand());
       if( !rhop_cast ) return;
       SgFunctionCallExp* rhop_func_call = isSgFunctionCallExp(rhop_cast->get_operand());
       if( !rhop_func_call ) return;
       SgFunctionRefExp* rhop_func = isSgFunctionRefExp(rhop_func_call->get_function());

       if( !rhop_func ) return;
       if (rhop_func->get_symbol()->get_name().getString().compare("malloc") == 0) {
         SgVarRefExp* lhop = isSgVarRefExp(aop->get_lhs_operand());

         if (lhop) {
           var_node_pairs.push_back(std::pair<SgInitializedName*, SgNode*> (lhop->get_symbol()->get_declaration(), node));
         }
       }
       

       return;
     }

     /* TODO: Come back to this */
     if (ain) {
       SgCastExp* cexp = isSgCastExp(ain->get_operand());
       SgFunctionCallExp* fcall = isSgFunctionCallExp(ain->get_operand());

       if (cexp) {
         fcall = isSgFunctionCallExp(cexp->get_operand());
         if (!fcall) {
        // std::cout << __FILE__ << "(" << __LINE__ << ")" << std::endl;
           return;
         }
       }

       if (fcall) {
         SgFunctionRefExp* func = isSgFunctionRefExp(fcall->get_function());
         if (!func) {
        // std::cout << __FILE__ << "(" << __LINE__ << ")" << std::endl;
           return;
         }
         
         if (func->get_symbol()->get_name().getString().find("malloc", 0) == std::string::npos) {
           return;
         }
       }

       SgInitializedName* in = isSgInitializedName(ain->get_parent());
       if (in) {
         var_node_pairs.push_back(std::pair<SgInitializedName*, SgNode*> (in, node));
       }
       return;
     }

     /* If the scope changes we should check to see if any variables have been
      * freed and not allocated in the same basic block.  If this has happened
      * we add a CheckerOutput object. */
     static SgScopeStatement* scope;
     if (bb) {
       if (isSgStatement(node->get_parent()) ||
           isSgBasicBlock(node->get_parent())) {
         return;
       }

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
   
