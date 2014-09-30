
// We need this header file so that we can use the mechanisms within ROSE to build a preprocessor
#include "rose.h"

// Temp code placed here while we debug it

#if 0
SgStatementPtrList*
extractRangeOfStatementsBetweenDeclarations (
   SgStatementPtrList & statementList,
   string startingMarkerVariableName,
   string endingMarkerVariableName,
   int classificationCode )
   {
  // This function supports the stripWrapper function to extract the statements representing a
  // transformation from the AST representing the program build from the transformation of the
  // specification.  This function extracts the statements located between integer declarations of
  // variables named by markerVariableName.

     SgStatementPtrList* returnStatmentListPtr = new SgStatementPtrList;
     ROSE_ASSERT (returnStatmentListPtr != NULL);

#if 0
     printf ("In extractRangeOfStatementsBetweenDeclarations(): statementList.size() = %d startingMarkerVariableName = %s \n",
          statementList.size(),startingMarkerVariableName.c_str());
#endif

  // iterator for use on statements
     SgStatementPtrList::iterator s_iter = statementList.begin();

  // Look for declarations in the global scope (denoted by coded surrounding variable declarations)
  // SgVariableDeclaration* variableDeclarationStatement = isSgVariableDeclaration(*s_iter);
  // if ( variableDeclarationStatement != NULL )
     while ( s_iter != statementList.end() )
        {

       // Look for declarations in the local scope (denoted by coded surrounding variable declarations)
          SgVariableDeclaration* variableDeclarationStatement = isSgVariableDeclaration(*s_iter);
          if ( variableDeclarationStatement != NULL )
             {
            // Check for "int START_OF_TRANSFORMATION_SOURCE_CODE;" within the AST
               ROSE_ASSERT (variableDeclarationStatement != NULL);
               ROSE_ASSERT (variableDeclarationStatement->get_variables().size() > 0);
            // ROSE_ASSERT (variableDeclarationStatement->get_variables().begin() != NULL);
            // ROSE_ASSERT (variableDeclarationStatement->get_variables().begin()->get_name().str() != NULL);
            // char* variableNameString = variableDeclarationStatement->get_variables().begin()->get_name().str();
               ROSE_ASSERT ( (*(variableDeclarationStatement->get_variables().begin())).get_name().str() != NULL);
               string startingVariableNameString = (*(variableDeclarationStatement->get_variables().begin())).get_name().str();

            // printf ("STARTING MARKER SEARCH: Variablename = %s \n",startingVariableNameString.c_str());

            // Look for the marker that indicates the start of the declarations that represent transformations
               if ( startingVariableNameString == startingMarkerVariableName )
                  {
                 // save all the declarations until we reach "int END_OF_TRANSFORMATION_SOURCE_CODE;"

                 // printf ("Found the STARTING MARKER \n");

                    s_iter++;
                    SgStatementPtrList::iterator start_iter = s_iter;
                    SgStatementPtrList::iterator end_iter   = s_iter;

                    bool closingMarkerFound = false;
                    while (closingMarkerFound == false )
                       {
                      // Look for the "int END_OF_TRANSFORMATION_SOURCE_CODE;" variable declaration

                      // printf ("Value of closingMarkerFound = %s \n",(closingMarkerFound == true) ? "true" : "false");
                         SgVariableDeclaration* nextVariableDeclarationStatement = isSgVariableDeclaration(*s_iter);
                         if ( nextVariableDeclarationStatement != NULL )
                            {
                           // Check for "int END_OF_TRANSFORMATION_SOURCE_CODE;" within the AST
                              ROSE_ASSERT (nextVariableDeclarationStatement != NULL);
                              ROSE_ASSERT (nextVariableDeclarationStatement->get_variables().size() > 0);
//                            ROSE_ASSERT (nextVariableDeclarationStatement->get_variables().begin() != NULL);
                           // ROSE_ASSERT (nextVariableDeclarationStatement->get_variables().begin()->get_name().str() != NULL);
                           // char* variableNameString = nextVariableDeclarationStatement->get_variables().begin()->get_name().str();
                              ROSE_ASSERT ( (*(nextVariableDeclarationStatement->get_variables().begin())).get_name().str() != NULL);
                              string closingVariableNameString = (*(nextVariableDeclarationStatement->get_variables().begin())).get_name().str();

                           // printf ("LOOKING FOR ENDING MARKER SEARCH: Variablename = %s \n",closingVariableNameString.c_str());

                           // Look for the marker that indicates the end of the declarations that represent transformations
                              if ( closingVariableNameString == endingMarkerVariableName )
                                 {
                                   closingMarkerFound = true;
                                // printf ("FOUND the ending declaration \n");
                                 }
                                else
                                 {
                                // printf ("Not a matching ending declaration (looking for %s) \n",endingMarkerVariableName.c_str());
                                   end_iter = s_iter;
                                   s_iter++;
                                 }
                            }
                           else
                            {
                           // Not an ending declaration
                           // printf ("Not a declaration statement \n");
                              end_iter = s_iter;
                              s_iter++;
                            }

                      // ROSE_ASSERT (start_iter != end_iter);
                       }

                 // increment the end_iter so that we can build the for loop over the proper iteration space
                 // (for loops have to have a conditional tests such as "transformationStatementIterator != end_iter")
                    end_iter++;
                    ROSE_ASSERT (start_iter != end_iter);

                 // extract out the declarations between the sarting and ending markers
                    SgStatementPtrList::iterator transformationStatementIterator = start_iter;
                    for (transformationStatementIterator  = start_iter;
                         transformationStatementIterator != end_iter;
                         transformationStatementIterator++)
                       {
                      // Putting each statement of the transformation specification into the returnTransformationBlock
#if 0
                         printf ("(*transformationStatementIterator)->sage_class_name() = %s \n",
                              (*transformationStatementIterator)->sage_class_name());
#endif
                      // Mark the file_info object in the statement so that it is classified
                         ROSE_ASSERT ((*transformationStatementIterator) != NULL);
                         ROSE_ASSERT ((*transformationStatementIterator)->get_file_info() != NULL);
                         (*transformationStatementIterator)->get_file_info()->set_classificationBitField(classificationCode);

                      // do we have to copy this first? (it should not be required)
                      // returnTransformationBlock->append_statement(*transformationStatementIterator);
                         returnStatmentListPtr->push_back(*transformationStatementIterator);
                       }

                    ROSE_ASSERT ( returnStatmentListPtr->size() > 0 );
                  }
                 else
                  {
                 // Not a starting declaration
                 // printf ("Not a starting declaration \n");
                  }
             }
            else
             {
            // Not a starting declaration
            // printf ("Outer iteration has not found a declaration (s_iter++) \n");
             }

       // printf ("Increment iterator (s_iter++) \n");
          s_iter++;
        }

#if 0
  // debuging code
     printf ("returnTransformationBlock->get_statements().size() = %d \n",
          returnTransformationBlock->get_statements().size());
     SgStatementPtrList::iterator transformationStatementIterator;
     for (transformationStatementIterator = returnTransformationBlock->get_statements().begin();
          transformationStatementIterator != returnTransformationBlock->get_statements().end();
          transformationStatementIterator++)
        {
          printf ("(*transformationStatementIterator)->sage_class_name() = %s \n",
               (*transformationStatementIterator)->sage_class_name());
        }
#endif

     ROSE_ASSERT (returnStatmentListPtr != NULL);
     return returnStatmentListPtr;
   }
#endif

#if 0
class TransformationFragmentAST
   {
  // This class forms a container for different types of statements representing a transformation
  // there maybe more types of statements (static declarations, etc.), but we can add more lists
  // if required. Or derive new classes from this class if required.

     public:
          SgStatementPtrList declarationStatementList;
          SgStatementPtrList initializationStatementList;
          SgStatementPtrList transformationStatementList;

          void setDeclarationStatementList ( const SgStatementPtrList & X );
          SgStatementPtrList getDeclarationStatementList ();

          void setInitializationStatementList ( const SgStatementPtrList & X );
          SgStatementPtrList getInitializationStatementList ();

          void setTransformationStatementList ( const SgStatementPtrList & X );
          SgStatementPtrList getTransformationStatementList ();
   };
#endif

#if 0
SgStatementPtrList*
SgNode::stripAwayWrapping ( SgFile & file, bool isADeclaration )
   {
  // This function disasembles the AST represented by the SgFile for a transformation and gets out
  // the parts that are relevant to the transformation. The list of statements that is returned is
  // marked (each statement is marked) as either: declaration, initialization, or transformation.
  // If (isADeclaration == true) then only code between the DECLARATION_MARKERS is extracted, else
  // code between the INITIALIZATION_MARKERS and the TRANSFORMATION_MARKERS is extracted.

#if 0
     printf ("Strip away the wrapper from the transformation's AST (isADeclaration = %s) \n",
          (isADeclaration) ? "true" : "false");
#endif

  // name of the file containing the transformation source code
     char* inputFileName = "rose_transformation.C";

  // We start by getting the SgScopeStatement and the using its iterator
  // to go through the statements in that scope.
     SgScopeStatement *globalScope = (SgScopeStatement *)(&(file.root()));
     ROSE_ASSERT (globalScope != NULL);
     ROSE_ASSERT (isSgScopeStatement(globalScope) != NULL);

     Sg_File_Info* fileInfo = new Sg_File_Info(inputFileName,0,0);
     ROSE_ASSERT (fileInfo != NULL);

     SgStatementPtrList* returnStatmentListPtr = new SgStatementPtrList;
     ROSE_ASSERT (returnStatmentListPtr != NULL);

  // TransformationFragmentAST* returnTransformationAST = new SgStatementPtrList;
  // ROSE_ASSERT (returnTransformationAST != NULL);

  // Loop though the declaration statements in the global scope (to find the appropriate function)
     SgDeclarationStatement::iterator s_iter = globalScope->begin();
     while ( s_iter != globalScope->end() )
        {
       // Check if the statement from the original file (not the include files)
       // We are only interested in transforming statements in the input file, later
       // we can broaden our reach to functions included from other header files etc.

          ROSE_ASSERT ((*s_iter) != NULL);
          ROSE_ASSERT ((*s_iter)->get_file_info() != NULL);
       // printf ("In TransformationSpecificationType::stripAwayWrapping(): inputFileName            = %s \n",inputFileName);
       // printf ("In TransformationSpecificationType::stripAwayWrapping(): (*s_iter)->getFileName() = %s \n",(*s_iter)->getFileName());

       // restrict our focus to the pieces of the AST associated with the current file
          if (isSameName(inputFileName, (*s_iter)->getFileName()) == true)
             {
            // printf ("In stripAwayWrapping(): (*s_iter)->sage_class_name()  = %s \n",(*s_iter)->sage_class_name());

            // Look for the function containing the transformation statements and extract 
            // the statements representing the transformation specification (so that they 
            // can be inserted into the returnTransformationBlock).
               SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration(*s_iter);
               if ( functionDeclarationStatement != NULL )
                  {
                 // check the name of the function (to make sure it is the correct name)
                 // if this is the correct function then get the function body and copy
                 // the statements out of the AST and into the returnTransformationBlock
                 // printf ("function name containing the transformation FOUND! \n");

                    ROSE_ASSERT (functionDeclarationStatement != NULL);
                    SgName sageName = functionDeclarationStatement->get_name();
                    string functionNameString = sageName.str();
                 // printf ("Transformation hidden in function name = %s \n",functionNameString.c_str());

                 // The containing function is given a specific name that we can recognize
                 // if (isSameName(functionNameString, "transformationContainerFunction") == true)
                    if ( functionNameString == "transformationContainerFunction" )
                       {
                         ROSE_ASSERT (functionDeclarationStatement->get_definition() != NULL);
                         SgFunctionDefinition *functionDefinitionStatement = 
                              isSgFunctionDefinition (functionDeclarationStatement->get_definition());
                         ROSE_ASSERT (functionDeclarationStatement != NULL);
                         ROSE_ASSERT (functionDefinitionStatement->get_body() != NULL);
                         SgBasicBlock *inputTransformationBlock = functionDefinitionStatement->get_body();
                         ROSE_ASSERT (inputTransformationBlock != NULL);

                         SgStatementPtrList::iterator transformationStatementIterator;

                      // Build a copy of the list so that we can remove the last
                      // statement (return 0;) and not change the original AST
                         SgStatementPtrList copyOfStatementList = inputTransformationBlock->get_statements();

                      // Remove the "return 0;" at the end of the list (we know it is there since we built the function)
                         copyOfStatementList.pop_back();

                         SgStatementPtrList* declarationStatmentList    = NULL;
                         SgStatementPtrList* initializationStatmentList = NULL;
                         SgStatementPtrList* transformationStatmentList = NULL;

                         if (isADeclaration == true)
                            {
                           // printf ("Processing VARIABLE_DECLARATION_MARKER \n");
                              declarationStatmentList =
                                   extractRangeOfStatementsBetweenDeclarations (
                                        copyOfStatementList,
                                        "VARIABLE_DECLARATIONS_MARKER_START",
                                        "VARIABLE_DECLARATIONS_MARKER_END",
                                        Sg_File_Info::e_declaration );
                              ROSE_ASSERT (declarationStatmentList != NULL);
                              ROSE_ASSERT (declarationStatmentList->size() > 0);

                              printf ("declarationStatmentList->size() = %" PRIuPTR " \n",declarationStatmentList->size());
                              returnStatmentListPtr->insert (returnStatmentListPtr->end(),
                                                        declarationStatmentList->begin(),
                                                        declarationStatmentList->end());
                           // printf ("returnStatmentListPtr->size() = %" PRIuPTR " \n",returnStatmentListPtr->size());
                            }
                           else
                            {
                           // printf ("Processing VARIABLE_INITIALIZATION_MARKER \n");
                              initializationStatmentList =
                                   extractRangeOfStatementsBetweenDeclarations (
                                        copyOfStatementList,
                                        "VARIABLE_INITIALIZATION_MARKER_START",
                                        "VARIABLE_INITIALIZATION_MARKER_END",
                                        Sg_File_Info::e_initialization );

                              returnStatmentListPtr->insert (returnStatmentListPtr->end(),
                                                        initializationStatmentList->begin(),
                                                        initializationStatmentList->end());

                           // printf ("returnStatmentListPtr->size() = %" PRIuPTR " \n",returnStatmentListPtr->size());
                           // printf ("Processing TRANSFORMATION_MARKER \n");

                              transformationStatmentList =
                                   extractRangeOfStatementsBetweenDeclarations (
                                        copyOfStatementList,
                                        "TRANSFORMATION_MARKER_START",
                                        "TRANSFORMATION_MARKER_END",
                                        Sg_File_Info::e_transformation );

                              returnStatmentListPtr->insert (returnStatmentListPtr->end(),
                                                        transformationStatmentList->begin(),
                                                        transformationStatmentList->end());
                           // printf ("returnStatmentListPtr->size() = %" PRIuPTR " \n",returnStatmentListPtr->size());
                            }
#if 0
                         printf ("Exiting after extraction of declaration, initialization, and transformation statements! \n");
                         ROSE_ABORT();
#endif
                       }
                  }
                 else
                  {
                 // printf ("function name containing the transformation has not been found! \n");
                  }

             } // end of filename test

       // increment to the next declaration statement (in the global scope)
          s_iter++;

        } // end of "while" loop

  // printf ("At base of SgNode::stripAwayWrapping ( SgFile & file, bool isADeclaration )  (returnStatmentListPtr->size() = %d) \n",
  //      returnStatmentListPtr->size());

  // Only in trival cases could the size be equal to zero (e.g. user adding "" as source code)
     ROSE_ASSERT ( returnStatmentListPtr->size() > 0 );

  // ROSE_ABORT();

     ROSE_ASSERT (returnStatmentListPtr != NULL);
     return returnStatmentListPtr;
   }
#endif








