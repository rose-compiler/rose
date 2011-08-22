#ifndef AST_REWRITE_MID_LEVEL_INTERFACE_TEMPLATES_C
#define AST_REWRITE_MID_LEVEL_INTERFACE_TEMPLATES_C

// #include "rose.h"
#include "rewrite.h"
#include <vector>
#include <string>

#undef PRINT_DEBUGGING_INFO
#ifdef _MSC_VER
#define PRINT_DEBUGGING_INFO 0
#else
#define PRINT_DEBUGGING_INFO false
#endif

#if 0
template <class ASTNodeCollection>
SgStatement*
MidLevelRewrite<ASTNodeCollection>::previousPrefixRelevantStatement ( 
   SgStatement* currentStatement )
   {
     SgStatement* returnStatement = ROSE::getPreviousStatement(currentStatement);
     switch (returnStatement->variantT())
        {
          case SgFunctionDefinition:
             break;
          case SgFunctionDeclaration:
             break;
          case SgBasicBlock:
             break;
          default:

        }

     return returnStatement;
   }
#endif

template <class ASTNodeCollection>
bool
MidLevelRewrite<ASTNodeCollection>::acceptableTargetForPrefix ( SgNode* astNode )
   {
  // This function limits the locations where a prefix can be generated from.
  //    1) The location must be a statement
  //    2) Since we start with the previous statement, there must exist a previous 
  //       statement and this is not defined in the case of a SgGlobal statement 
  //       (global scope).

     bool returnValue = false;

     if ( (isSgStatement(astNode) != NULL) && (isSgGlobal(astNode) == NULL) )
          returnValue = true;

     return returnValue;
   }

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::generatePrefixAndSuffix ( 
     SgNode* astNode, 
     std::string & prefixString, 
     std::string & suffixString,
     bool generateIncludeDirectives,
     bool prefixIncludesCurrentStatement )
   {
  // This function generates a prefix suitable for use in compiling new code specified
  // as a string from any location in a program where the string shold compile if edited 
  // into place manually by the user. The actions involved traversing from the current 
  // position in the AST (astNode) to the root of the AST (SgProject, or SgFile, or SgGlobal).
  // A few details are observed to make this happen:
  //      1) all declarations are recorded (this is fairly easy)
  //      2) all typedefs are preserved (this is not done currently, but should be fairly easy)
  //      3) the current scopes are preserved (this is complex and being debugged currently)

  // Currently we only compute the prefix from a statement (this can be generalized to expressions later).
  // if ( (isSgStatement(astNode) != NULL) )
     if ( acceptableTargetForPrefix(astNode) == true )
        {
#if PRINT_DEBUGGING_INFO
          printf ("\n\n*************************************************************\n");
          printf ("In generatePrefixAndSuffixStrings: astNode is a %s \n",astNode->sage_class_name());
          printf ("In generatePrefixAndSuffixStrings: astNode = %s \n",astNode->unparseToString().c_str());
#endif

#if 0
       // DQ (2/1/2004): This is no longer needed since we no longer use the visit flags in the AST
       // Part of a temporary fix to explicitly clear all the flags
          AstClearVisitFlags cleanTreeFlags;
#endif

       // Retrieve the SgProject node from any location in the AST
          SgProject* project = TransformationSupport::getProject(astNode);
          ROSE_ASSERT (project != NULL);

#if 0
       // DQ (2/1/2004): This is no longer needed since we no longer use the visit flags in the AST
          printf ("Temporary Fix: Clearing internal visit flags in MidLevelRewrite::generatePrefixAndSuffix() \n");
          cleanTreeFlags.traverse(project);
#endif

       // build the traversal object (algorithm)
          PrefixInheritedAttribute inheritedAttribute;
          PrefixSuffixGenerationTraversal traversal (generateIncludeDirectives);

       // printf ("Build prefix starting at MODIFIED input position relative to astNode! \n");

       // Get the current statement (from any astNode, this makes it easy to generalize 
       // this function to expressions later).
          SgStatement* currentStatement = TransformationSupport::getStatement(astNode);
          ROSE_ASSERT (currentStatement != NULL);
          SgNode* startingStatementForPrefixGeneration = astNode;

#if PRINT_DEBUGGING_INFO
          printf ("currentStatement = %s \n",currentStatement->sage_class_name());
          printf ("currentStatement = %s \n",currentStatement->unparseToString().c_str());
#endif

       // Record the current statment so that we can determine if the previous 
       // statement (if it is sought) is in the same scope or not.
       // traversal.previousScope = isSgStatement(currentStatement->get_parent());
          traversal.previousScope = currentStatement->get_scope();
          ROSE_ASSERT (traversal.previousScope != NULL);

#if PRINT_DEBUGGING_INFO
          printf ("traversal.previousScope = %s \n",traversal.previousScope->sage_class_name());
          printf ("traversal.previousScope = %s \n",traversal.previousScope->unparseToString().c_str());
#endif

          bool addedOpeningAndClosingBrace = false;

       // If the current statement is a declaration statement then we want to avoid
       // having it appear in the prefix. So look for the previous statement.
          if (prefixIncludesCurrentStatement == false)
             {
            // Avoid building a prefix that should not include the current statement 
            // (e.g. for test codes that replace the current statement with itself 
            // (which can happen in reality as well)).
            // printf ("Looking for previous statement of currentStatement = %s \n",currentStatement->sage_class_name());
               SgStatement* startingLocation = ROSE::getPreviousStatement(currentStatement);
               ROSE_ASSERT (startingLocation != NULL);

            // Handle special cases where we want to start a little further toward the root in the AST
               switch (startingLocation->variantT())
                  {
                 // case V_SgClassDeclaration:
                    case V_SgFunctionDeclaration:
                    case V_SgMemberFunctionDeclaration:
                       {
                      // printf ("Handling special case of SgClassDeclaration or SgFunctionDeclaration \n");
                         startingLocation = ROSE::getPreviousStatement(startingLocation);
                         break;
                       }
                    default:
                       {
                      // printf ("default case of special handling! \n");
                         break;
                       }
                  }

               startingStatementForPrefixGeneration = startingLocation;
               ROSE_ASSERT (startingStatementForPrefixGeneration != NULL);

            // If including the current statement then for specific IR node add an opening brace "{" and closing brace "}"
               printf ("startingStatementForPrefixGeneration = %s \n",startingStatementForPrefixGeneration->sage_class_name());
               switch (startingStatementForPrefixGeneration->variantT())
                  {
                 // special cases
                    case V_SgClassDefinition:
                      // printf ("special case: add opening and closing braces \n");
                      // addedOpeningAndClosingBrace = true;
                         break;
                    default:
                       {
                      // ignore these cases
                       }
                  }
               printf ("addedOpeningAndClosingBrace = %s \n",addedOpeningAndClosingBrace ? "true" : "false");
             }

       // The first statement in global scope has no previous statement so we have no prefix!
          if (startingStatementForPrefixGeneration != NULL)
             {
#if PRINT_DEBUGGING_INFO
               printf ("startingStatementForPrefixGeneration = %s \n",startingStatementForPrefixGeneration->sage_class_name());
               printf ("startingStatementForPrefixGeneration = %s \n",startingStatementForPrefixGeneration->unparseToString().c_str());
#endif
            // printf ("########### PREFIX TRAVERSAL: START ############################ \n");
               traversal.traverse(startingStatementForPrefixGeneration,inheritedAttribute);
            // printf ("########### PREFIX TRAVERSAL: END ############################## \n\n\n");

#if 0
            // DQ (2/1/2004): This is no longer needed since we no longer use the visit flags in the AST
               printf ("Temporary Fix: Clearing internal visit flags in MidLevelRewrite::generatePrefixAndSuffix() \n");
               cleanTreeFlags.traverse(project);
#endif

               prefixString = traversal.generatePrefixString();
               suffixString = traversal.generateSuffixString();

               if (addedOpeningAndClosingBrace == true)
                  {
                    prefixString += "\n /* opening brace */ { \n";
                    suffixString += "\n /* closing brace */ } \n";
                  }
             }
            else
             {
            // error case
#if 0
               ROSE_ASSERT ( currentStatement != NULL );
               ROSE_ASSERT ( currentStatement->get_parent() != NULL );
               ROSE_ASSERT ( isSgGlobal(currentStatement->get_parent()) != NULL );
               printf ("In MidLevelRewrite<ASTNodeCollection>::generatePrefixAndSuffix(): startingStatementForPrefixGeneration == NULL \n");
               printf ("currentStatement = %s \n",currentStatement->sage_class_name());
               printf ("currentStatement->get_parent() = %s \n",currentStatement->get_parent()->sage_class_name());
#endif
            // abort
               ROSE_ASSERT (false);
             }

#if PRINT_DEBUGGING_INFO
          printf ("Leaving generatePrefixAndSuffixStrings: astNode is a %s \n",astNode->sage_class_name());
          printf ("************************************************************* \n\n");
#endif
        }
       else
        {
#if 0
          printf ("ERROR: generatePrefixAndSuffixStrings() only works on SgStatements for now (this can be fixed later) \n");
          ROSE_ABORT();
#endif

       // Can't generate a prefix for these statements, but make sure we return empty strings.
          prefixString = "";
          suffixString = "";
        }

#if 0
     printf ("In MidLevelRewrite<ASTNodeCollection>::generatePrefixAndSuffix() prefixString = \n%s\n",prefixString.c_str());
     printf ("In MidLevelRewrite<ASTNodeCollection>::generatePrefixAndSuffix() suffixString = \n%s\n",suffixString.c_str());
#endif

#if 0
     printf ("Exiting in MidLevelRewrite<ASTNodeCollection>::generatePrefixAndSuffix() \n");
     ROSE_ABORT();
#endif
   }

#if 0
// DQ (8/7/2011): Test where this is required since it leads to linking errors when ROSE is configured using "--enable-static --disable-shared"
// multiple definition of `MidLevelRewrite<MidLevelInterfaceNodeCollection>::replace(SgStatement*, std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)'
template <>
void
MidLevelRewrite<MidLevelInterfaceNodeCollection>::replace ( SgStatement* target, const std::string & transformationString )
   {
  // Semantics of replace is always insert and remove (but it can't always be implemented that 
  // way e.g. for loop initializer statements).  It is simpler to use the replace option within
  // the insert command, then lower level mechanisms take care of the statement removal and 
  // attached comments/directives are processed properly.
  // insert(target,transformationString);
  // remove(target);
     insert(target,transformationString,MidLevelInterfaceNodeCollection::SurroundingScope,MidLevelInterfaceNodeCollection::ReplaceCurrentPosition);
   }
#endif

template <class ASTNodeCollection>
void
MidLevelRewrite<ASTNodeCollection>::remove  ( SgStatement* target )
   {
  // This function will move any attached comments/directives to the surrounding statements
     LowLevelRewrite::remove(target);
   }

// This function might be able to be moved from here to the Interface collection class.
// We should investigate that after I have more of the mid-level and high-level interface working.
template <class ASTNodeCollection>
typename ASTNodeCollection::NodeCollectionType
MidLevelRewrite<ASTNodeCollection>::fileStringToNodeCollection ( 
     SgNode* astNode, 
     std::string transformationString )
   {
  // This function takes the AST representing the compilation of the transformation strings for a
  // given scope (or subset of a scope or specific transformation) and separates the statements and
  // expressions into the ASTFragmentCollection object.

     ROSE_ASSERT (astNode!= NULL);

  // The project node is required so that we can recover the original 
  // commandline required to compile the intermediate file
  // SgProject* project = ProjectQueryTraversal::getProject(astNode);
     SgProject* project = TransformationSupport::getProject(astNode);
     ROSE_ASSERT (project != NULL);

     std::string finalSourceCodeString = transformationString;

  // printf ("Inside of MidLevelRewrite<ASTNodeCollection>::fileStringToNodeCollection \n");
  // printf ("#####\nfinalString = \n%s\n#####\n",finalSourceCodeString.c_str());

  // Get the filename without path or suffix so that the intermediate file can be given 
  // a name that will make it unique to the compilation/transformation support of the 
  // input file and with a number so that the independent transformation will not collide 
  // and we can simplify the debugging of the transformations.
  // SgFile* currentFile = TransformationSupport::getFile(astNode);
     SgSourceFile* currentFile = TransformationSupport::getSourceFile(astNode);
     ROSE_ASSERT (currentFile != NULL);
  // string currentFileNameWithSuffix = ROSE::stripPathFromFileName(currentFile->getFileName());
  // printf ("currentFileNameWithSuffix = %s \n",currentFileNameWithSuffix.c_str());
  // string currentFileName = ROSE::stripFileSuffixFromFileName(currentFileNameWithSuffix.c_str());
  // printf ("currentFileName = %s \n",currentFileName.c_str());
  // std::string currentFileName = ROSE::stripFileSuffixFromFileName(ROSE::stripPathFromFileName(currentFile->getFileName()));
     std::string currentFileName = StringUtility::stripFileSuffixFromFileName(StringUtility::stripPathFromFileName(currentFile->getFileName()));

  // Make the file name different each time a new set of transformation strings are compiled
     static int fileNumber = 1;
     std::string numberString   = StringUtility::numberToString(fileNumber++);
     std::string fileNameString = "rose_transformation_" + currentFileName + numberString + ".C";

  // For now - write the string representing the file containing the strings representing the
  // transformation out to a file Later we can try to avoid the intermediate file generation (but
  // for now this helps us debug the whole transformation specification mechanism).
     StringUtility::writeFile(
          /* string containing transformation strings */ finalSourceCodeString.c_str(),
       // /* filename */ "rose_transformation.C",
          /* filename */ fileNameString.c_str(),
          /* directory */ "./" );

  // Now generate the AST representing the transformation (for substitution)
  // The problem with sending each transformation through the compiler
  // infrastructure is that it could be slow.  Short specialized header files
  // might be helpful in reducing the compile time associated with this approach.
  // We might want to just use a SgFile instead of a SgProject!
     int errorCode = 0;

  // Now we want to get the command line out of the project object (instead of a static variable)
  // This allows there to be more than on project in the same executable.
  // DQ (10/16/2005): Modified this to track changes in SgProject
     std::vector<std::string> project_argv = project->get_originalCommandLineArgumentList();
     ROSE_ASSERT (project_argv.size() > 1);

     std::vector<std::string> transformation_argv = SgNode::buildCommandLineToSubstituteTransformationFile (project_argv,fileNameString);

  // We need the lifetime to be extended beyond that of this function (to avoid purify error)
  // SgFile transformationAST (transformation_argc,transformation_argv,errorCode);
     //AS(10/04/08) Because it is hard to get all the initializations correct without doing determine file type we now require the
     //calling of the constructor directly to create the AST
  //   SgSourceFile* transformationASTPointer = new SgSourceFile (transformation_argv,errorCode, 0, project);
     SgSourceFile* transformationASTPointer = isSgSourceFile(determineFileType(transformation_argv, errorCode, project));

     ROSE_ASSERT (transformationASTPointer != NULL);
     ROSE_ASSERT (errorCode <= 2);

  // GB (9/4/2009): Construction of a SgFile using determineFileType() no
  // longer calls the AST fixups. So we need to call them here, before the
  // AST is returned to user code.
     AstPostProcessing(transformationASTPointer);

  // printf ("DONE: Calling SgProject constructor \n");

#if 0
     std::string pdffilename = std::string("./") + std::string(ROSE::stripPathFromFileName(ROSE::getFileName(transformationASTPointer)))+".pdf";
  // printf ("Expected PDF file name = %s \n",pdffilename.c_str());

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     AstPDFGeneration pdftest;
     pdftest.generateWithinFile(transformationASTPointer);

  // printf ("Exiting to test PDF file generation \n");
  // ROSE_ABORT();
#endif

#if 0
     std::string dotfilename=string("./") + std::string(ROSE::stripPathFromFileName(ROSE::getFileName(transformationASTPointer)))+".dot";
  // printf ("Expected DOT file name = %s \n",dotfilename.c_str());

  // test ROSE specific class
     AstDOTGeneration astdotgen;
     astdotgen.generateInputFiles(project,DOTGeneration<SgNode*>::TOPDOWNBOTTOMUP,dotfilename);
#endif

  // At this point the transformation has been placed into the global scope and
  // we have to extract it into a form more useful to use (ready to substitute)
  // Ultimately we require more generality than this. The stripAwayWrapping() 
  // function is a member function of the base class (TransformationSpecificationType).

#if 0
     printf ("Get the AST fragement representing the transformation out of the new AST! exiting ... \n");
     ROSE_ABORT();
#endif

  // Use the MidLevelRewrite's traversal mechanism to recognize
  // the AST fragements imbedded in the intermediate file's AST
     AST_FragmentIdentificationInheritedAttributeType inheritedValue;
     AST_FragmentIdentificationSynthesizedAttributeType synthesizedAttribute;
     AST_FragmentIdentificationTraversal treeTraversal;

#if 1
  // Call the tree traversal
     synthesizedAttribute = treeTraversal.traverse(transformationASTPointer,inheritedValue);
#else
  // AST_FragmentIdentificationSynthesizedAttributeType synthesizedAttribute;
     printf ("ERROR: Must call tree traversal! \n");
     ROSE_ABORT();
#endif

  // synthesizedAttribute.display("After traversal");

#if 0
     printf ("synthesizedAttribute.treeFragementListArray[GlobalScopeTopOfScope].size() = %d \n",
          synthesizedAttribute.treeFragementListArray[GlobalScopeTopOfScope].size());
     list<SgStatement*>::iterator statementIterator;
     for (statementIterator = synthesizedAttribute.treeFragementListArray[GlobalScopeTopOfScope].begin();
          statementIterator != synthesizedAttribute.treeFragementListArray[GlobalScopeTopOfScope].end();
          statementIterator++)
        {
           printf ("     unparse statement = %s \n",(*statementIterator)->unparseToString().c_str());
        }
#endif

  // Things to assert during the debugging
     synthesizedAttribute.consistancyCheck("Called from ASTFragmentContainer() constructor!");

  // Now copy the pointers to the tree fragments from the synthesizedAttribute to the
  // ASTFragmentArray. The completes the generation of the AST tree fragements from their
  // specification via strings.
     NodeCollectionType dataCollection = synthesizedAttribute.treeFragementListArray;


  // Error checking
     bool notAnEmptyContainer = false;
     for (unsigned int i=0; i < dataCollection.size(); i++)
        {
          if (dataCollection[i].size() > 0)
               notAnEmptyContainer = true;
        }

    
     std::cout << "AST NODE " << astNode->class_name() << std::endl;
     
     ROSE_ASSERT (notAnEmptyContainer == true || isSgUnknownFile(astNode) != NULL );

     return dataCollection;
   }


// Alternative names:
//   string postponeMacroExpansion(string)
//   string wrapMarcoCall(string)
//   string doNotEvaluateMacro(string)
template <class ASTNodeCollection>
std::string
MidLevelRewrite<ASTNodeCollection>::postponeMacroExpansion ( std::string macroCall )
   {
  // Generate a declaration that can be compiled in the intermediate code (but not expanded) 
  // and trapped by the unparser and transformed back to a macro invocation.

  // Use a counter value to allow repeated invocations to be defined uniquely 
  // (macro invocation must be hidden in unique variable declarations)
     static int counter = 0;

  // std::string prefix = "char* rose_macro_declaration_" + StringUtility::numberToString(counter) + " = \"ROSE-TRANSFORMATION-MACRO:";
     std::string prefix = "char* rose_macro_declaration_" + StringUtility::numberToString(counter) + " = \"ROSE-MACRO-CALL:";
     std::string suffix = "\";";

  // Fix quoted strings to escape all quotes (escaped quotes are unescaped in
  // the unparser (the other end))
  // string returnString = prefix + macroCall + suffix;
     std::string processedInputString = StringUtility::copyEdit (macroCall,"\"","\\\"");
     std::string returnString = prefix + processedInputString + suffix;

  // printf ("In postponeMacroExpansion(): returnString = %s \n",returnString.c_str());

     counter++;
     return returnString;
   }


// *************************************************************************
// *************************************************************************
//             Functions that are not used any more (I think)
// *************************************************************************
// *************************************************************************

// endif for AST_REWRITE_MID_LEVEL_INTERFACE_TEMPLATES_C
#endif

