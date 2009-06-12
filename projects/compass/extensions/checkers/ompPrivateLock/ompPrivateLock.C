// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Omp Private Lock Analysis
// Author: Chunhua Liao
// Date: 10-June-2009

#include "rose.h"
#include "compass.h"

extern const Compass::Checker* const ompPrivateLockChecker;

// DQ (1/17/2009): Added declaration to match external defined in file:
// rose/projects/compass/extensions/prerequisites/ProjectPrerequisite.h
// I can't tell that it is defined anywhere in compass except the extern 
// declaration in ProjectPrerequisite.h
//Compass::ProjectPrerequisite Compass::projectPrerequisite;

namespace CompassAnalyses
   { 
     namespace OmpPrivateLock
        { 
        /*! \brief Omp Private Lock: Add your description here 
         */

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction 
             {
                    Compass::OutputObject* output;
            // Checker specific parameters should be allocated here.

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

CompassAnalyses::OmpPrivateLock::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,::ompPrivateLockChecker->checkerName,::ompPrivateLockChecker->shortDescription)
   {}

CompassAnalyses::OmpPrivateLock::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["OmpPrivateLock.YourParameter"]);


   }

void
CompassAnalyses::OmpPrivateLock::Traversal::
visit(SgNode* node)
{ 
  SgVarRefExp* n = isSgVarRefExp(node);
  if (n)
  {

    std::string type_name = n->get_type()->unparseToString();
    if (type_name=="omp_lock_t") // TODO there should be a better way for this
    {
      if (isSgAddressOfOp(n->get_parent()) && isSgFunctionCallExp(n->get_parent()->get_parent()->get_parent()))
      {
        SgFunctionSymbol *func_symbol=  isSgFunctionCallExp(n->get_parent()->get_parent()->get_parent())->getAssociatedFunctionSymbol();
        ROSE_ASSERT(func_symbol != NULL);
        std::string func_call_name = func_symbol->get_name().getString();
        if (func_call_name == "omp_unset_lock" ||func_call_name == "omp_set_lock" ||func_call_name == "omp_test_lock")
        {
          // only consider calls set/unset/test locks
          // get the lock declaration statement
          SgVariableDeclaration* lock_decl = isSgVariableDeclaration(n->get_symbol()->get_declaration()->get_declaration());
          ROSE_ASSERT(lock_decl);
          //get the enclosing parallel region statement
          SgOmpParallelStatement* lock_region = SageInterface::getEnclosingNode<SgOmpParallelStatement>(n);
          // orphaned case
          // local lock could be private, send warning
          if (lock_region == NULL)
          {
            if (!isSgGlobal(lock_decl->get_scope()))
            {
              //cerr<<"Found a possible private lock for a parallel region "<<endl;
              output->addOutput(new CheckerOutput(n));
            }
          }
          else // the lock reference cite is lexically within a parallel region
            // if the lock declaration is inside the region, send error message
          {
            if (SageInterface::isAncestor(lock_region, lock_decl))
            {
              output->addOutput(new CheckerOutput(n));
              //cerr<<"Found a reference to a private lock within a parallel region"<<endl;
            }
          }
        }
      } // function call
    } // type
  }// var Ref

} //End of the visit function.

// Checker main run function and metadata

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::OmpPrivateLock::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::OmpPrivateLock::Traversal(params, output);
}

extern const Compass::Checker* const ompPrivateLockChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        "OmpPrivateLock",
     // Descriptions should not include the newline character "\n".
        "Using a private lock within a parallel region has no effect.",
        "Using a private lock within a parallel region has no effect. Please make sure the lock is shared for the parallel region.",
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
   
