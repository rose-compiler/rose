// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "fixupNullPointers.h"
// This file supports functionality to fixup NULL pointers in 
// the AST that we would like to not be NULL in a valid AST.

void fixupNullPointersInAST( SgNode* node )
   {
  // DQ (3/11/2006): Introduce tracking of performance of ROSE.
     TimingPerformance timer1 ("Fixup Null pointers:");

     FixupNullPointersInAST astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }


void
FixupNullPointersInAST::visit ( SgNode* node )
   {
  // DQ (3/11/2006): Set NULL pointers where we would like to have none.
  // printf ("In FixupNullPointersInAST::visit(): node = %s \n",node->class_name().c_str());

     ROSE_ASSERT(node != NULL);
     switch (node->variantT())
        {
       // This fixup was implemented because the inliner demonstrated how Qing's AST interface 
       // could be used to build an SgForStatement where4 many parts of the loop were left as 
       // NULL pointers.  All added IR nodes are marked as compiler generated.
          case V_SgForStatement:
             {
            // Note that the for loop's initializer is set internally by default, it is a list; 
            // and is allowed to be an empty list.  Thus no Null pointer value is require to indicate
            // that there are no variable present.
               SgForStatement* forStatement = isSgForStatement(node);
               ROSE_ASSERT(forStatement != NULL);
            // DQ (3/16/2006): Avoid leaving a null pointer to the SgForStatement's
            // SgForInitStatement (an empty list is OK).
               if (forStatement->get_for_init_stmt() == NULL)
                  {
#if 1
                    SgForInitStatement* forInitStatement = new SgForInitStatement(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
                    ROSE_ASSERT(forInitStatement != NULL);
                    forStatement->set_for_init_stmt(forInitStatement);
                    forInitStatement->set_parent(forStatement);

#if 0
                 // DQ (3/17/2006): Add at least one statement so that the CFG will work (test), we would like to not require this!
                    SgNullStatement* nullStatement = new SgNullStatement(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
                    forInitStatement->append_init_stmt(nullStatement);
                    nullStatement->set_parent(forInitStatement);
#endif
#else
                    printf ("FixupNullPointersInAST::visit() -- case SgForStatement: Skipping fixup of NULL pointer to SgForInitStatement \n");
#endif
                  }
            // Avoid leaving a null pointer for the for loop's test statement (note the test 
            // is not an expression in C or C++).
               if (forStatement->get_test() == NULL)
                  {
                    SgNullStatement* nullStatement = new SgNullStatement(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
                    ROSE_ASSERT(nullStatement != NULL);
                    forStatement->set_test(nullStatement);
                    nullStatement->set_parent(forStatement);
                  }

            // Avoid leaving a null pointer for the for loop's increment expression.
               if (forStatement->get_increment() == NULL)
                  {
                 // SgExpressionRoot* expressionRoot = 
                 //      new SgExpressionRoot(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode(),
                 //                           new SgNullExpression(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode()));
                 // ROSE_ASSERT(expressionRoot != NULL);
                 // forStatement->set_increment_expr_root(expressionRoot);
                 // expressionRoot->set_parent(forStatement);
                    SgExpression* expression = new SgNullExpression(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
                    ROSE_ASSERT(expression != NULL);
                    forStatement->set_increment(expression);
                    expression->set_parent(forStatement);
                  }
               break;
             }

#if 0
       // DQ (3/25/2006): This can no longer be a NULL pointer, since it is now a C++ string internally.
          case V_SgStringVal:
             {
               SgStringVal* stringVal = isSgStringVal(node);
               if (stringVal->get_value() == NULL)
                  {
                    printf ("Fixing Up SgStringVal string pointer \n");
                    stringVal->set_value("");

                 // DQ (3/23/2006): This only appears to be a problem because the 
                 // AST Copy mechanism had a bug, so trap this out for now while 
                 // we fix that bug more directly.
                    ROSE_ASSERT(false);
                  }
               break;
             }
#endif

       // It is perfectly acceptable to not handle many specific IR node and allow
       // this code to evolve as a way to tighten up the design of the AST over time.
          default:
             {
            // printf ("Not handled in FixupNullPointersInAST::visit(%s) \n",node->class_name().c_str());
             }
        }

  // DQ (10/27/2007): Added as a test! (Trying to debug the outliner)
     ROSE_ASSERT(node != NULL);
     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
#if 0
          if (isSgExprStatement(locatedNode) != NULL)
               printf ("Testing start and end of construct Sg_File_Info objects for locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
#endif
          if (locatedNode->get_startOfConstruct() == NULL)
               printf ("Warning: startOfConstruct not set: Testing end of construct Sg_File_Info objects for locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());

          ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
#if 1
       // DQ (10/27/2007): Make this a warning for now since it appears to be a common problem
          if (locatedNode->get_endOfConstruct() == NULL)
               printf ("Warning: endOfConstruct not set: Testing end of construct Sg_File_Info objects for locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
#else
          ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);
#endif
        }

   }

