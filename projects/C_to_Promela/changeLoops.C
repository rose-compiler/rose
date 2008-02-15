#include "rose.h"
#include <list>
#include <iostream>
#include "tools.h"

using namespace std;

void changeForLoops(SgFunctionDefinition * fDef)
{
     // Find the for statements in the program
     list<SgNode*> forStatements = 
       NodeQuery::querySubTree(fDef, V_SgForStatement);

     // Iterate through them
     for (list<SgNode*>::const_iterator i = forStatements.begin();
          i != forStatements.end(); ++i) {

       // This code does the following transformation:
       // for(init; test; step) body;
       // -> {init; while(test) {body; label: step;}}
       // with continue statements in body changed to "goto step;"

       // Get the current for statement
       SgForStatement* forStatement = isSgForStatement(*i);
       ROSE_ASSERT (forStatement);

       // Create the outer block
       SgBasicBlock* outerBlock =
        new SgBasicBlock(Sg_File_Info::
                           generateDefaultFileInfoForTransformationNode());

       // Add in the initialization statements from the for loop
       SgStatementPtrList& initStatements = forStatement->get_init_stmt();
       for (SgStatementPtrList::const_iterator j = initStatements.begin();
            j != initStatements.end(); ++j) {
         outerBlock->append_statement(*j);
         (*j)->set_parent(outerBlock);
       }

       // Create the body of the while loop
       // The body has the body from the for loop, a label, and then the step
       // from the for loop
       // All continue statements from the body must be changed to point to the
       // new label

       // Create an empty block
       SgBasicBlock* whileBody =
         new SgBasicBlock(
               Sg_File_Info::generateDefaultFileInfoForTransformationNode());

       // Create a label where continue statements should point
       static int labelNameCounter = 0;
       ostringstream labelNameStream;
       labelNameStream << "continueLabel" << labelNameCounter++;
       string labelName = labelNameStream.str();
       SgLabelStatement* continueLabel =
         new SgLabelStatement(
               Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
               labelName);

       // Change the body of the for loop to point to the new label
       changeContinuesToGotos(forStatement->get_loop_body(),
                              new SgLabelSymbol(continueLabel));

       // Assemble the while body:
       // Add the (modified) body from the for loop
       whileBody->append_statement(forStatement->get_loop_body());
       forStatement->get_loop_body()->set_parent(whileBody);
       // Add the new label
       whileBody->append_statement(continueLabel);
       continueLabel->set_parent(whileBody);

       // Add the step from the for loop
       SgExprStatement* stepStatement =
         new SgExprStatement(
               Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
               forStatement->get_increment());
       forStatement->get_increment()->set_parent(stepStatement);
       whileBody->append_statement(stepStatement);
       stepStatement->set_parent(whileBody);

       // Create the inner while statement
       SgWhileStmt* whileStatement =
         new SgWhileStmt(
               Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
               forStatement->get_test(),
               whileBody);
       whileStatement->get_condition()->set_parent(whileStatement);
       whileBody->set_parent(whileStatement);

       // Append the while statement to the outer block
       outerBlock->append_statement(whileStatement);
       whileStatement->set_parent(outerBlock);

       // Replace the for statement with the new block
       SgStatement* forParent = isSgStatement(forStatement->get_parent());
       ROSE_ASSERT (forParent);
       forParent->replace_statement(forStatement, outerBlock);
     }

  // Generate source code from AST and call the vendor's compiler
}

