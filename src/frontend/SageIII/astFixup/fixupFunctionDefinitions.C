
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupFunctionDefinitions.h"
void fixupFunctionDefinitions( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup function definitions - missing body:");

  // This simplifies how the traversal is called!
     FixupFunctionDefinitions astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }

void
FixupFunctionDefinitions::visit ( SgNode* node )
   {
  // DQ (6/24/2005): Fixup of function definitions that are missing a body

  // printf ("node = %s \n",node->sage_class_name());

     switch (node->variantT())
        {
          case V_SgFunctionDefinition:
             {
            // DQ (8/9/2005): Test for existence of body in function definition
               SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(node);
               if (functionDefinition->get_body() == NULL)
                  {
#if PRINT_DEVELOPER_WARNINGS
                    printf ("AST Fixup: function definition violates the rule that every function definition has a function body, building an empty SgBasicBlock \n");
                 // functionDefinition->get_file_info()->display("debug");
#endif
                    SgBasicBlock* body = new SgBasicBlock(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
                    ROSE_ASSERT(body != NULL);
                    body->set_endOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
                    functionDefinition->set_body(body);
                 // this is required since the access function don't have side-effects on their parameters
                    body->set_parent(functionDefinition);

                    ROSE_ASSERT(body->get_parent() == functionDefinition);
                  }
               ROSE_ASSERT(functionDefinition->get_body() != NULL);
               break;
             }
          default:
             {
            // no other cases appear to be a problem!
             }
        }
   }


