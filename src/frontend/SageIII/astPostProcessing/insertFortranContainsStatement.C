// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "insertFortranContainsStatement.h"
void insertFortranContainsStatement (SgNode* node)
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("Fixup Fortran references:");

  // DQ (10/3/2008): This bug in OFP is now fixed so no fixup is required.
     printf ("Error: fixup of contains statement no longer required. \n");
     ROSE_ABORT();

     InsertFortranContainsStatement astFixupTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     astFixupTraversal.traverse(node,preorder);
   }


void
InsertFortranContainsStatement::visit ( SgNode* node )
   {
  // DQ (10/3/2008): This bug in OFP is now fixed so no fixup is required.
     printf ("Error: fixup of contains statement no longer required. \n");
     ROSE_ABORT();

  // DQ (11/24/2007): Output the current IR node for debugging the traversal of the Fortran AST.
     ROSE_ASSERT(node != NULL);
#if 0
     Sg_File_Info* fileInfo = node->get_file_info();

     printf ("node = %s fileInfo = %p \n",node->class_name().c_str(),fileInfo);
     if (fileInfo != NULL)
        {
          bool isCompilerGenerated = fileInfo->isCompilerGenerated();
          std::string filename = fileInfo->get_filenameString();
          int line_number = fileInfo->get_line();
          int column_number = fileInfo->get_line();

          printf ("--- isCompilerGenerated = %s position = %d:%d filename = %s \n",isCompilerGenerated ? "true" : "false",line_number,column_number,filename.c_str());
        }
#endif

     SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(node);

  // This is for handling where CONTAINS is required in a function
     if (functionDefinition != NULL)
        {
          SgBasicBlock* block = functionDefinition->get_body();
          SgStatementPtrList & statementList = block->get_statements();
          SgStatementPtrList::iterator i = statementList.begin();

          bool firstFunctionDeclaration = false;
          bool functionDeclarationSeen  = false;

          while (i != statementList.end() && firstFunctionDeclaration == false)
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);

            // DQ (1/20/2008): Note that entry statements should not cause introduction of a contains statement!
               if (isSgEntryStatement(functionDeclaration) != NULL)
                    functionDeclaration = NULL;

               if (functionDeclaration != NULL)
                  {
                    firstFunctionDeclaration = functionDeclarationSeen == false;
                    functionDeclarationSeen  = true;

                    if (firstFunctionDeclaration == true)
                       {
                      // Insert a CONTAINS statement.
                      // printf ("Building a contains statement (in function) \n");
                         SgContainsStatement* containsStatement = new SgContainsStatement();
                         SageInterface::setSourcePosition(containsStatement);
                         containsStatement->set_definingDeclaration(containsStatement);

                         block->get_statements().insert(i,containsStatement);
                         containsStatement->set_parent(block);
                         ROSE_ASSERT(containsStatement->get_parent() != NULL);
                       }
                  }

               i++;
             }
        }

#if 0
  // OFP now has better support for the CONTAINS statement so this code is not longer required.

  // The use of CONTAINS in modules appears to be handled by OFP, so no fixup is required.
     SgClassDefinition* classDefinition = isSgClassDefinition(node);

  // This is for handling where CONTAINS is required in a module
     if (classDefinition != NULL)
        {
          SgDeclarationStatementPtrList & statementList = classDefinition->get_members();
          SgDeclarationStatementPtrList::iterator i = statementList.begin();

          bool firstFunctionDeclaration = false;
          bool functionDeclarationSeen  = false;

          while (i != statementList.end() && firstFunctionDeclaration == false)
             {
               printf ("InsertFortranContainsStatement: *i in statementList in module = %p = %s \n",*i,(*i)->class_name().c_str());

               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
               if (functionDeclaration != NULL)
                  {
                    firstFunctionDeclaration = functionDeclarationSeen == false;
                    functionDeclarationSeen  = true;

                    if (firstFunctionDeclaration == true)
                       {
                      // Insert a CONTAINS statement.
                      // printf ("Building a contains statement (in module) \n");
                         SgContainsStatement* containsStatement = new SgContainsStatement();
                         SageInterface::setSourcePosition(containsStatement);
                         containsStatement->set_definingDeclaration(containsStatement);

                      // This insert function does not set the parent (unlike for SgBasicBlock)
                         classDefinition->get_members().insert(i,containsStatement);
                         containsStatement->set_parent(classDefinition);
                         ROSE_ASSERT(containsStatement->get_parent() != NULL);
                       }
                  }

               i++;
             }
        }
#endif
   }
