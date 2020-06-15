#include "sage3basic.h"
#include "fixupFileInfoFlags.h"

// #include "stringify.h"

using namespace Rose;

// documented in header file
size_t
fixupFileInfoInconsistanties(SgNode *ast)
   {
  // DQ (11/14/2015): Added support to fixup the Sg_File_Info objects where they are detected to be inconsistant.
  // This consistancy test is a precursor to possibily moving the API for specification of transformation to
  // the SgLocatedNode since it is redundant and error prown to have it in each of multiple Sg_File_Info objects.
  // Note that the OpenMP transformations are at least one place where this is not set consistantly, so it is
  // more helpful to fix it to be consistant and issue a warning as a part of the process of moving the API
  // and communicating the deprication of the API in the Sg_File_Info over that in the SgLocatedNode.  Most
  // other transformations mechanism now use the API in the SgLocatedNode (but this test will help test that).

  // Note that there may be other flags in the Sg_File_Info objects that shuld be consistant (these can be 
  // added to this fixup function).  Other flags to test include:
  //    isCompilerGenerated
  //    isOutputInCodeGeneration
  //    isShared
  //    isFrontendSpecific
  //    isSourcePositionUnavailableInFrontend
  //    isCommentOrDirective
  //    isToken
  //    isDefaultArgument
  //    isImplicitCast
  // 
  // Or a less precise mechanism might be to just set the classificationBitField.
  // Note also that not all of these have been or should be moved to the SgLocatedNode API (though this is 
  // a subject up for discussion).

     struct T1: public AstSimpleProcessing 
        {
          size_t nviolations;
          T1(): nviolations(0) {}

          void visit(SgNode *node)
             {
               SgLocatedNode *located = isSgLocatedNode(node);
               if (located)
                  {
                 // This test is only looking at the consistancy of the setting of transforamtions across all
                 // of the Sg_File_Info objects in a SgLocatedNode (and the extra one in a SgExpression).

                    bool result = located->get_startOfConstruct()->isTransformation();

                    ROSE_ASSERT(located->get_startOfConstruct() != NULL);
                    if (located->get_endOfConstruct() != NULL)
                       {
#if 0
                         printf ("NOTE: located node = %p = %s testing: located->get_startOfConstruct()->isTransformation() != located->get_endOfConstruct()->isTransformation() \n",located,located->class_name().c_str());
#endif
                         if (result != located->get_endOfConstruct()->isTransformation())
                            {
                              if (result == true)
                                   located->get_endOfConstruct()->setTransformation();
                                else
                                   located->get_endOfConstruct()->unsetTransformation();

                              printf ("WARNING: In fixupFileInfoInconsistanties(): located = %p = %s testing: get_endOfConstruct()->isTransformation() inconsistantly set (set to match startOfConstruct) \n",located,located->class_name().c_str());
                              located->get_startOfConstruct()->display("fixupFileInfoInconsistanties()");
                            }
                         ROSE_ASSERT(located->get_startOfConstruct()->isTransformation() == located->get_endOfConstruct()->isTransformation());
                       }
                      else
                       {
                         printf ("WARNING: In fixupFileInfoInconsistanties(): located = %p = %s testing: get_endOfConstruct() != NULL (failed) \n",located,located->class_name().c_str());
                         located->get_startOfConstruct()->display("fixupFileInfoInconsistanties()");
                       }

                    const SgExpression* expression = isSgExpression(located);
                    if (expression != NULL && expression->get_operatorPosition() != NULL)
                       {
#if 0
                         printf ("NOTE: expression = %p = %s testing: result != expression->get_operatorPosition()->isTransformation() \n",located,located->class_name().c_str());
#endif
                         if (result != expression->get_operatorPosition()->isTransformation())
                            {
                              if (result == true)
                                   expression->get_operatorPosition()->setTransformation();
                                else
                                   expression->get_operatorPosition()->unsetTransformation();

                              printf ("WARNING: In fixupFileInfoInconsistanties(): expression located = %p = %s testing: get_operatorPosition()->isTransformation() inconsistantly set (set to match startOfConstruct) \n",expression,expression->class_name().c_str());
                              expression->get_startOfConstruct()->display("fixupFileInfoInconsistanties()");
                            }
                         ROSE_ASSERT(expression->get_startOfConstruct()->isTransformation() == expression->get_operatorPosition()->isTransformation());
                       }
                  }
             }

        } t1;

     t1.traverse(ast, preorder);
     return t1.nviolations;
   }

                
