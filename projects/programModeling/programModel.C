#include "rose.h"

#include "programModel.h"

// Functions required by the tree traversal mechanism
ProgramModelAttributeCollectorInheritedAttribute
ProgramModelAttributeCollectorTraversal::evaluateInheritedAttribute (
     SgNode* astNode,
     ProgramModelAttributeCollectorInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("!!!!! In evaluateInheritedAttribute() \n");
     printf ("     (traversalNodeCounter=%d) astNode->sage_class_name() = %s \n",
          traversalNodeCounter,astNode->sage_class_name());
     printf ("     astNode->unparseToString() = %s \n",astNode->unparseToString().c_str());
#endif

#if 0
     vector<string> successorNames = astNode->get_traversalSuccessorNamesContainer();
     printf ("     #of successors = %zu successorNames = ",successorNames.size());
     for (vector<string>::iterator s = successorNames.begin(); s != successorNames.end(); s++)
        {
           printf ("%s ",(*s).c_str());
        }
     printf ("\n");
#endif

     switch(astNode->variantT())
        {
          default:
             {
            // g++ needs a block here
             }
        }

     return inheritedAttribute;
   }

ProgramModelAttributeCollectorSynthesizedAttribute
ProgramModelAttributeCollectorTraversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     ProgramModelAttributeCollectorInheritedAttribute inheritedAttribute,
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     ProgramModelAttributeCollectorSynthesizedAttribute returnAttribute;

     switch(astNode->variantT())
        {
          case V_SgForStatement:
             {
            // printf ("Case SgForStatement: Memory access summaries computed symbolically (total static memory accesses = %d) \n",returnAttribute.numberOfAccesses);
#if 0
               printf ("Case SgForStatement: call Maple interface to generate symbolic equation, initializer = %d, condition = %d, increment = %d, body = %d (static total = %d) \n",
                    synthesizedAttributeList[SgForStatement_for_init_stmt].numberOfAccesses,
                    synthesizedAttributeList[SgForStatement_test_expr_root].numberOfAccesses,
                    synthesizedAttributeList[SgForStatement_increment_expr_root].numberOfAccesses,
                    synthesizedAttributeList[SgForStatement_loop_body].numberOfAccesses,
                    returnAttribute.numberOfAccesses);
#endif
            // Now we call the Maple interface to generate a symbolic representation of the equation 



               break;
             }

          default:
             {
            // g++ needs a block here
             }
        }

     return returnAttribute;
   }


// This is placed into a namespace
void
ProgramModeling::computeModelEquations ( SgProject* project )
   {
     printf ("Inside of ProgramModeling::computeModelEquations() \n");

     ProgramModelAttributeCollectorInheritedAttribute inheritedAttribute;

     ProgramModelAttributeCollectorTraversal traversal;
     traversal.traverseInputFiles(project,inheritedAttribute);

     printf ("Leaving ProgramModeling::computeModelEquations() \n");
   }

