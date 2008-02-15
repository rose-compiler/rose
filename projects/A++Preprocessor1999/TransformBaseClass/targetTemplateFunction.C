// Include rose.h so we can manipulate ROSE objects to recognize and do the transformations
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rose.h>
#include "TransformationBase.h"

// Include the defintion of this class
// Each transformation source file must include the header file for
// that transformation class definition (declaration?)
// include "transformation_1.h"

ROSE_TransformationBase::TargetTemplate::~TargetTemplate ()
   {
  // Nothing to delete here
   }

ROSE_TransformationBase::TargetTemplate::TargetTemplate ( SgStatement *targetFunctionStatement )
   {
  // Constructor for nested class

     ROSE_ASSERT (targetFunctionStatement != NULL);

  // showSgStatement (cout,targetFunctionStatement);

  // is a SgMemberFunctionDeclaration
     SgMemberFunctionDeclaration *memberFunctionDeclaration = isSgMemberFunctionDeclaration(targetFunctionStatement);
     ROSE_ASSERT (memberFunctionDeclaration != NULL);
     SgFunctionDefinition *functionDefinition = memberFunctionDeclaration->get_definition();
     ROSE_ASSERT (functionDefinition != NULL);
     SgFunctionDeclaration* functionDeclarationStatement = functionDefinition->get_declaration();
     ROSE_ASSERT (functionDeclarationStatement != NULL);

  // Test that the name of the function is "target" (to verify that we have the right function)
     SgName sageName = functionDeclarationStatement->get_name();
     const char* nameString = sageName.str();
  // cout << "In SimpleArrayAssignment::TargetTemplate::TargetTemplate(SgStatement): Function Name = " << nameString << endl;
     if (!ROSE::isSameName (nameString,"target"))
        {
          printf ("ERROR: Function Name representing target template is incorrect (nameString=%s) should be 'target' \n",nameString);
          ROSE_ABORT();
        }

  // Now go get the SgBasicBlock (the function body from the function declaration)
     ROSE_ASSERT (functionDeclarationStatement->get_definition() != NULL);
     SgFunctionDefinition *functionDefinitionStatement = 
          isSgFunctionDefinition (functionDeclarationStatement->get_definition());
     SgBasicBlock *basicBlock = functionDefinitionStatement->get_body();
     if (basicBlock != NULL)
        {
          Boolean collectStatementsToRepresentTarget = FALSE;
          ROSE_ASSERT (basicBlock != NULL);
          SgStatementPtrList::iterator statementIterator;
          for (statementIterator = basicBlock->get_statements().begin(); 
               statementIterator != basicBlock->get_statements().end();
               statementIterator++)
             {
            // Get the SgStatement from the SgStatementPrt using the irep() member function
#ifdef USE_SAGE3
               SgStatement *blockStmt = *statementIterator;
#else
               SgStatement *blockStmt = (*statementIterator).irep();
#endif
               ROSE_ASSERT (blockStmt != NULL);

            // showSgStatement (cout, blockStmt, 10);

            // Look for a pragma within each statement, until we find the 
            // "rose transformTarget on" pragma.  Then make a list of the 
            // statements until we reach the "rose transformTarget off" pragma.
               SgPragma *pragma = ROSE::getPragma (blockStmt);
               if (pragma != NULL)
                  {
                    char* pragmaString = pragma->get_name();

                    ROSE_ASSERT (pragmaString != NULL);
                    printf ("strlen(pragmaString) = %d \n",strlen(pragmaString));
                    if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                         cout << "In TargetTemplate constructor: pragmaString = " << pragmaString << endl;

                 // The assertions in the statements below make sure that "on" preceeds "off"
                    if (ROSE::isSameName (pragmaString,"rose transformTarget on"))
                       {
                         ROSE_ASSERT (collectStatementsToRepresentTarget == FALSE);
                         collectStatementsToRepresentTarget = TRUE;
                       }
		    
                    if (ROSE::isSameName (pragmaString,"rose transformTarget off"))
                       {
                         ROSE_ASSERT (collectStatementsToRepresentTarget == TRUE);
                         collectStatementsToRepresentTarget = FALSE;
                       }
                  }
                 else
                  {
                 // We don't have to print anything out in this case
                 // cout << "In TargetTemplate constructor: pragmaString is NULL" << endl;
                  }

               if (collectStatementsToRepresentTarget == TRUE)
                  {
                 // Now place the statements between the pragmas into a list representing the target transfromation
                    if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                         printf ("Collect this Statement! \n");

                    targetStatementList.push_back (blockStmt);
                  }
             }

          if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
               printf ("targetStatementList.size() = %d \n",targetStatementList.size());
        }
       else
        {
          if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
	       printf ("ERROR: (basicBlock == NULL) (function body definition not available) \n");
	}

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("TARGET TEMPLATE SETUP! \n");
   }





