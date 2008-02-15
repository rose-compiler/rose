// Include rose.h so we can manipulate ROSE objects to recognize and do the transformations
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rose.h>
#include "TransformationBase.h"
#include "supported_dimensions.h"

// Include the defintion of this class
// Each transformation source file must include the header file for
// that transformation class definition (declaration?)
// include "transformation_1.h"

ROSE_TransformationBase::TransformationTemplate::~TransformationTemplate ()
   {
  // Nothing to delete here
   }

ROSE_TransformationBase::TransformationTemplate::TransformationTemplate ( SgStatement *transformationFunctionStatement )
   {
  // Constructor for nested class

     ROSE_ASSERT (transformationFunctionStatement != NULL);

  // showSgStatement (cout,transformationFunctionStatement);

  // is a SgMemberFunctionDeclaration
     SgMemberFunctionDeclaration *memberFunctionDeclaration = isSgMemberFunctionDeclaration(transformationFunctionStatement);
     ROSE_ASSERT (memberFunctionDeclaration != NULL);
     SgFunctionDefinition *functionDefinition = memberFunctionDeclaration->get_definition();
     ROSE_ASSERT (functionDefinition != NULL);
     SgFunctionDeclaration* functionDeclarationStatement = functionDefinition->get_declaration();
     ROSE_ASSERT (functionDeclarationStatement != NULL);

  // Test that the name of the function is "target" (to verify that we have the right function)
     SgName sageName = functionDeclarationStatement->get_name();
     const char* nameString = sageName.str();
  // cout << "In SimpleArrayAssignment::TargetTemplate::TargetTemplate(SgStatement): Function Name = " << nameString << endl;
     if (!ROSE::isSameName (nameString,"transformationTemplate"))
        {
          printf ("ERROR: Function Name representing transformation template is incorrect (nameString=%s) should be 'transformationTemplate' \n",nameString);
          ROSE_ABORT();
        }

  // Now go get the SgBasicBlock (the function body from the function declaration)
     ROSE_ASSERT (functionDeclarationStatement->get_definition() != NULL);
     SgFunctionDefinition *functionDefinitionStatement = 
          isSgFunctionDefinition (functionDeclarationStatement->get_definition());
     SgBasicBlock *basicBlock = functionDefinitionStatement->get_body();
     if (basicBlock != NULL)
        {
       // Collect the statements representing the different parts of the transformation so they
       // can be applied in different ways in the transformation process
          collectTransformationTemplateStatements          (basicBlock);
          collectUniqueTransformationTemplateStatements    (basicBlock);
          collectLhsTransformationTemplateStatements       (basicBlock);
          collectRhsTransformationTemplateStatements       (basicBlock);
          collectOuterLoopTransformationTemplateStatements (basicBlock);
       // collectInnerLoopTransformationTemplateStatements (basicBlock);
        }
       else
        {
	  printf ("ERROR: (basicBlock == NULL) (function body definition not available) \n");
          ROSE_ABORT();
	}

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("TRANSFORMATION TEMPLATE SETUP! \n");
   }

void
ROSE_TransformationBase::TransformationTemplate::
collectStatementsFromTransformationBlock ( 
     SgBasicBlock *basicBlock, char* targetStringStart, 
     char* targetStringEnd, SgStatementPtrList & stmtList )
   {
     Boolean collectStatements = FALSE;
     ROSE_ASSERT (basicBlock != NULL);

     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("COLLECT STATEMENTS BETWEEN PRAGMAS %s (on/off) \n",targetStringStart);

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

#if 0
          if (ROSE::getLineNumber(blockStmt) == 82)
               showSgStatement (cout, blockStmt, 10);
#endif

       // Look for a pragma within each statement, until we find the 
       // "rose transformTarget on" pragma.  Then make a list of the 
       // statements until we reach the "rose transformTarget off" pragma.
       // This function collects the entire transformation that 
       // will be reproduced into the users application code
          SgPragma *pragma = ROSE::getPragma (blockStmt);


          if (pragma != NULL)
             {
               char* pragmaString = pragma->get_name();
            // cout << "In collectStatementsFromTransformationBlock: pragmaString = " << pragmaString << endl;

            // The assertions in the statements below make sure that "on" preceeds "off"
               if (ROSE::isSameName (pragmaString,targetStringStart))
                  {
                    ROSE_ASSERT (collectStatements == FALSE);
                    collectStatements = TRUE;

                    if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                         cout << "In collectStatementsFromTransformationBlock: FOUND (START) pragmaString = " << pragmaString << endl;
                  }

               if (ROSE::isSameName (pragmaString,targetStringEnd))
                  {
                    if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
                         cout << "In collectStatementsFromTransformationBlock: FOUND (END) pragmaString = " << pragmaString << endl;

                    ROSE_ASSERT (collectStatements == TRUE);
                    collectStatements = FALSE;
                  }
             }
            else
             {
            // We don't have to print anything out in this case
            // cout << "In TargetTemplate constructor: pragmaString is NULL" << endl;
             }

          if (collectStatements == TRUE)
             {
            // Now place the statements between the pragmas into a list representing the target transfromation
	    // printf ("Collect this Statement! \n");
               stmtList.push_back (blockStmt);
             }
        }

  // Each pass should have collected something, check and make sure!
  // printf ("stmtList.size() = %d \n",stmtList.size());
     ROSE_ASSERT (stmtList.size() > 0);
   }


void
ROSE_TransformationBase::TransformationTemplate::
collectTransformationTemplateStatements ( SgBasicBlock *blockStmt )
   {
  // This function collects the entire transformation that 
  // will be reproduced into the users application code
     collectStatementsFromTransformationBlock (
          blockStmt,"rose transformTemplate on",
          "rose transformTemplate off",
          transformationStatementList);
   }

void
ROSE_TransformationBase::TransformationTemplate::
collectUniqueTransformationTemplateStatements    ( SgBasicBlock *blockStmt )
   {
  // This function collects the entire transformation that 
  // will be reproduced into the users application code
     collectStatementsFromTransformationBlock (
          blockStmt,"rose transformTemplate uniqueCode on",
          "rose transformTemplate uniqueCode off",
          uniqueStatementList);
   }

void
ROSE_TransformationBase::TransformationTemplate::
collectLhsTransformationTemplateStatements       ( SgBasicBlock *blockStmt )
   {
  // This function collects the entire transformation that 
  // will be reproduced into the users application code
     collectStatementsFromTransformationBlock (
          blockStmt,"rose transformTemplate LhsOperandSpecificCode on",
          "rose transformTemplate LhsOperandSpecificCode off",
          lhsStatementList);

// Now we have to initialize the pointers into the program tree for each statement
// where a variable name appears so that we can change the variable name

   }

void
ROSE_TransformationBase::TransformationTemplate::
collectRhsTransformationTemplateStatements       ( SgBasicBlock *blockStmt )
   {
  // This function collects the entire transformation that 
  // will be reproduced into the users application code
     collectStatementsFromTransformationBlock (
          blockStmt,"rose transformTemplate operandSpecificCode on",
          "rose transformTemplate operandSpecificCode off",
          rhsStatementList);
   }

// CW: This function collects the code pieces for the loop structure of the
// transformated code from the ROSE_TRANSFORMATION files.
// blockStmt is the part of the SAGE tree which represents the basic block
// where the code pieces can be found.
void
ROSE_TransformationBase::TransformationTemplate::
collectOuterLoopTransformationTemplateStatements ( SgBasicBlock *blockStmt )
{
	// CW: buffer size must be large enougth to keep the pragma
	char buffer1[100];
	char buffer2[100];

	for(int i=0;i<ROSE_DISTINGUISED_NO_OF_DIMENSIONS;i++)
	{
		// CW: check buffer size
		ROSE_ASSERT(strlen("rose transformTemplate loopConstructionCode_xxD off")<100);
		
		// CW: generation on and off pragma for current dimension to be able
		// to collect code pieces from ROSE_TRANSFORMATION_3.h
		sprintf(buffer1,"%s%u%s","rose transformTemplate loopConstructionCode_",i+1,"D on");
		sprintf(buffer2,"%s%u%s","rose transformTemplate loopConstructionCode_",i+1,"D off");

		// CW: collect code piece for loop structure
		collectStatementsFromTransformationBlock(
			blockStmt, buffer1, buffer2, outerLoopStatementList[i]);	
	};

	// CW: if we have less code pieces than the amount of supported dimension
	// then we handle those dimensions as special cases of the highest supported dimension
	// Consequently ROSE_MAX_ARRAY_DIMENSION+1 code pieces must be provided in
	// ROSE_TRANSFORMATION_3.h. One of them must be for the highest supported dimension.
	if(ROSE_MAX_ARRAY_DIMENSION>ROSE_DISTINGUISED_NO_OF_DIMENSIONS){
		// CW: generation on and off pragma for maximal dimension to be able
		// to collect code pieces from ROSE_TRANSFORMATION_3.h
		sprintf(buffer1,"%s%u%s","rose transformTemplate loopConstructionCode_",ROSE_MAX_ARRAY_DIMENSION,"D on");
		sprintf(buffer2,"%s%u%s","rose transformTemplate loopConstructionCode_",ROSE_MAX_ARRAY_DIMENSION,"D off");

		// CW: collect code piece for loop structure
		collectStatementsFromTransformationBlock(
			blockStmt, buffer1, buffer2, outerLoopStatementList[ROSE_MAX_ARRAY_DIMENSION-1]);
	
		for(int j=ROSE_DISTINGUISED_NO_OF_DIMENSIONS;j<ROSE_MAX_ARRAY_DIMENSION-1;j++)
		{
			// CW: the other dimensions are handled as special case of the maximum
			// dimension.
			outerLoopStatementList[j]=outerLoopStatementList[ROSE_MAX_ARRAY_DIMENSION-1];
		}
	}
}

void
ROSE_TransformationBase::TransformationTemplate::
collectInnerLoopTransformationTemplateStatements ( SgBasicBlock *blockStmt )
   {
  // This function collects the entire transformation that 
  // will be reproduced into the users application code
     collectStatementsFromTransformationBlock (
          blockStmt,"rose transformTemplate innerloop on",
          "rose transformTemplate innerloop off",
          innerLoopStatementList);
   }

